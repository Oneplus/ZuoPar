#include <iostream>
#include <fstream>
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "utils/io/instance/dependency.h"
#include "app/depparser/arceager/action_utils.h"
#include "app/depparser/arceager/pipe.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

Pipe::Pipe(const LearnOption& opts)
  : mode(kPipeLearn), weight(0), decoder(0), learner(0) {
  _INFO << "::LEARN:: mode is actived.";
  _INFO << "report: model file is " << opts.model_path;
  _INFO << "report: reference file is " << opts.reference_path;
  _INFO << "report: beam size is " << opts.beam_size;

  this->reference_path = opts.reference_path;
  this->model_path = opts.model_path;
  this->beam_size = opts.beam_size;
  this->display_interval = opts.display_interval;
  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::Pipe(const TestOption& opts)
  : mode(kPipeTest), weight(0), decoder(0), learner(0) {
  BOOST_LOG_TRIVIAL(info) << "::TEST:: mode is actived.";
  this->model_path = opts.model_path;
  this->input_path = opts.input_path;
  this->output_path = opts.output_path;
  this->beam_size = opts.beam_size;
  this->display_interval = opts.display_interval;
  _INFO << "report: model file is " << opts.model_path;
  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

bool
Pipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exists.";
    return false;
  }

  if (!forms_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return false;
  }

  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";
    return false;
  }

  if (!deprels_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load deprels alphabet.";
    return false;
  }

  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load weight.";
    return false;
  }

  return true;
}

void
Pipe::run() {
  namespace ioutils = ZuoPar::IO;

  dataset.clear();
  if (mode == kPipeLearn) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt";
      return;
    }

    _INFO << "report: loading dataset from reference file.";
    ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet, true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    // not implemented.
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return;
    }
    ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet, true);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << forms_alphabet.size() << " forms(s) is detected.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is detected.";

  decoder = new Decoder(deprels_alphabet.size(), beam_size, weight);

  if (mode == kPipeLearn) {
    learner = new Learner(weight);
  }
  size_t N = dataset.size();
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));
  for (size_t n = 0; n < N; ++ n) {
    const Dependency& instance = dataset[n];
    // calculate the oracle transition actions.
    std::vector<Action> actions;
    if (mode == kPipeLearn) {
      ActionUtils::get_oracle_actions(instance, actions);
    }

    int max_nr_actions = instance.size() * 2 - 1;
    State init_state(&instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    if (mode == kPipeLearn) {
      learner->set_timestamp(n+ 1);
      learner->learn(result.first, result.second);
    } else {
      Dependency output;
      for (int i = 0; i < instance.size(); ++ i) {
        output.push_back(instance.forms[i], instance.postags[i], 0, 0);
      }
      build_output((*result.first), output);
      ioutils::write_dependency_instance((*os),
          output,
          forms_alphabet,
          postags_alphabet,
          deprels_alphabet);
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode == kPipeLearn) {
    learner->set_timestamp(N);
    learner->flush();

    std::ofstream mfs(model_path);
    if (!mfs.good()) {
      _WARN << "pipe: failed to save model.";
    } else {
      forms_alphabet.save(mfs);
      postags_alphabet.save(mfs);
      deprels_alphabet.save(mfs);
      weight->save(mfs);
      _INFO << "pipe: model saved to " << model_path;
    }
  }
}

void
Pipe::build_output(const State& source, Dependency& output) {
  size_t len = source.ref->size();
  if (output.heads.size() != len) {
    output.heads.resize(len);
  }

  if (output.deprels.size() != len) {
    output.deprels.resize(len);
  }

  for (size_t i = 0; i < len; ++ i) {
    output.heads[i] = source.heads[i];
    output.deprels[i] = source.deprels[i];
    if (output.heads[i] == -1) {
      output.deprels[i] = deprels_alphabet.encode("ROOT");
    }
  }
}


} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
