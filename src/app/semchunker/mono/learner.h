#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__

#include "system/structure_online_learner.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/state.h"
#include "app/semchunker/mono/weight.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

typedef TransitionStructureOnlineLearner<Action, State, Weight> Learner;

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__
