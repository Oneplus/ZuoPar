#ifndef __ZUOPAR_APP_DEPPARSER_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_PIPE_H__

#include <iostream>
#include <fstream>
#include <algorithm>
#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "utils/io/instance/dependency.h"
#include "frontend/common_pipe_cfg.h"
#include "app/depparser/opt.h"

namespace ZuoPar {
namespace DependencyParser {

namespace fe = ZuoPar::FrontEnd;
namespace eg = ZuoPar::Engine;

template <
  class Action,
  class ActionUtils,
  class State,
  class Weight,
  class Decoder,
  class Learner
>
class CommonDependencyPipe: public fe::CommonPipeConfigure {
protected:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CommonDependencyPipe(const fe::LearnOption& opts)
    : weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(opts) {
    if (load_model(opts.model_path)) {
      _INFO << "report: model is loaded.";
    } else {
      _INFO << "report: model is not loaded.";
    }
  }
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CommonDependencyPipe(const LearnOption& opts)
    : weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path)) {
      _INFO << "report: model is loaded.";
    } else {
      _INFO << "report: model is not loaded.";
    }
  }

  /**
   * The testing mode constructor.
   *
   *  @param[in]  opts  The testing options.
   */
  CommonDependencyPipe(const TestOption& opts)
    : weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path)) {
      _INFO << "report: model is loaded.";
    } else {
      _INFO << "report: model is not loaded.";
    }
  }

  bool setup() {
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
      ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
          postags_alphabet, deprels_alphabet, true);
      _INFO << "report: dataset is loaded from reference file.";
    } else {
      std::ifstream ifs(input_path.c_str());
      if (!ifs.good()) {
        _ERROR << "#: failed to open input file.";
        _ERROR << "#: testing halt";
        return false;
      }
      ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
          postags_alphabet, deprels_alphabet, true);
    }
    _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
    _INFO << "report: " << forms_alphabet.size() << " forms(s) is detected.";
    _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
    _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is detected.";
    return true;
  }

  //! Perform learning or testing according to the configuration.
  void run() {
    namespace ioutils = ZuoPar::IO;
    if (!setup()) {
      return;
    }

    deprel_t root_tag = deprels_alphabet.encode(this->root.c_str());
    if (mode == kPipeLearn) {
      decoder = new Decoder(deprels_alphabet.size(), root_tag,
          beam_size, false, update_strategy, weight);
      learner = new Learner(weight, this->algorithm);
    } else {
      decoder = new Decoder(deprels_alphabet.size(), root_tag,
          beam_size, true, update_strategy, weight);
    }

    size_t N = dataset.size();
    std::ostream* os = (mode == kPipeLearn ? NULL: ioutils::get_ostream(output_path.c_str()));
    std::vector<std::size_t> ranks;
    for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
    while (this->shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

    for (size_t n = 0; n < N; ++ n) {
      const Dependency& instance = dataset[ranks[n]];
      // calculate the oracle transition actions.
      std::vector<Action> actions;
      if (mode == kPipeLearn) {
        ActionUtils::get_oracle_actions(instance, actions);
      }

      int max_nr_actions = instance.size() * 2 - 1;
      State init_state(&instance);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);

      if (mode == kPipeLearn) {
        learner->set_timestamp(n+ 1);
        learner->learn(result.first, result.second);
      } else {
        Dependency output;
        build_output((*result.first), output);
        ioutils::write_dependency_instance((*os), output, forms_alphabet,
            postags_alphabet, deprels_alphabet);
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

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path) {
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

  /**
   * Save model to the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  void save_model(const std::string& model_path) {
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

  /**
   * Build the dependency output for the state chain which ends with the source
   * state.
   *
   *  @param[in]  source  The last state in state chain.
   *  @param[out] output  The output dependency parse.
   */
  void build_output(const State& source, Dependency& output) {
    size_t len = source.ref->size();
    output.forms = source.ref->forms;
    output.postags = source.ref->postags;
    output.heads.resize(len);
    output.deprels.resize(len);

    for (size_t i = 0; i < len; ++ i) {
      output.heads[i] = source.heads[i];
      output.deprels[i] = source.deprels[i];
      if (output.heads[i] == -1) {
        output.deprels[i] = deprels_alphabet.encode(root.c_str());
      }
    }
  }

protected:
  //! The pointer to the weights instances which is pointwise averaged
  //! perceptron model.
  Weight* weight;

  //! The parameter learner.
  Learner* learner;

  //! The pointer to the decoder.
  Decoder* decoder;

  //! The alphabets of forms.
  eg::TokenAlphabet forms_alphabet;

  //! The alphabets of postags.
  eg::TokenAlphabet postags_alphabet;

  //! The alphabets of dependency relations.
  eg::TokenAlphabet deprels_alphabet;

  //! The dataset.
  std::vector<Dependency> dataset;

  std::string root;
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_PIPE_H__
