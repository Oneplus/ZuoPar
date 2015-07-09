#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_LEARNER_H__

#include "system/online_learner.h"
#include "experimental/hc_search/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

typedef OnlineLearner<ScoreContext, Weight> Learner;

} //  namespace cstep
} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_LEARNER_H__
