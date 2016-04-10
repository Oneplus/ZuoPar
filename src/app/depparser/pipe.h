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
#include "frontend/common_pipe_utils.h"

namespace ZuoPar {
namespace DependencyParser {

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
class DependencyPipe {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  DependencyPipe(const boost::program_options::variables_map& vm)
    : weight(new Weight), decoder(0), learner(0), conf(vm) {
    if (vm.count("model") && load_model(vm["model"].as<std::string>())) {
      _INFO << "[RPT] model is loaded.";
    } else {
      _INFO << "[RPT] model is not loaded.";
    }
  }

  ~DependencyPipe() {
    if (weight)  { delete weight;  weight = 0;  }
    if (decoder) { delete decoder; decoder = 0; }
    if (learner) { delete learner; learner = 0; }
  }

  bool setup(const std::string& path, std::vector<Dependency>& ds, bool insert) {
    ds.clear();
    if (insert) {
      std::ifstream ifs(path);
      if (!ifs.good()) {
        _ERROR << "#: failed to open reference file.";
        _ERROR << "#: training halt";
        return false;
      }
      _INFO << "[RPT] loading dataset from reference file.";
      IO::read_dependency_dataset(ifs, ds, forms_alphabet,
        postags_alphabet, deprels_alphabet);
      _INFO << "[RPT] dataset is loaded from reference file.";
    } else {
      std::ifstream ifs(path);
      if (!ifs.good()) {
        _ERROR << "#: failed to open input file.";
        _ERROR << "#: testing halt";
        return false;
      }
      IO::read_dependency_dataset(ifs, ds, forms_alphabet,
        postags_alphabet, deprels_alphabet, 0x03);
    }
    _INFO << "[RPT] " << ds.size() << " instance(s) is loaded.";
    _INFO << "[RPT] " << forms_alphabet.size() << " forms(s) is detected.";
    _INFO << "[RPT] " << postags_alphabet.size() << " postag(s) is detected.";
    _INFO << "[RPT] " << deprels_alphabet.size() << " deprel(s) is detected.";
    return true;
  }

  void set_signature(const std::string& sign) { signature = sign; }

  double evaluate(const std::vector<Dependency>& dataset) {
    std::string output = FrontEnd::get_output_name(signature, conf);
    std::ostream* os = IO::get_ostream(output);
    decoder->set_use_avg();
    for (const Dependency& instance : dataset) {
      int max_actions = MaxNumberOfActionsFunction()(instance);
      State init_state(&instance);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
        std::vector<Action>(), max_actions);

      Dependency output;
      build_output((*result.first), output);
      IO::write_dependency_instance((*os), output, forms_alphabet,
        postags_alphabet, deprels_alphabet);
    }

    _INFO << "[PIP] processed #" << dataset.size() << " instances.";
    if (os == (&(std::cout))) { return 0.; }
    delete os;
    return Utility::execute_script(conf["script"].as<std::string>(), output);
  }

  void test() {
    if (!setup(conf["input"].as<std::string>(), dataset, false)) { return; }

    deprel_t root_tag = deprels_alphabet.insert(this->root);
    decoder = new Decoder(deprels_alphabet.size(), root_tag,
      conf["beam"].as<unsigned>(), true, UpdateStrategy::kNaive, weight);

    double score = evaluate(dataset);
    _INFO << "pipe test score: " << score;
  }

  //! Perform learning or testing according to the configuration.
  void learn() {
    if (!setup(conf["train"].as<std::string>(), dataset, true)) { return; }
    if (conf.count("devel")) { setup(conf["devel"].as<std::string>(), devel_dataset, false); }

    deprel_t root_tag = deprels_alphabet.insert(this->root);
    decoder = new Decoder(deprels_alphabet.size(), root_tag,
      conf["beam"].as<unsigned>(), false,
      get_update_strategy(conf["update"].as<std::string>()), weight);
    learner = new Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

    unsigned n_seen = 0, N = dataset.size();
    double best_score = 0.;
    std::string model_path = FrontEnd::get_model_name(signature, conf);
    for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
      _INFO << "[PIP] iteration #" << iter + 1 << ", start training.";
      std::random_shuffle(dataset.begin(), dataset.end());
      for (const Dependency& instance : dataset) {
        ++n_seen;
        // calculate the oracle transition actions.
        std::vector<Action> actions;
        ActionUtils::get_oracle_actions(instance, actions);

        int max_actions = MaxNumberOfActionsFunction()(instance);
        State init_state(&instance);
        typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_actions);

        learner->set_timestamp(n_seen);
        learner->learn(result.first, result.second);

        if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
          _INFO << "[PIP] processed #" << n_seen << " instances.";
        }
        if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
          learner->flush();
          double score = evaluate(devel_dataset);
          decoder->reset_use_avg();
          _INFO << "[PIP] evaluate score: " << score;
          if (score > best_score) {
            _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
            save_model(model_path);
            best_score = score;
          }
        }
      }
      learner->flush();
      _INFO << "[PIP] iter" << iter + 1 << " #errros: " << learner->errors();
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
  }

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path) {
    std::ifstream mfs(model_path);

    if (!mfs.good()) {
      _WARN << "[PIP] model doesn't exists.";             return false; }
    if (!forms_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load forms alphabet.";    return false; }
    if (!postags_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load postags alphabet.";  return false; }
    if (!deprels_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load deprels alphabet.";  return false; }
    if (!weight->load(mfs)) {
      _WARN << "[PIP] failed to load weight.";            return false; }
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
      _WARN << "[PIP] failed to save model.";
    } else {
      forms_alphabet.save(mfs);
      postags_alphabet.save(mfs);
      deprels_alphabet.save(mfs);
      weight->save(mfs);
      _INFO << "[PIP] model saved to " << model_path;
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
  std::vector<Dependency> devel_dataset;
  const boost::program_options::variables_map& conf;
  std::string signature;
  std::string root;
};

// CoNLLX pipe -------------

class CoNLLXDependencyRepository {
public:
  bool setup(const std::string& path, std::vector<CoNLLXDependency>& ds, bool insert) {
    ds.clear();
    std::ifstream ifs(path.c_str());
    if (insert) {
      if (!ifs.good()) {
        _ERROR << "#: failed to open reference file (training halt).";
        return false;
      }

      _INFO << "[RPT] loading dataset from reference file.";
      IO::read_conllx_dependency_dataset(ifs, ds, forms_alphabet,
          lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
          deprels_alphabet);
      _INFO << "[RPT] dataset is loaded from reference file.";
    } else {
      if (!ifs.good()) {
        _ERROR << "#: failed to open input file.";
        _ERROR << "#: testing halt";
        return false;
      }
      IO::read_conllx_dependency_dataset(ifs, ds, forms_alphabet,
          lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
          deprels_alphabet, 0x1f);
    }
    _INFO << "[RPT] " << ds.size() << " instance(s) is loaded.";
    _INFO << "[RPT] " << forms_alphabet.size()    << " form(s) is detected.";
    _INFO << "[RPT] " << lemmas_alphabet.size()   << " lemmas(s) is detected.";
    _INFO << "[RPT] " << cpostags_alphabet.size() << " cpostag(s) is detected.";
    _INFO << "[RPT] " << postags_alphabet.size()  << " postag(s) is detected.";
    _INFO << "[RPT] " << feat_alphabet.size()     << " feat(s) is detected.";
    _INFO << "[RPT] " << deprels_alphabet.size()  << " deprel(s) is detected.";
    return true;
  }

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(std::ifstream& mfs) {
    if (!mfs.good()) {
      _WARN << "[PIP] model doesn't exists.";             return false; }
    if (!forms_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load forms alphabet.";    return false; }
    if (!lemmas_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load lemmas alphabet.";   return false; }
    if (!cpostags_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load cpostag alphabet.";  return false; }
    if (!postags_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load postags alphabet.";  return false; }
    if (!feat_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load feat alphabet.";     return false; }
    if (!deprels_alphabet.load(mfs)) {
      _WARN << "[PIP] failed to load deprels alphabet.";  return false; }
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
  std::vector<CoNLLXDependency> devel_dataset;
  std::string signature;
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
class CoNLLXDependencyPipe: public CoNLLXDependencyRepository {
public:
  /**
   * The pipe constructor.
   *
   *  @param[in]  vm  The options.
   */
  CoNLLXDependencyPipe(const boost::program_options::variables_map& vm)
    : weight(new Weight), decoder(0), learner(0), conf(vm) {
    root = conf["root"].as<std::string>();
    if (vm.count("model") && load_model(vm["model"].as<std::string>())) {
      _INFO << "[RPT] model is loaded."; 
    } else {
      _INFO << "[RPT] model is not loaded."; 
    }
    _INFO << "[RPT] root = " << root;
  }

  ~CoNLLXDependencyPipe() {
    if (weight)  { delete weight;  weight = 0;  }
    if (decoder) { delete decoder; decoder = 0; }
    if (learner) { delete learner; learner = 0; }
  }

  void set_signature(const std::string& sign) { signature = sign; }

  //! Perform learning.
  void learn() {
    if (!setup(conf["train"].as<std::string>(), dataset, true)) {
      _ERROR << "[PIP] failed to load traininig data, training halted";
      return;
    }
    if (!conf.count("devel") || !setup(conf["devel"].as<std::string>(), devel_dataset, false)) {
      _WARN << "[PIP] development data is not loaded.";
    }

    deprel_t root_tag = deprels_alphabet.insert(root);
    decoder = new Decoder(deprels_alphabet.size(), root_tag, Decoder::kLeft,
      conf["beam"].as<unsigned>(), false,
      get_update_strategy(conf["update"].as<std::string>()),
      weight);

    learner = new Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

    unsigned n_seen = 0, N = dataset.size();
    double best_score = 0.;
    std::string model_path = FrontEnd::get_model_name(signature, conf);

    for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
      _INFO << "[PIP] iteration #" << iter + 1 << ", start training.";
      std::random_shuffle(dataset.begin(), dataset.end());
      for (const CoNLLXDependency& instance : dataset) {
        // calculate the oracle transition actions.
        ++n_seen;
        std::vector<Action> actions;
        ActionUtils::get_oracle_actions(instance, actions);
        
        int max_actions = MaxNumberOfActionsFunction()(instance);
        State init_state(&instance);
        typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_actions);
 
        learner->set_timestamp(n_seen);
        learner->learn(result.first, result.second);

        if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
          _INFO << "[PIP] processed #" << n_seen % N << " / " << n_seen / N << " instances.";
        }
        if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
          learner->flush();
          double score = evaluate(devel_dataset);
          decoder->reset_use_avg();
          _INFO << "[PIP] evaluate score: " << score;
          if (score > best_score) {
            _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
            save_model(model_path);
            best_score = score;
          }
        }
      }

      learner->flush();
      _INFO << "[PIP] iter" << iter + 1 << " #errros: " << learner->errors();
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
    _INFO << "[PIP] best development score: " << best_score;
  }

  double evaluate(const std::vector<CoNLLXDependency>& dataset) {
    std::string output = FrontEnd::get_output_name(signature, conf);
    std::ostream* os = IO::get_ostream(output);
    decoder->set_use_avg();
    for (const CoNLLXDependency& instance : dataset) {
      std::vector<Action> actions;
      int max_actions = MaxNumberOfActionsFunction()(instance);
      State init_state(&instance);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_actions);

      CoNLLXDependency output;
      build_output((*result.first), output, deprels_alphabet.encode(root));
      IO::write_conllx_dependency_instance((*os), output, forms_alphabet,
        lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
        deprels_alphabet);
    }
    _INFO << "[PIP] processed #" << dataset.size() << " instance(s).";
    if (os == (&(std::cout))) { return 0.; }
    delete os;
    return Utility::execute_script(conf["script"].as<std::string>(), output);
  }

  void test() {
    if (setup(conf["input"].as<std::string>(), dataset, false)) {
      return;
    }
    deprel_t root_tag = deprels_alphabet.insert(root);
    decoder = new Decoder(deprels_alphabet.size(), root_tag, Decoder::kLeft,
      conf["beam"].as<unsigned>(), true, UpdateStrategy::kNaive, weight);
    double score = evaluate(dataset);
    _INFO << "test: score" << score;
  }

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path) {
    std::ifstream mfs(model_path);

    if (!CoNLLXDependencyRepository::load_model(mfs)) {
      return false;
    }
    if (!weight->load(mfs)) {
      _WARN << "[PIP] failed to load weight.";
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
      _WARN << "[PIP] failed to save model.";
    } else {
      CoNLLXDependencyRepository::save_model(mfs);
      weight->save(mfs);
      _INFO << "[PIP] model saved to " << model_path;
    }
  }

protected:
  const boost::program_options::variables_map& conf;
  Weight* weight;     //! The parameter
  Learner* learner;   //! The parameter learner.
  Decoder* decoder;   //! The pointer to the decoder.
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
class GreedySearchCoNLLXDependencyPipe: public CoNLLXDependencyRepository {
private:
  Decoder* decoder;
  Weight* weight;
  const boost::program_options::variables_map& conf;
public:
  GreedySearchCoNLLXDependencyPipe(const boost::program_options::variables_map& vm):
    weight(new Weight), decoder(nullptr), conf(vm) {
    root = conf["root"].as<std::string>();
    if (vm.count("model") && load_model(vm["model"].as<std::string>())) {
      _INFO << "[RPT] model is loaded.";
    } else {
      _INFO << "[RPT] model is not loaded.";
    }
    _INFO << "[RPT] root = " << root;
  }

  ~GreedySearchCoNLLXDependencyPipe() {
    if (weight)   { delete weight;  weight = nullptr; }
    if (decoder)  { delete decoder; decoder = nullptr; }
  }

  void set_signature(const std::string& sign) { signature = sign; }

  double evaluate(const std::vector<CoNLLXDependency>& dataset) {
    // evaluation not rely on the decoder.
    std::string output = FrontEnd::get_model_name(signature, conf);
    std::ostream* os = IO::get_ostream(output);

    for (const CoNLLXDependency& instance : dataset) {
      //auto L = instance.forms.size();
      int max_nr_actions = MaxNumberOfActionsFunction()(instance);
      //std::vector<State> states(L* 2);
      std::vector<State> states(max_nr_actions + 1); // one more state
      states[0].copy(State(&instance));
      int ended_step = 0;
      for (auto step = 0; step < max_nr_actions; ++step) {
        std::vector<Action> possible_actions;
        PackedScores<Action> scores;
        decoder->get_possible_actions(states[step], possible_actions);
        weight->batchly_score(states[step], possible_actions, true, scores);

        Action best_action; floatval_t best_score = -1e20;
        for (auto& act : possible_actions) {
          if (scores[act] > best_score) {
            best_action = act;
            best_score = scores[act];
          }
        }

        decoder->transit(states[step], best_action, 0, &states[step + 1]);
        ended_step = step + 1; // sync ended step
        if (states[ended_step].is_complete()){ break; }
      }

      CoNLLXDependency output;
      build_output(states[ended_step], output, deprels_alphabet.encode(root));
      IO::write_conllx_dependency_instance((*os), output, forms_alphabet,
        lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
        deprels_alphabet);
    }
    if (os == (&(std::cout))) { return 0.; }
    if (os != nullptr) { delete os;  }
    return Utility::execute_script(conf["script"].as<std::string>(), output);
  }

  void test() {
    if (!setup(conf["input"].as<std::string>(), dataset, false)) { return; }

    decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
      Decoder::kLeft, 1, true, kEarlyUpdate, nullptr);
    double score = evaluate(dataset);
    _INFO << "test: score " << score;
  }

  void learn() {
    if (!setup(conf["train"].as<std::string>(), dataset, true)) { return; }
    if (conf.count("devel")) { setup(conf["devel"].as<std::string>(), devel_dataset, false); }

    std::string model_path = FrontEnd::get_model_name(signature, conf);
    decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
        Decoder::kLeft, 1, false, kEarlyUpdate, nullptr);

    unsigned n_samples = 0, n_seen = 0, N = dataset.size();
    double best_score = 0.;
    for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
      std::random_shuffle(dataset.begin(), dataset.end());
      _INFO << "start training at iteration#" << iter + 1;
      for (const CoNLLXDependency& instance : dataset) {
        ++n_seen;
        std::vector<Action> oracle_actions;
        std::vector<Action> possible_actions;
        ActionUtils::get_oracle_actions(instance, oracle_actions);

        // size_t L = data.forms.size();
        std::vector<State> states(oracle_actions.size() + 1);
        states[0].copy(State(&instance));
        // The third parameter is not needed, put zero to it.
        for (auto step = 0; step < oracle_actions.size(); ++ step) {
          ++n_samples;
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

          // Currently, only averaged perceptron is supported.
          if (best_action != oracle_action) {
            weight->update(states[step], best_action, n_samples, -1);
            weight->update(states[step], oracle_action, n_samples, 1);
          }

          decoder->transit(states[step], oracle_action, 0, &states[step+ 1]);
        }
        if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
          _INFO << "[PIP] processed #" << n_seen % N << "/" << n_seen / N << " instances.";
        }
        if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
          double score = evaluate(devel_dataset);
          weight->flush(n_samples);
          _INFO << "[PIP] evaluate at instance #" << n_seen << ", score: " << score;
          if (score > best_score) {
            _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
            save_model(model_path);
            best_score = score;
          }
        }
      }
      weight->flush(n_samples);
      double score = evaluate(devel_dataset);
      _INFO << "[PIP] evaluate at the end of iteration#" << iter + 1 << " score: " << score;
      if (score > best_score) {
        _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
        save_model(model_path);
        best_score = score;
      }
    }
  }

  void save_model(const std::string& model_path) {
    std::ofstream mfs(model_path);
    if (!mfs.good()) {
      _WARN << "[PIP] failed to save model.";
    } else {
      CoNLLXDependencyRepository::save_model(mfs);
      weight->save(mfs);
      boost::archive::text_oarchive oa(mfs);
      oa << root;
      _INFO << "[PIP] model is saved to " << model_path;
    }
  }

  bool load_model(const std::string& model_path) {
    std::ifstream mfs(model_path);

    if (!CoNLLXDependencyRepository::load_model(mfs)) {
      return false;
    }
    if (!weight->load(mfs)) {
      _WARN << "[PIP] failed to load weight.";
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
