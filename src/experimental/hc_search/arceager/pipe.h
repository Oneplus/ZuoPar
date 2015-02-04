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
  Pipe(const LearnTwoOption& opts);
  Pipe(const TestOption& opts);
  Pipe(const EvaluateOption& opts);
  bool setup();
  void run();
  bool phase_one_load_model(const std::string& phase_one_model_path);
  bool phase_two_load_model(const std::string& phase_two_model_path);
  void phase_one_save_model(const std::string& phase_one_model_path);
  void phase_two_save_model(const std::string& phase_two_model_path);
  void build_output(const State& source, Dependency& output);
  int loss(const Dependency& predict, const Dependency& oracle, bool labeled);
private:
  enum PipeModeExt {
    kPipeLearnPhaseOne,
    kPipeLearnPhaseTwo,
    kPipeEvaluate,
    kPipeTest};
  PipeModeExt mode_ext;

  HeuristicWeight*  heuristic_weight;
  HeuristicLearner* heuristic_learner;

  CostWeight*   cost_weight;
  CostLearner*  cost_learner;

  Decoder* decoder;

  std::string phase_one_model_path;
  std::string phase_two_model_path;
  eg::TokenAlphabet forms_alphabet;
  eg::TokenAlphabet postags_alphabet;
  eg::TokenAlphabet deprels_alphabet;
  std::vector<Dependency> dataset;
  bool rerank;
  std::string root;
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
