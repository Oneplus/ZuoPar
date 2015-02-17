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
public:
  Pipe(const LearnOneOption& opts);
  Pipe(const PrepareTwoOption& opts);
  Pipe(const LearnTwoOption& opts);
  Pipe(const TestOption& opts);
  Pipe(const EvaluateOption& opts);
  ~Pipe();
  bool setup();
  bool setup2();
  void run();
  void run2();
  bool phase_one_load_model(const std::string& phase_one_model_path);
  bool phase_two_load_model(const std::string& phase_two_model_path);
  void phase_one_save_model(const std::string& phase_one_model_path);
  void phase_two_save_model(const std::string& phase_two_model_path);
  void build_output(const State& source, Dependency& output);
  void build_knowledge();
  // void write_prepared_data(const State& good, const State& bad, std::ostream& os);
private:
  void train_one_pair(const Dependency& oracle, const Dependency& good,
      const Dependency& bad, int good_rank, int bad_rank, 
      floatval_t good_score, floatval_t bad_score, int timestamp);
  //!
  bool is_punctuation(const form_t& postag);

  //! Calculate the loss of the predicted dependency tree
  int loss(const Dependency& predict, const Dependency& oracle,
      bool labeled, bool ignore_punctuation, int& nr_effective_tokens);
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

  enum PipeModeExt {
    kPipeLearnPhaseOne,
    kPipePreparePhaseTwo,
    kPipeLearnPhaseTwo,
    kPipeEvaluate,
    kPipeTest};

  enum PipeLearnOneMethod {
    kPipeLearnOneRegular,
    kPipeLearnOneBest,
    kPipeLearnOneWorst
  };

  enum PipeLearnTwoMethod {
    kPipeLearnTwoOracleAgainstRest,
    kPipeLearnTwoNaiveGoodAgainstBad,
    kPipeLearnTwoRelexedGoodAgainstBad
  };

  typedef boost::tuple<
    DependencyWrapper,
    DependencyWrapper,
    DependencyWrapper> train_tuple_t;

  void wrapper_to_instance(const DependencyWrapper& wrapper, Dependency& instance);
  void run2_learn_naive(const std::vector< train_tuple_t >& train_data);
  void run2_learn_oracle_against_rest();
  void run2_learn_naive_good_against_bad();
  void run2_learn_relaxed_good_against_bad();

  //! The global mode control flag.
  PipeModeExt mode_ext;
  PipeLearnOneMethod learn_one_method;
  PipeLearnTwoMethod learn_two_method;

  bool ignore_punctuation;
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
