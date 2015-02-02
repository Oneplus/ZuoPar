#include <iostream>
#include <fstream>
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

Pipe::Pipe(const fe::LearnOption& opts)
  : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
  if (load_model(opts.model_path)) {
    _INFO << "report: model is loaded.";
  } else {
    _INFO << "report: model is not loaded.";
  }
}

Pipe::Pipe(const fe::TestOption& opts)
  : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
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
    ioutils::read_postag_with_cache_dataset(ifs, dataset, postags_alphabet, '/', true);
    _INFO << "report: dataset is loaded from reference file.";
  } else {
    // not implemented.
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt";
      return;
    }
    ioutils::read_postag_with_cache_dataset(ifs, dataset, postags_alphabet, '/', false);
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";

  if (mode == kPipeLearn) {
    decoder = new Decoder(postags_alphabet.size(), false, beam_size, weight);
    learner = new Learner(weight);
  } else {
    decoder = new Decoder(postags_alphabet.size(), true, beam_size, weight);
  }
  size_t N = dataset.size();
  std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));
  for (size_t n = 0; n < N; ++ n) {
    const PostagWithLiteralCache& instance = dataset[n];
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
      PostagWithLiteralCache output;
      build_output((*result.first), output);
      ioutils::write_postag_with_cache_instance((*os), output, postags_alphabet, '/');
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

    std::ofstream mfs(model_path);
    if (!mfs.good()) {
      _WARN << "pipe: failed to save model.";
    } else {
      //forms_alphabet.save(mfs);
      postags_alphabet.save(mfs);
      weight->save(mfs);
      _INFO << "pipe: model saved to " << model_path;
    }
  }
}

void
Pipe::build_output(const State& source, PostagWithLiteralCache& output) {
  size_t len = source.ref->size();

  output.cache.resize(len);
  output.postags.resize(len);
  for (size_t i = 0; i < len; ++ i) {
    output.cache[i] = source.ref->cache[i];
    output.postags[i] = source.postags[i];
  }
}


} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
