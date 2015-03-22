#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__

#include "system/structure_online_learner.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/state.h"
#include "app/semchunker/mono/weight.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

struct Loss {
  floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0.;
    for(const State *p = predict, *q = correct;
        p->previous && q->previous;
        p = p->previous, q = q->previous) {
      if (p->last_action != q->last_action) { retval += 1.; }
    }
    return retval;
  }
};

typedef TransitionStructureOnlineLearner<
  Action, State, Weight, Loss
> Learner;

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_LEARNER_H__
