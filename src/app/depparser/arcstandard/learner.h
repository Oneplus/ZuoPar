#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_LEARNER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_LEARNER_H__

#include "system/structure_learner.h"
#include "system/structure_learner_minibatch.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

typedef TransitionStructureLearner<Action, State, Weight> Learner;
typedef TransitionStructureLearnerMiniBatch<Action, State, Weight> MinibatchLearner;

} //  end for arcstandard
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_LEARNER_H__
