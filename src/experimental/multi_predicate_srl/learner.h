#ifndef __ZUOPAR_EXPERIMENTAL_MONO_SRL_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_MONO_SRL_LEARNER_H__

#include "system/structure_online_learner.h"
#include "experimental/multi_predicate_srl/action.h"
#include "experimental/multi_predicate_srl/state.h"
#include "experimental/multi_predicate_srl/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

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
  ActionCollection, State, Weight, Loss
> Learner;

} //  namespace multipredicatesrl
} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MONO_SRL_LEARNER_H__
