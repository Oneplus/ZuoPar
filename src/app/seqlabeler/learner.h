#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_H__

#include "system/structure_learner.h"
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/state.h"
#include "app/seqlabeler/weight.h"

namespace ZuoPar {
namespace SequenceLabeler {

typedef TransitionStructureLearner<Action, State, Weight> Learner;

} //  end for sequencelabeler
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_H__
