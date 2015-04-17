#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__

#include <iostream>
#include <fstream>
#include <boost/tuple/tuple.hpp>
#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "frontend/common_opt.h"
#include "frontend/common_pipe_cfg.h"
#include "experimental/hc_search/arceager/opt.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/weight.h"
#include "experimental/hc_search/arceager/decoder.h"
#include "experimental/hc_search/arceager/learner.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;
namespace eg = ZuoPar::Engine;

class Pipe: public fe::CommonPipeConfigure {
private:
  struct DependencyWrapper {
    DependencyWrapper(const std::vector<form_t>& f,
        const std::vector<postag_t>& p,
        const std::vector<int>& h,
        const std::vector<deprel_t>& d)
      : forms(f), postags(p), heads(h), deprels(d) {}

    const std::vector<form_t>& forms;
    const std::vector<postag_t>& postags;
    const std::vector<int>& heads;
    const std::vector<deprel_t>& deprels;
  };

  struct RerankingInstanceParse {
    int rank;
    floatval_t score;
    std::vector<int> heads;
    std::vector<deprel_t> deprels;

    void resize(size_t N) {
      heads.resize(N);
      deprels.resize(N);
    }
  };

  struct RerankingInstance {
    std::vector<form_t>   forms;
    std::vector<postag_t> postags;
    RerankingInstanceParse oracle;
    std::vector< RerankingInstanceParse > good;
    std::vector< RerankingInstanceParse > bad;

    void init(size_t N, size_t nr_good, size_t nr_bad) {
      forms.resize(N);  postags.resize(N);  oracle.resize(N);
      good.resize(nr_good); bad.resize(nr_bad);
      for (RerankingInstanceParse& item: good) { item.resize(N); }
      for (RerankingInstanceParse& item: bad) { item.resize(N); }
    }
  };


public:
  //! Build pipe for training first type data.
  Pipe(const LearnOneOption& opts);
  //! Build pipe for preparing data for phase2.
  Pipe(const PrepareTwoOption& opts);
  //! Build pipe for learning the second phase.
  Pipe(const LearnTwoOption& opts);
  //! Build pipe for testing.
  Pipe(const TestOption& opts);
  //! Build pipe for evaluation, on the UAS/LAS upperbound, averaged UAS/LAS
  Pipe(const EvaluateOption& opts);
  //! The deallocator
  ~Pipe();
  void run();
  //! special for learn2
  void learn2();
private:
  //! The setup precedure for phase(1)
  bool setup();
  //! The setup precedure for phase(2)
  bool setup2();
  bool phase_one_load_model(const std::string& phase_one_model_path);

  bool phase_two_load_model(const std::string& phase_two_model_path);

  void phase_one_save_model(const std::string& phase_one_model_path);

  void phase_two_save_model(const std::string& phase_two_model_path);

  void build_output(const State& source, Dependency& output);

  void build_knowledge();

  //!
  bool is_punctuation(const form_t& postag);

  //! Calculate the loss of the predicted dependency tree
  int wrong(const Dependency& predict, const Dependency& oracle,
      bool labeled, bool ignore_punctuation, int& nr_effective_tokens);

  //
  void wrapper_to_instance(const DependencyWrapper& wrapper, Dependency& instance);

  //
  void learn2_learn_from_pair(const Dependency& oracle,
      const Dependency& good,
      const Dependency& bad,
      const State& good_state,
      const State& bad_state,
      const floatval_t& good_score,
      const floatval_t& bad_score,
      int timestamp);

  enum PipeModeExt {
    kPipeLearnPhaseOne,
    kPipePreparePhaseTwo,
    kPipeLearnPhaseTwo,
    kPipeEvaluate,
    kPipeTest};

  enum PipeLearnOneMethod {
    kPipeLearnOneBaseline,
    kPipeLearnOneBest,
    kPipeLearnOneRandom,
    kPipeLearnOneWorst
  };

  PipeModeExt mode_ext;                 //! The global mode flag.
  PipeLearnOneMethod learn_one_method;  //! The learning method for 1st phase.

  bool ignore_punctuation;          //! Use to specify to ignore punctuation
  bool rerank;                      //! Use to specify rerank.
  std::string root;                 //! The root relation string.
  std::string language;             //! The language.
  std::string phase_one_model_path; //! The path to the phase one model.
  std::string phase_two_model_path; //! The path to the phase two model.

  //! The weight for the heuristic step.
  HeuristicWeight*  heuristic_weight;

  //! The learner for the heuristic step.
  HeuristicLearner* heuristic_learner;

  //! The weight for the reranking step.
  CostWeight* cost_weight;

  //! The learner for the reranking step.
  CostLearner* cost_learner;

  //! The decoder, for step (1);
  Decoder* decoder;

  eg::TokenAlphabet forms_alphabet;
  eg::TokenAlphabet postags_alphabet;
  eg::TokenAlphabet deprels_alphabet;

  std::vector<Dependency> dataset;
  std::vector<RerankingInstance> dataset2;
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
