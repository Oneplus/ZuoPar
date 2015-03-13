#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_H__

#include "system/structure_online_learner.h"
#include "system/structure_online_learner_minibatch.h"
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/state.h"
#include "app/seqlabeler/weight.h"

namespace ZuoPar {
namespace SequenceLabeler {

typedef TransitionStructureOnlineLearner<
  Action, State, Weight
> Learner;

typedef TransitionStructureOnlineLearnerMiniBatch<
  Action, State, Weight
> MinibatchLearner;

} //  end for sequencelabeler
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_H__
