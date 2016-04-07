#include <iostream>
#include <fstream>
#include "utils/misc.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/postag.h"
#include "utils/io/instance/postag.h"
#include "app/lexanalyzer/postagger/action_utils.h"
#include "app/lexanalyzer/postagger/pipe.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

namespace fe = ZuoPar::FrontEnd;


Pipe::Pipe(const boost::program_options::variables_map& vm)
  : weight(0), decoder(0), learner(0), conf(vm) {
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
  weight = new Weight;
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

void Pipe::learn() {
  namespace ioutils = ZuoPar::IO;
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

  unsigned n_seen = 0, tot_seen = conf["maxiter"].as<unsigned>() * dataset.size();
  double best_score = 0.;
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    _INFO << "pipe: iteration #" << iter + 1 << ", start training.";
    std::random_shuffle(dataset.begin(), dataset.end());
    size_t N = dataset.size();
    for (const Postag& instance : dataset) {
      ++n_seen;
      std::vector<Action> actions;
      ActionUtils::get_oracle_actions(instance, actions);

      int max_nr_actions = instance.size();
      State init_state(&instance);
      Decoder::const_decode_result_t result = 
        decoder->decode(init_state, actions, max_nr_actions);

      learner->set_timestamp(n_seen);
      learner->learn(result.first, result.second);

      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "pipe: processed " << double(n_seen) / tot_seen  << "% instances.";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        double score = evaluate(devel_dataset);
        _INFO << "pipe: evaluate score: " << score;
        if (score > best_score) {
          _INFO << "pipe: new best model is achieved, save to " << conf["model"].as<std::string>();
          best_score = score;
        }
      }
    }
    double score = evaluate(devel_dataset);
    _INFO << "pipe: evaluate score: " << score;
    if (score > best_score) {
      _INFO << "pipe: new best model is achieved, save to " << conf["model"].as<std::string>();
      best_score = score;
    }
  }
}

double Pipe::evaluate(const std::vector<Postag>& dataset) {
  namespace ioutils = ZuoPar::IO;

  std::string output;
  if (conf.count("output")) {
    output = conf["output"].as<std::string>();
  } else {
    output = "postagger.output" + boost::lexical_cast<std::string>(Utility::get_pid());
  }

  std::ostream* os = ioutils::get_ostream(output); 
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
  return Utility::execute_script(conf["script"].as<std::string>(), conf["output"].as<std::string>());
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
  ioutils::read_postag_dataset(ifs, dataset, postags_alphabet,
    conf["delimiter"].as<char>(), false);

  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";

  decoder = new Decoder(postags_alphabet.size(), conf["beam"].as<unsigned>(), true,
    get_update_strategy("naive"), weight);

  double score = evaluate(dataset);
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
