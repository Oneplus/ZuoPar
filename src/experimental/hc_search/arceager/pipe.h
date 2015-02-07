#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__

#include <iostream>
#include <fstream>
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
  bool setup();
  void run();
  void learn2();
  bool phase_one_load_model(const std::string& phase_one_model_path);
  bool phase_two_load_model(const std::string& phase_two_model_path);
  void phase_one_save_model(const std::string& phase_one_model_path);
  void phase_two_save_model(const std::string& phase_two_model_path);
  void build_output(const State& source, Dependency& output);
  void build_knowledge();
  void write_prepared_data(const State& good, const State& bad, std::ostream& os);
  bool read_dataset2();
private:
  //!
  bool is_punctuation(const form_t& postag);

  //! Calculate the loss of the predicted dependency tree
  int loss(const Dependency& predict, const Dependency& oracle,
      bool labeled, bool ignore_punctuation, int& nr_effective_tokens);
private:
  enum PipeModeExt {
    kPipeLearnPhaseOne,
    kPipePreparePhaseTwo,
    kPipeLearnPhaseTwo,
    kPipeEvaluate,
    kPipeTest};

  PipeModeExt mode_ext;
  bool rerank;                      //! Use to specify rerank.
  bool oracle;                      //! Used in preparation phase.
  std::string root;                 //! The root relation string.
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
  std::vector<std::pair<Dependency, Dependency> > dataset2;
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
