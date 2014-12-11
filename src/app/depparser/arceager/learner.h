#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__

#include "system/structure_learner.h"
#include "app/depparser/arceager/action.h"
#include "app/depparser/arceager/state.h"
#include "app/depparser/arceager/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

typedef TransitionStructureLearner<Action, State, Weight> Learner;

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__
