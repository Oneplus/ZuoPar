#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_LEARNER_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_LEARNER_H__

#include "system/structure_online_learner.h"
#include "app/lexanalyzer/wordseg/action.h"
#include "app/lexanalyzer/wordseg/state.h"
#include "app/lexanalyzer/wordseg/weight.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

struct Loss {
  floatval_t operator()(const State* predict, const State* correct) {  return 1.; }
};

typedef TransitionStructureOnlineLearner<Action, State, Weight, Loss> Learner;

} //  end for chinesewordsegmentor
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_LEARNER_H__
