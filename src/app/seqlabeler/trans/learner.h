#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_LEARNER_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_LEARNER_H__

#include "system/structure_online_learner.h"
#include "system/structure_online_learner_minibatch.h"
#include "app/seqlabeler/trans/action.h"
#include "app/seqlabeler/trans/state.h"
#include "app/seqlabeler/trans/weight.h"

namespace ZuoPar {
namespace SequenceLabeler {

struct Loss {
  floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0;
    for (auto i = 0; i < predict->ref->size(); ++ i) {
      if (predict->tags[i] != correct->tags[i]) { retval += 1; }
    }
    return retval;
  }
};

typedef TransitionStructureOnlineLearner<
  Action, State, Weight, Loss
> Learner;

typedef TransitionStructureOnlineLearnerMiniBatch<
  Action, State, Weight, Loss
> MinibatchLearner;

} //  end for sequencelabeler
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_LEARNER_H__
