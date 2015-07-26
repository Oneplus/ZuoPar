#ifndef __ZUOPAR_APP_DEPPARSER_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_PIPE_H__

#include <iostream>
#include <fstream>
#include <algorithm>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "types/dependency.h"
#include "types/internal/packed_scores.h"
#include "engine/token_alphabet.h"
#include "utils/misc.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "utils/io/instance/dependency.h"
#include "frontend/common_pipe_cfg.h"
#include "app/depparser/opt.h"
#include "app/depparser/greedy_opt.h"

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
  class Learner,
  class MaxNumberOfActionsFunction
>
class DependencyPipe: public fe::CommonPipeConfigure {
protected:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  DependencyPipe(const fe::LearnOption& opts)
    : weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(opts) {
    if (load_model(opts.model_path)) { _INFO << "report: model is loaded.";}
    else                             { _INFO << "report: model is not loaded.";}
  }
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  DependencyPipe(const LearnOption& opts): weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path)) { _INFO << "report: model is loaded.";}
    else                             { _INFO << "report: model is not loaded.";}
  }

  /**
   * The testing mode constructor.
   *
   *  @param[in]  opts  The testing options.
   */
  DependencyPipe(const TestOption& opts): weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path)) { _INFO << "report: model is loaded.";}
    else                             { _INFO << "report: model is not loaded."; }
  }

  ~DependencyPipe() {
    if (weight)  { delete weight;  weight = 0;  }
    if (decoder) { delete decoder; decoder = 0; }
    if (learner) { delete learner; learner = 0; }
  }

  bool setup() {
    dataset.clear();
    if (mode == kPipeLearn) {
      std::ifstream ifs(reference_path.c_str());
      if (!ifs.good()) {
        _ERROR << "#: failed to open reference file.";
        _ERROR << "#: training halt";
        return false;
      }

      _INFO << "report: loading dataset from reference file.";
      IO::read_dependency_dataset(ifs, dataset, forms_alphabet,
          postags_alphabet, deprels_alphabet);
      _INFO << "report: dataset is loaded from reference file.";
    } else {
      std::ifstream ifs(input_path.c_str());
      if (!ifs.good()) {
        _ERROR << "#: failed to open input file.";
        _ERROR << "#: testing halt";
        return false;
      }
      IO::read_dependency_dataset(ifs, dataset, forms_alphabet,
          postags_alphabet, deprels_alphabet, 0x03);
    }
    _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
    _INFO << "report: " << forms_alphabet.size() << " forms(s) is detected.";
    _INFO << "report: " << postags_alphabet.size() << " postag(s) is detected.";
    _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is detected.";
    return true;
  }

  //! Perform learning or testing according to the configuration.
  void run() {
    if (!setup()) {
      return;
    }

    deprel_t root_tag = deprels_alphabet.insert(this->root);
    if (mode == kPipeLearn) {
      decoder = new Decoder(deprels_alphabet.size(), root_tag,
          beam_size, false, update_strategy, weight);
      learner = new Learner(weight, this->algorithm);
    } else {
      decoder = new Decoder(deprels_alphabet.size(), root_tag,
          beam_size, true, UpdateStrategy::kNaive, weight);
    }

    size_t N = dataset.size();
    std::ostream* os = (mode == kPipeLearn ? NULL: IO::get_ostream(output_path.c_str()));
    std::vector<std::size_t> ranks;
    Utility::shuffle(N, this->shuffle_times, ranks);

    for (size_t n = 0; n < N; ++ n) {
      const Dependency& instance = dataset[ranks[n]];
      // calculate the oracle transition actions.
      std::vector<Action> actions;
      if (mode == kPipeLearn) {
        ActionUtils::get_oracle_actions(instance, actions);
      }

      int max_nr_actions = MaxNumberOfActionsFunction()(instance);
      State init_state(&instance);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);

      if (mode == kPipeLearn) {
        learner->set_timestamp(n+ 1);
        //if (result.first != result.second) { _TRACE << "failed at: " << result.first->buffer; }
        learner->learn(result.first, result.second);
      } else {
        Dependency output;
        build_output((*result.first), output);
        IO::write_dependency_instance((*os), output, forms_alphabet,
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

    if (os != &(std::cout) && os != NULL) { delete os; }
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
      _WARN << "pipe: model doesn't exists.";             return false; }
    if (!forms_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load forms alphabet.";    return false; }
    if (!postags_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load postags alphabet.";  return false; }
    if (!deprels_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load deprels alphabet.";  return false; }
    if (!weight->load(mfs)) {
      _WARN << "pipe: failed to load weight.";            return false; }
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
  Weight* weight;     //! The parameter
  Learner* learner;   //! The parameter learner.
  Decoder* decoder;   //! The pointer to the decoder.
  eg::TokenAlphabet forms_alphabet;     //! The alphabets of forms.
  eg::TokenAlphabet postags_alphabet;   //! The alphabets of postags.
  eg::TokenAlphabet deprels_alphabet;   //! The alphabets of dependency relations.
  std::vector<Dependency> dataset;      //! The dataset.
  std::string root;
};

// CoNLLX pipe -------------

class CoNLLXDependencyRepository {
public:
  bool setup(const std::string& path, bool insert) {
    dataset.clear();
    std::ifstream ifs(path.c_str());
    if (insert) {
      if (!ifs.good()) {
        _ERROR << "#: failed to open reference file (training halt).";
        return false;
      }

      _INFO << "report: loading dataset from reference file.";
      IO::read_conllx_dependency_dataset(ifs, dataset, forms_alphabet,
          lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
          deprels_alphabet);
      _INFO << "report: dataset is loaded from reference file.";
    } else {
      if (!ifs.good()) {
        _ERROR << "#: failed to open input file.";
        _ERROR << "#: testing halt";
        return false;
      }
      IO::read_conllx_dependency_dataset(ifs, dataset, forms_alphabet,
          lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
          deprels_alphabet, 0x1f);
    }
    _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
    _INFO << "report: " << forms_alphabet.size()    << " form(s) is detected.";
    _INFO << "report: " << lemmas_alphabet.size()   << " lemmas(s) is detected.";
    _INFO << "report: " << cpostags_alphabet.size() << " cpostag(s) is detected.";
    _INFO << "report: " << postags_alphabet.size()  << " postag(s) is detected.";
    _INFO << "report: " << feat_alphabet.size()     << " feat(s) is detected.";
    _INFO << "report: " << deprels_alphabet.size()  << " deprel(s) is detected.";
    return true;
  }

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(std::ifstream& mfs) {
    if (!mfs.good()) {
      _WARN << "pipe: model doesn't exists.";             return false; }
    if (!forms_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load forms alphabet.";    return false; }
    if (!lemmas_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load lemmas alphabet.";   return false; }
    if (!cpostags_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load cpostag alphabet.";  return false; }
    if (!postags_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load postags alphabet.";  return false; }
    if (!feat_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load feat alphabet.";     return false; }
    if (!deprels_alphabet.load(mfs)) {
      _WARN << "pipe: failed to load deprels alphabet.";  return false; }
    return true;
  }

  /**
   * Save model to the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  void save_model(std::ofstream& mfs) {
    forms_alphabet.save(mfs);
    lemmas_alphabet.save(mfs);
    cpostags_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    feat_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
  }

  /**
   * Build output from the source state.
   *
   *  @param[in]  source  The source state.
   *  @param[in]  output  The dependency instance.
   */
  template<class State>
  void build_output(const State& source, CoNLLXDependency& output,
      deprel_t root_deprel) {
    size_t len = source.ref->size();
    output.forms = source.ref->forms;
    output.lemmas = source.ref->lemmas;
    output.cpostags = source.ref->cpostags;
    output.postags = source.ref->postags;
    output.feats = source.ref->feats;

    output.heads.resize(len);
    output.deprels.resize(len);
    for (size_t i = 0; i < len; ++ i) {
      output.heads[i] = source.heads[i];
      output.deprels[i] = source.deprels[i];
      if (output.heads[i] == -1) { output.deprels[i] = root_deprel; }
    }
  }

public:
  Engine::TokenAlphabet forms_alphabet;     //! The alphabet for forms.
  Engine::TokenAlphabet lemmas_alphabet;    //! The alphabet for lemmas.
  Engine::TokenAlphabet cpostags_alphabet;  //! The alphabet for cpostags.
  Engine::TokenAlphabet postags_alphabet;   //! The alphabet for postags.
  Engine::TokenAlphabet feat_alphabet;      //! The alphabet for feat.
  Engine::TokenAlphabet deprels_alphabet;   //! The alphabet for dependency relations.
  std::vector<CoNLLXDependency> dataset;
};


template <
  class Action,
  class ActionUtils,
  class State,
  class Weight,
  class Decoder,
  class Learner,
  class MaxNumberOfActionsFunction
>
class CoNLLXDependencyPipe:
  public fe::CommonPipeConfigure,
  public CoNLLXDependencyRepository {
protected:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CoNLLXDependencyPipe(const fe::LearnOption& opts)
    : weight(0), decoder(0), learner(0), fe::CommonPipeConfigure(opts) {
    if (load_model(opts.model_path)) { _INFO << "report: model is loaded."; }
    else                             { _INFO << "report: model is not loaded."; }
  }
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CoNLLXDependencyPipe(const LearnOption& opts): weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path))  { _INFO << "report: model is loaded."; }
    else                              { _INFO << "report: model is not loaded."; }
  }

  /**
   * The testing mode constructor.
   *
   *  @param[in]  opts  The testing options.
   */
  CoNLLXDependencyPipe(const TestOption& opts): weight(0), decoder(0), learner(0),
    fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)) {
    this->root = opts.root;
    if (load_model(opts.model_path))  { _INFO << "report: model is loaded."; }
    else                              { _INFO << "report: model is not loaded."; }
  }

  ~CoNLLXDependencyPipe() {
    if (weight)  { delete weight;  weight = 0;  }
    if (decoder) { delete decoder; decoder = 0; }
    if (learner) { delete learner; learner = 0; }
  }

  //! Perform learning or testing according to the configuration.
  void run() {
    if (mode == kPipeLearn) {
      if (!setup(reference_path, true)) { return; }
    } else if (mode == kPipeTest) {
      if (!setup(input_path, false)) { return; }
    }

    deprel_t root_tag = deprels_alphabet.insert(this->root);
    if (mode == kPipeLearn) {
      decoder = new Decoder(deprels_alphabet.size(), root_tag, Decoder::kLeft,
          beam_size, false, update_strategy, weight);
      learner = new Learner(weight, this->algorithm);
    } else {
      decoder = new Decoder(deprels_alphabet.size(), root_tag, Decoder::kLeft,
          beam_size, true, UpdateStrategy::kNaive, weight);
    }

    size_t N = dataset.size();
    std::ostream* os = (mode == kPipeLearn ? NULL: IO::get_ostream(output_path.c_str()));
    std::vector<std::size_t> ranks;
    for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
    while (this->shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

    for (size_t n = 0; n < N; ++ n) {
      const CoNLLXDependency& instance = dataset[ranks[n]];
      // calculate the oracle transition actions.
      std::vector<Action> actions;
      if (mode == kPipeLearn) { ActionUtils::get_oracle_actions(instance, actions);  }

      int max_nr_actions = MaxNumberOfActionsFunction()(instance);
      State init_state(&instance);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);

      if (mode == kPipeLearn) {
        learner->set_timestamp(n+ 1);
        //if (result.first != result.second) { _TRACE << "failed at: " << result.first->buffer; }
        learner->learn(result.first, result.second);
      } else {
        CoNLLXDependency output;
        build_output((*result.first), output, deprels_alphabet.encode(root));
        IO::write_conllx_dependency_instance((*os), output, forms_alphabet,
            lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
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
      _INFO << "pipe: nr errors: " << learner->errors();
      save_model(model_path);
    }

    if (os != &(std::cout) && os != NULL) { delete os; }
  }

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path) {
    weight = new Weight;
    std::ifstream mfs(model_path);

    if (!CoNLLXDependencyRepository::load_model(mfs)) {
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
      CoNLLXDependencyRepository::save_model(mfs);
      weight->save(mfs);
      _INFO << "pipe: model saved to " << model_path;
    }
  }

protected:
  Weight* weight;     //! The parameter
  Learner* learner;   //! The parameter learner.
  Decoder* decoder;   //! The pointer to the decoder.
  std::string root;
};

template <
  class Action,
  class ActionUtils,
  class State,
  class Weight,
  class Decoder,
  class Learner,
  class MaxNumberOfActionsFunction
>
class GreedySearchCoNLLXDependenyPipe: public CoNLLXDependencyRepository {
private:
  //! The supported modes.
  enum PipeMode { kPipeLearn, kPipeTest };

  std::string reference_path;
  std::string input_path;
  std::string output_path;
  std::string model_path;
  std::string root;
  int shuffle_times;

  PipeMode mode;

  Decoder* decoder;
  Weight* weight;

public:
  GreedySearchCoNLLXDependenyPipe(const GreedyLearnOption& opt):
    mode(kPipeLearn),
    weight(nullptr), decoder(nullptr),
    reference_path(opt.reference_path), model_path(opt.model_path),
    shuffle_times(opt.shuffle_times),
    root(opt.root) {
    if (load_model(opt.model_path)) { _INFO << "report: model is loaded.";}
    else                            { _INFO << "report: model is not loaded.";}
    _INFO << "report: root = " << root;
  }

  GreedySearchCoNLLXDependenyPipe(const GreedyTestOption& opt):
    mode(kPipeTest),
    weight(nullptr), decoder(nullptr),
    input_path(opt.input_path), output_path(opt.output_path), model_path(opt.model_path),
    shuffle_times(0) {
    if (load_model(opt.model_path)) { _INFO << "report: model is loaded.";}
    else                            { _INFO << "report: model is not loaded.";}
    _INFO << "report: root = " << root;
  }

  ~GreedySearchCoNLLXDependenyPipe() {
    if (weight) { delete weight; weight = nullptr; }
    if (decoder) { delete decoder; decoder = nullptr; }
  }

  void test() {
    if (!setup(input_path, false)) { return; }

    decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
      Decoder::kLeft, 1, true, kEarlyUpdate, nullptr);

    std::ostream* os = IO::get_ostream(output_path.c_str());
    auto N = dataset.size();

    for (auto n = 0; n < N; ++ n) {
      const CoNLLXDependency& instance = dataset[n];
      //auto L = instance.forms.size();
      int max_nr_actions = MaxNumberOfActionsFunction()(instance) ;
      //std::vector<State> states(L* 2);
      std::vector<State> states(max_nr_actions + 1) ; // one more state
      states[0].copy(State(&instance));
      int ended_step = 0 ;
      for (auto step = 0 ; step < max_nr_actions ; ++ step) {
        std::vector<Action> possible_actions;
        PackedScores<Action> scores;
        decoder->get_possible_actions(states[step], possible_actions);
        weight->batchly_score(states[step], possible_actions, true, scores);

        Action best_action; floatval_t best_score = -1e20;
        for (auto& act: possible_actions) {
          if (scores[act] > best_score) {
            best_action = act;
            best_score = scores[act];
          }
        }

        decoder->transit(states[step], best_action, 0, &states[step+ 1]);
        ended_step = step + 1 ; // sync ended step
        if(states[ended_step].is_complete()){ break ;}
      }

      CoNLLXDependency output;
      //build_output(states[L*2-1], output, deprels_alphabet.encode(root));
      build_output(states[ended_step], output, deprels_alphabet.encode(root));
      IO::write_conllx_dependency_instance((*os), output, forms_alphabet,
          lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
          deprels_alphabet);
    }
    if (os != &(std::cout) && os != NULL) { delete os; }
  }

  void learn() {
    if (!setup(reference_path, true)) { return; }

    decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
        Decoder::kLeft, 1, false, kEarlyUpdate, nullptr);

    auto N = dataset.size();
    auto n_samples = 0;
    std::vector<size_t> ranks;
    Utility::shuffle(N, shuffle_times, ranks);

    for (int n = 0; n < N; ++ n) {
      const CoNLLXDependency& instance = dataset[ranks[n]];
      std::vector<Action> oracle_actions;
      std::vector<Action> possible_actions;
      ActionUtils::get_oracle_actions(instance, oracle_actions);

      // size_t L = data.forms.size();
      std::vector<State> states(oracle_actions.size() + 1);
      states[0].copy(State(&instance));
      // The third parameter is not needed, put zero to it.
      for (auto step = 0; step < oracle_actions.size(); ++ step) {
        n_samples += 1;

        auto& oracle_action = oracle_actions[step];
        PackedScores<Action> scores;

        decoder->get_possible_actions(states[step], possible_actions);
        weight->batchly_score(states[step], possible_actions, false, scores);

        Action best_action; floatval_t best_score = -1e20;
        for (auto& act: possible_actions) {
          if (scores[act] > best_score) {
            best_action = act;
            best_score = scores[act];
          }
        }

        if (best_action != oracle_action) {
          weight->update(states[step], best_action, n_samples, -1);
          weight->update(states[step], oracle_action, n_samples, 1);
        }

        decoder->transit(states[step], oracle_action, 0, &states[step+ 1]);
      }
      if ((n+ 1) % 1000 == 0) { _INFO << "report: processed " << n+1 << " instances."; }
    }
    weight->flush(n_samples);

    save_model(model_path);
  }

  void save_model(const std::string& model_path) {
    std::ofstream mfs(model_path);
    if (!mfs.good()) {
      _WARN << "pipe: failed to save model.";
    } else {
      CoNLLXDependencyRepository::save_model(mfs);
      weight->save(mfs);
      boost::archive::text_oarchive oa(mfs);
      oa << root;
      _INFO << "pipe: model is saved to " << model_path;
    }
  }

  bool load_model(const std::string& model_path) {
    weight = new Weight;
    std::ifstream mfs(model_path);

    if (!CoNLLXDependencyRepository::load_model(mfs)) {
      return false;
    }
    if (!weight->load(mfs)) {
      _WARN << "pipe: failed to load weight.";
      return false;
    }

    boost::archive::text_iarchive ia(mfs);
    ia >> root;
    return true;
  }
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_PIPE_H__
