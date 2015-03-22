#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__

#include "system/structure_online_learner.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/state.h"
#include "app/lexanalyzer/postagger/weight.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

struct Loss {
  floatval_t operator()(const State* predict, const State* correct) {
    floatval_t retval = 0;
    for (auto i = 0; i < predict->ref->size(); ++ i) {
      if (predict->postags[i] != correct->postags[i]) { retval += 1; }
    }
    return retval;
  }
};

typedef TransitionStructureOnlineLearner<Action, State, Weight, Loss> Learner;

} //  end for postagger
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__
