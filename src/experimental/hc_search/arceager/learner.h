#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__

#include "system/online_learner.h"
#include "system/structure_online_learner.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

struct Loss {
  floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0.;
    for (auto i = 0; i < predict->buffer; ++ i) {
      if (predict->heads[i] != correct->heads[i])           { retval += 2.; }
      else if (predict->deprels[i] != correct->deprels[i])  { retval += 1.; }
    }
    return retval;
  }
};

typedef TransitionStructureOnlineLearner<
  Action, State, HeuristicWeight, Loss
> HeuristicLearner;

typedef OnlineLearner<
  State,
  CostWeight
> CostLearner;

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__
