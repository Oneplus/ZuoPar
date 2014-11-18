#include <iostream>
#include <fstream>
#include "utils/logging.h"
#include "utils/io/dataset.h"
#include "app/depparser/arcstandard/action_utils.h"
#include "app/depparser/arcstandard/pipe.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

Pipe::Pipe(const LearnOption& opts)
  : mode(kPipeLearn), weight(0), decoder(0), learner(0) {
  _INFO << "::LEARN:: mode is actived.";
  _INFO << "report: model file is " << opts.model_path;
  _INFO << "report: reference file is " << opts.reference_path;
  _INFO << "report: beam size is " << opts.beam_size;

  this->reference_path = opts.reference_path;
  this->model_path = opts.model_path;
  this->beam_size = opts.beam_size;
  load_model(opts.model_path);
  _INFO << "report: model is loaded.";
}

Pipe::Pipe(const TestOption& opts)
  : mode(kPipeTest), weight(0), decoder(0), learner(0) {
  BOOST_LOG_TRIVIAL(info) << "::TEST:: mode is actived.";
  this->model_path = opts.model_path;
  _INFO << "report: model file is " << opts.model_path;
  load_model(opts.model_path);
  _INFO << "report: model is loaded.";
}

void
Pipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exists.";
    return;
  }

  if (!forms_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return;
  }

  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";
    return;
  }

  if (!deprels_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load deprels alphabet.";
    return;
  }

  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load weight.";
    return;
  }
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
    ioutils::read_plain_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet);
    _INFO << "report: dataset is loaded from reference file.";
    _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
    _INFO << "report: " << forms_alphabet.size() << " forms(s) is detected.";
    _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
    _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is detected.";
  } else {
    // not implemented.
  }

  decoder = new Decoder(deprels_alphabet.size(), beam_size, weight);

  if (mode == kPipeLearn) {
    learner = new Learner(weight);
  }
  size_t N = dataset.size();
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
    }
  }

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

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
