#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_LEARNER_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_LEARNER_H__

#include "system/structure_online_learner.h"
#include "system/structure_online_learner_minibatch.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/state.h"
#include "app/depparser/choi2013/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

typedef TransitionStructureOnlineLearner<Action, State, Weight> Learner;
typedef TransitionStructureOnlineLearnerMiniBatch<Action, State, Weight> MinibatchLearner;

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_CHOI2013_LEARNER_H__
