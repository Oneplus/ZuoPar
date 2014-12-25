#include <iostream>
#include <fstream>
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/sequence_instance.h"
#include "utils/io/instance/sequence_instance.h"
#include "app/seqlabeler/action_utils.h"
#include "app/seqlabeler/pipe.h"

namespace ZuoPar {
namespace SequenceLabeler {

Pipe::Pipe(const LearnOption& opts)
  : mode(kPipeLearn), weight(0), decoder(0), learner(0) {
  _INFO << "::LEARN:: mode is activated.";

  this->reference_path = opts.reference_path;
  this->model_path = opts.model_path;
  this->beam_size = opts.beam_size;
  this->display_interval = opts.display_interval;
  this->algorithm = Learner::kAveragePerceptron;
  if (opts.algorithm == "pa") {
    this->algorithm = Learner::LearningAlgorithm::kPassiveAgressive;
  }
  this->early_update = opts.early_update;

  _INFO << "report: model file is " << opts.model_path;
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

  if (!attributes_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return false;
  }

  if (!tags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load tags alphabet.";
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
    _INFO << "report: beam size = " << this->beam_size;
    _INFO << "report: early update = " << (this->early_update ? "true": "false");
    _INFO << "report: learning algorithm = " << (
        this->algorithm == Learner::kAveragePerceptron ?
        "averaged perceptron": "passive aggressive");

    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt";
      return;
    }
    _INFO << "report: loading dataset from reference file.";
    ioutils::read_sequence_instance_dataset(ifs, dataset, attributes_alphabet,
        tags_alphabet, true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    // not implemented.
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return;
    }
    ioutils::read_sequence_instance_dataset(ifs, dataset, attributes_alphabet,
        tags_alphabet, false);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << attributes_alphabet.size() << " attribute(s) is detected.";
  _INFO << "report: " << tags_alphabet.size() << " tag(s) is detected.";

  decoder = new Decoder(tags_alphabet.size(), beam_size, early_update, weight);

  if (mode == kPipeLearn) {
    learner = new Learner(weight, this->algorithm);
  }
  size_t N = dataset.size();
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));
  for (size_t n = 0; n < N; ++ n) {
    const SequenceInstance& instance = dataset[n];
    // calculate the oracle transition actions.
    std::vector<Action> actions;
    if (mode == kPipeLearn) {
      ActionUtils::get_oracle_actions(instance, actions);
    }

    int max_nr_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    if (mode == kPipeLearn) {
      learner->set_timestamp(n+ 1);
      learner->learn(result.first, result.second);
    } else {
      SequenceInstance output;
      build_output((*result.first), output);
      ioutils::write_sequence_instance((*os), output, attributes_alphabet, tags_alphabet);
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
      attributes_alphabet.save(mfs);
      tags_alphabet.save(mfs);
      weight->save(mfs);
      _INFO << "pipe: model saved to " << model_path;
    }
  }
}

void
Pipe::build_output(const State& source, SequenceInstance& output) {
  size_t len = source.ref->size();

  output.resize(len);
  for (size_t i = 0; i < len; ++ i) {
    output[i].tag = source.tags[i];
  }
}


} //  end for namespace sequencelabeler
} //  end for namespace zuopar
