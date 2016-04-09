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
    _INFO << "[RPT] model is loaded.";
  } else {
    _INFO << "[RPT] model is not loaded.";
  }
}

Pipe::~Pipe() {
  if (weight)   { delete weight;  weight = nullptr;   }
  if (decoder)  { delete decoder; decoder = nullptr;  }
  if (learner)  { delete learner; learner = nullptr;  }
}

bool Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);
  if (!mfs.good()) { _WARN << "[PIP] model doesn't exists."; return false; }
  if (!weight->load(mfs)) { _WARN << "[PIP] failed to load weight."; return false; }
  return true;
}

bool Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) { _WARN << "[PIP] failed to save model."; return false; }
  else { weight->save(mfs); _INFO << "[PIP] model saved to " << model_path; }
  return true;
}

bool Pipe::setup(const std::string& path, std::vector<Segmentation>& ds, bool insert) {
  ds.clear();
  std::ifstream ifs(path);
  if (!ifs.good()) {
    return false;
  }
  _INFO << "[PIP] alphabet is " << (insert ? "open" : "closed") << ".";
  _INFO << "[PIP] loading dataset from: " << path;
  IO::read_segmentation_dataset(ifs, ds, insert);
  _INFO << "[PIP] dataset is loaded with " << ds.size() << " instance(s)";
  return true;
}

void Pipe::learn() {
  if (!setup(conf["train"].as<std::string>(), dataset, true)) {
    _ERROR << "#: failed to open reference file, training halt.";
    return;
  }
  if (!conf.count("devel") || !setup(conf["devel"].as<std::string>(), devel_dataset, false)) {
    _WARN << "[PIP] development data is not loaded";
  }

  decoder = new Decoder(conf["beam"].as<unsigned>(), false,
    get_update_strategy(conf["update"].as<std::string>()), weight);
  learner = new Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

  std::string model_path = FrontEnd::get_model_name("cws", conf);
  double best_score = 0.;

  unsigned n_seen = 0, N = dataset.size();
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    _INFO << "[PIP] iteration " << iter + 1 << ", start training.";
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
        _INFO << "[PIP] processed #" << n_seen % N << "/" << n_seen / N << " instances.";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        double score = evaluate(devel_dataset);
        decoder->reset_use_avg();
        _INFO << "[PIP] evaluate at instance#" << n_seen << ", score: " << score;
        if (score > best_score) {
          _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
          save_model(model_path);
          best_score = score;
        }
      }
    }
    learner->flush();
    _INFO << "[PIP] iteration " << iter + 1 << " #errros: " << learner->errors();
    learner->clear_errors();
    double score = evaluate(devel_dataset);
    decoder->reset_use_avg();
    _INFO << "[PIP] evaluate at the end of iteration#" << iter + 1 << " score: " << score;
    if (score > best_score) {
      _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
      save_model(model_path);
      best_score = score;
    }
  }
  _INFO << "[PIP] end of training, best development score: " << best_score;
}

double Pipe::evaluate(const std::vector<Segmentation>& dataset) {
  std::string output = FrontEnd::get_output_name("wordseg", conf);
  std::ostream* os = IO::get_ostream(output);
  decoder->set_use_avg();
  for (const Segmentation& instance : dataset) {
    std::vector<Action> actions;

    int max_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result =
      decoder->decode(init_state, actions, max_actions);

    Segmentation output;
    build_output((*result.first), output);
    IO::write_segmentation_instance((*os), output);
  }
  _INFO << "[PIP] processed #" << dataset.size() << " instances.";
  if (os == (&(std::cout))) { return 0.; }
  delete os;
  return Utility::execute_script(conf["script"].as<std::string>(), output);
}

void Pipe::test() {
  if (!setup(conf["input"].as<std::string>(), dataset, false)) {
    _ERROR << "#: failed to open input file, training halt.";
    return;
  }

  decoder = new Decoder(conf["beam"].as<unsigned>(), true, UpdateStrategy::kNaive, weight);
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
