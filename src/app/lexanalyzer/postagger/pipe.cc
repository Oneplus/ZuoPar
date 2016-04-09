#include <iostream>
#include <fstream>
#include <boost/format.hpp>
#include "utils/misc.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/postag.h"
#include "utils/io/instance/postag.h"
#include "frontend/common_pipe_utils.h"
#include "app/lexanalyzer/postagger/action_utils.h"
#include "app/lexanalyzer/postagger/pipe.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

namespace ioutils = ZuoPar::IO;

Pipe::Pipe(const boost::program_options::variables_map& vm)
  : weight(new Weight), decoder(0), learner(0), conf(vm) {
  if (conf.count("model") && load_model(conf["model"].as<std::string>())) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::~Pipe() {
  if (weight)   { delete weight;  weight = 0; }
  if (decoder)  { delete decoder; decoder = 0; }
  if (learner)  { delete learner; learner = 0; }
}

bool Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exists.";
    return false;
  }

  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";
    return false;
  }

  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load weight.";
    return false;
  }

  return true;
}

bool Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save model."; return false; 
  } else {
    postags_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "pipe: model saved to " << model_path;
  }
  return true;
}

void Pipe::learn() {
  dataset.clear();
  devel_dataset.clear();

  std::ifstream ifs(conf["train"].as<std::string>());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file, training halt.";
    return;
  }

  _INFO << "report: loading dataset from training file.";
  ioutils::read_postag_dataset(ifs, dataset, postags_alphabet, 
    conf["delimiter"].as<char>(), true);
  _INFO << "report: training dataset is loaded from reference file.";
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";

  std::ifstream devel_ifs(conf["devel"].as<std::string>());
  if (devel_ifs.good()) {
    ioutils::read_postag_dataset(devel_ifs, devel_dataset,
      postags_alphabet, conf["delimiter"].as<char>(), false);
    _INFO << "report: develop dataset is loaded from reference file.";
    _INFO << "report: " << devel_dataset.size() << " instance(s) is loaded.";
  } else {
    _WARN << "Development data is not loaded";
  }

  decoder = new Decoder(postags_alphabet.size(), conf["beam"].as<unsigned>(), false,
    get_update_strategy(conf["update"].as<std::string>()),
    weight);
  learner = new Learner(weight);

  unsigned n_seen = 0, N = dataset.size();
  double best_score = 0.;
  std::string model_path = FrontEnd::get_model_name("pos", conf);

  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    _INFO << "pipe: iteration #" << iter + 1 << ", start training.";
    std::random_shuffle(dataset.begin(), dataset.end());
    for (const Postag& instance : dataset) {
      ++n_seen;
      std::vector<Action> actions;
      ActionUtils::get_oracle_actions(instance, actions);

      int max_actions = instance.size();
      State init_state(&instance);
      Decoder::const_decode_result_t result = 
        decoder->decode(init_state, actions, max_actions);

      learner->set_timestamp(n_seen);
      learner->learn(result.first, result.second);

      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "pipe: processed " << n_seen % N << "/" << n_seen / N << " instances.";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        double score = evaluate(devel_dataset);
        decoder->reset_use_avg();
        _INFO << "pipe: evaluate score: " << score;
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
  _INFO << "pipe: best development score: " << best_score;
}

double Pipe::evaluate(const std::vector<Postag>& dataset) {
  std::string output = FrontEnd::get_output_name("postagger", conf);
  std::ostream* os = ioutils::get_ostream(output); 
  decoder->set_use_avg();
  for (const Postag& instance : dataset) {
    std::vector<Action> actions;

    int max_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result = 
      decoder->decode(init_state, actions, max_actions);

    Postag output;
    build_output((*result.first), output);
    ioutils::write_postag_instance((*os), output, postags_alphabet, conf["delimiter"].as<char>());
  }
  _INFO << "pipe: processed #" << dataset.size() << " instances.";
  if (os == (&(std::cout))) { return 0.; }
  delete os;
  return Utility::execute_script(conf["script"].as<std::string>(), output);
}

void Pipe::test() {
  dataset.clear();
  // not implemented.
  std::ifstream ifs(conf["input"].as<std::string>());
  if (!ifs.good()) {
    _ERROR << "#: failed to open input file, training halt.";
    return;
  }
  ioutils::read_postag_dataset(ifs, dataset, postags_alphabet,
    conf["delimiter"].as<char>(), false);

  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";

  decoder = new Decoder(postags_alphabet.size(), conf["beam"].as<unsigned>(), true,
    get_update_strategy("naive"), weight);

  double score = evaluate(dataset);
  _INFO << "test: score " << score;
}

void Pipe::build_output(const State& source, Postag& output) {
  size_t len = source.ref->size();
  output.forms = source.ref->forms;
  output.postags.resize(len);
  for (size_t i = 0; i < len; ++ i) { output.postags[i] = source.postags[i]; }
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
