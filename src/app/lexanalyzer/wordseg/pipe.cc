#include <iostream>
#include <fstream>
#include "utils/misc.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/segmentation.h"
#include "utils/io/instance/segmentation.h"
#include "frontend/common_pipe_utils.h"
#include "app/lexanalyzer/wordseg/action_utils.h"
#include "app/lexanalyzer/wordseg/pipe.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

namespace fe = ZuoPar::FrontEnd;

Pipe::Pipe(const boost::program_options::variables_map& vm) 
  : weight(new Weight), decoder(0), learner(0), conf(vm) {
  if (vm.count("model") && load_model(vm["model"].as<std::string>())) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::~Pipe() {
  if (weight)   { delete weight;  weight = nullptr;   }
  if (decoder)  { delete decoder; decoder = nullptr;  }
  if (learner)  { delete learner; learner = nullptr;  }
}

bool Pipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);
  if (!mfs.good()) { _WARN << "pipe: model doesn't exists."; return false; }
  if (!weight->load(mfs)) { _WARN << "pipe: failed to load weight."; return false; }
  return true;
}

bool Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) { _WARN << "pipe: failed to save model."; return false; }
  else { weight->save(mfs); _INFO << "pipe: model saved to " << model_path; }
  return true;
}


void Pipe::learn() {
  namespace ioutils = ZuoPar::IO;

  std::ifstream ifs(conf["train"].as<std::string>());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file.";
    _ERROR << "#: training halt";
    return;
  }
  _INFO << "report: loading dataset from reference file.";
  ioutils::read_segmentation_dataset(ifs, dataset, true);
  _INFO << "report: loaded " << dataset.size() << " training instance(s).";
  
  if (conf.count("devel")) {
    std::ifstream devel_ifs(conf["devel"].as<std::string>());
    if (!devel_ifs.good()) {
      _WARN << "report: Failed to load development data.";
    } else {
      ioutils::read_segmentation_dataset(devel_ifs, devel_dataset, false);
      _INFO << "report: loaded " << devel_dataset.size() << " development instance(s).";
    }
  }

  decoder = new Decoder(conf["beam"].as<unsigned>(), false, weight);
  learner = new Learner(weight);

  std::string model_path = FrontEnd::get_model_name("cws", conf);
  double best_score = 0.;

  unsigned n_seen = 0, N = dataset.size();
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    for (const Segmentation& instance : dataset) {
      ++n_seen;
      std::vector<Action> actions;
      ActionUtils::get_oracle_actions(instance, actions);
      for (const Action& a : actions) { _TRACE << a; }

      int max_nr_actions = instance.size();
      State init_state(&instance);
      Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

      learner->set_timestamp(n_seen);
      learner->learn(result.first, result.second);

      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "pipe: processed #" << n_seen % N << "/" << n_seen / N << " instances.";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        double score = evaluate(devel_dataset);
        decoder->reset_use_avg();
        _INFO << "pipe: evaluate at instance#" << n_seen << ", score: " << score;
        if (score > best_score) {
          _INFO << "pipe: NEW best model is achieved, save to " << model_path;
          save_model(model_path);
          best_score = score;
        }
      }
    }
    learner->flush();
    _INFO << "pipe: iter" << iter + 1 << " #errros: " << learner->errors();
    learner->clear_errors();
    double score = evaluate(devel_dataset);
    decoder->reset_use_avg();
    _INFO << "pipe: evaluate at the end of iteration#" << iter + 1 << " score: " << score;
    if (score > best_score) {
      _INFO << "pipe: NEW best model is achieved, save to " << model_path;
      save_model(model_path);
      best_score = score;
    }
  }
  _INFO << "pipe: end of training, best development score: " << best_score;
}

double Pipe::evaluate(const std::vector<Segmentation>& dataset) {
  namespace ioutils = ZuoPar::IO;

  std::string output = FrontEnd::get_output_name("wordseg", conf);
  std::ostream* os = ioutils::get_ostream(output);
  decoder->set_use_avg();
  for (const Segmentation& instance : dataset) {
    std::vector<Action> actions;

    int max_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result =
      decoder->decode(init_state, actions, max_actions);

    Segmentation output;
    build_output((*result.first), output);
    ioutils::write_segmentation_instance((*os), output);
  }
  _INFO << "pipe: processed #" << dataset.size() << " instances.";
  if (os == (&(std::cout))) { return 0.; }
  delete os;
  return Utility::execute_script(conf["script"].as<std::string>(), output);
}

void Pipe::test() {
  namespace ioutils = ZuoPar::IO;
  dataset.clear();
  // not implemented.
  std::ifstream ifs(conf["input"].as<std::string>());
  if (!ifs.good()) {
    _ERROR << "#: failed to open input file, training halt.";
    return;
  }
  ioutils::read_segmentation_dataset(ifs, dataset, false);
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";

  decoder = new Decoder(conf["beam"].as<unsigned>(), true, weight);

  double score = evaluate(dataset);
  _INFO << "test: score " << score;
}

void Pipe::build_output(const State& source, Segmentation& output) {
  output.chars = source.ref->chars;
  std::vector<Action> action_sequence;
  const State* p = &source;
  while (p->previous) { action_sequence.push_back(p->last_action); p = p->previous; }
  std::reverse(action_sequence.begin(), action_sequence.end());
  std::string word = output.chars[0];
  for (auto i = 0; i < action_sequence.size()- 1; ++ i) {
    if (ActionUtils::is_append(action_sequence[i])) {
      word += output.chars[i+ 1];
    } else {
      output.words.push_back(word);
      word = output.chars[i+ 1];
    }
  }
  output.words.push_back(word);
}

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
