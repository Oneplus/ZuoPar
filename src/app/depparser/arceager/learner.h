#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__

#include "system/structure_online_learner.h"
#include "system/structure_online_learner_minibatch.h"
#include "app/depparser/arceager/action.h"
#include "app/depparser/arceager/state.h"
#include "app/depparser/arceager/weight.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

struct Loss {
  floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0.;
    for (auto i = 0; i < predict->buffer; ++ i) {
      if (predict->heads[i] != correct->heads[i])           { retval += 2.; }
      else if (predict->deprels[i] != correct->deprels[i])  { retval += 1.; }
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

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_LEARNER_H__
