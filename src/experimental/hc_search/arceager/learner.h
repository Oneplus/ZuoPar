#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__

#include "system/online_learner.h"
#include "system/structure_learner.h"
#include "system/structure_learner_minibatch.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

typedef TransitionStructureLearner<
  Action,
  State,
  HeuristicWeight
> HeuristicLearner;

typedef OnlineLearner<
  State,
  CostWeight
> CostLearner;

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_LEARNER_H__
