#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_PIPE_H__

#include <iostream>
#include <fstream>
#include <boost/tuple/tuple.hpp>
#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "frontend/common_opt.h"
#include "frontend/common_pipe_cfg.h"
#include "app/depparser/utils.h"
#include "experimental/hc_search/cstep_opt.h"
#include "experimental/hc_search/weight.h"
#include "experimental/hc_search/learner.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

namespace fe = ZuoPar::FrontEnd;

class Pipe: public fe::CommonPipeConfigure {
private:
  struct RerankingTree { // for one tree
    int loss;
    floatval_t h_score;
    floatval_t c_score;
    std::vector<int>      heads;
    std::vector<deprel_t> deprels;

    void resize(size_t N) { heads.resize(N); deprels.resize(N); }
  };

  struct RerankingInstance {
    CoNLLXDependency instance; //! The instance to store forms, postags, deprels
    RerankingTree oracle; //! The oracle tree
    std::vector<RerankingTree> trees;
  };

  struct Sample {
    // Two group of (+/-) instances
    const CoNLLXDependency* ref;
    std::vector<RerankingTree *> good;
    std::vector<RerankingTree *> bad;
  };

private:
  enum RankingStrategy { kGold, kCoarse, kFine };
  RankingStrategy rank_strategy;
  DependencyParser::DependencyParserUtils::EvaluationStrategy evaluate_strategy;

private:
  Weight* weight;   //! The weight for the reranking step.
  Learner* learner; //! The learner.
  std::vector<RerankingInstance> dataset; //! The dependency
  std::vector<Sample> samples; //! The samples
  Engine::TokenAlphabet forms_alphabet; //! The form alphabet.
  Engine::TokenAlphabet postags_alphabet; //! The postag alphabet.
  Engine::TokenAlphabet deprels_alphabet; //! The deprel alphabet.
  std::string language;
  floatval_t alpha; //  only used in testing model.
public:
  //! Build pipe for learning the second phase.
  Pipe(const LearnOption& opts);

  //! Build pipe for testing.
  Pipe(const TestOption& opts);

  //! The deallocator
  ~Pipe();

  //!
  void test();

  //! special for learn2
  void learn();
private:
  //! The setup precedure for phase(2)
  bool load_model(const std::string& cstep_model_path);

  void save_model(const std::string& cstep_model_path);

  bool load_data(const std::string& filename, bool with_oracle);

  // Check if the input is legal.
  bool check_instance(const std::vector<std::string>& info,
      const std::vector< std::vector<std::string> >& mat) const;

  void column_to_reranking_tree(int i,
      const std::vector< std::string >& header,
      const std::vector< std::vector<std::string> >& mat,
      RerankingTree& tree);

  void generate_training_samples();

  int wrong(const CoNLLXDependency& instance,
      const std::vector<int>& predict_heads,
      const std::vector<int>& predict_deprels,
      const std::vector<int>& heads,
      const std::vector<int>& deprels,
      bool labeled) const;

  void build_knowledge();

  void learn_one_pair(const CoNLLXDependency* instance,
      const RerankingTree* good,
      const RerankingTree* bad,
      int timestamp);

  /*
  //!
  bool is_punctuation(const form_t& postag);

  //! Calculate the loss of the predicted dependency tree
  int wrong(const Dependency& predict, const Dependency& oracle,
      bool labeled, bool ignore_punctuation, int& nr_effective_tokens);

  //
  bool ignore_punctuation;          //! Use to specify to ignore punctuation
  bool rerank;                      //! Use to specify rerank.
  std::string root;                 //! The root relation string.
  std::string language;             //! The language.

*/
};

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_PIPE_H__
