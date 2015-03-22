#include <iostream>
#include <fstream>
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/segmentation.h"
#include "utils/io/instance/segmentation.h"
#include "app/lexanalyzer/wordseg/action_utils.h"
#include "app/lexanalyzer/wordseg/pipe.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

namespace fe = ZuoPar::FrontEnd;

Pipe::Pipe(const fe::LearnOption& opts)
  : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
  if (load_model(opts.model_path)) { _INFO << "report: model is loaded."; }
  else { _INFO << "report: model is not loaded."; }
}

Pipe::Pipe(const fe::TestOption& opts)
  : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
  if (load_model(opts.model_path)) { _INFO << "report: model is loaded."; }
  else { _INFO << "report: model is not loaded."; }
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

bool Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  dataset.clear();
  if (mode == kPipeLearn) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt";
      return false;
    }
    _INFO << "report: loading dataset from reference file.";
    ioutils::read_segmentation_dataset(ifs, dataset, true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return false;
    }
    ioutils::read_segmentation_dataset(ifs, dataset, false);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  return true;
}

void Pipe::run() {
  namespace ioutils = ZuoPar::IO;
  if (!setup()) {
    return;
  }

  if (mode == kPipeLearn) {
    decoder = new Decoder(beam_size, false, weight);
    learner = new Learner(weight);
  } else {
    decoder = new Decoder(beam_size, true, weight);
  }
  size_t N = dataset.size();
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));
  for (size_t n = 0; n < N; ++ n) {
    const Segmentation& instance = dataset[n];
    // calculate the oracle transition actions.
    std::vector<Action> actions;
    if (mode == kPipeLearn) {
      ActionUtils::get_oracle_actions(instance, actions);
      for (const Action& a: actions) { _TRACE << a; }
    }

    int max_nr_actions = instance.size();
    State init_state(&instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    if (mode == kPipeLearn) {
      learner->set_timestamp(n+ 1);
      learner->learn(result.first, result.second);
    } else {
      Segmentation output;
      build_output((*result.first), output);
      ioutils::write_segmentation_instance((*os), output);
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode == kPipeLearn) {
    learner->set_timestamp(N);
    learner->flush();
    _INFO << "pipe: nr errors: " << learner->errors();
    save_model(model_path);
  }
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
