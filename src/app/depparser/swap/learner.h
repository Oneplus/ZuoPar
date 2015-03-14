#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_LEARNER_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_LEARNER_H__

#include "system/structure_online_learner.h"
#include "system/structure_online_learner_minibatch.h"
#include "app/depparser/swap/action.h"
#include "app/depparser/swap/state.h"
#include "app/depparser/swap/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

typedef TransitionStructureOnlineLearner<Action, State, Weight> Learner;
typedef TransitionStructureOnlineLearnerMiniBatch<Action, State, Weight> MinibatchLearner;

} //  end for swap
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_LEARNER_H__
