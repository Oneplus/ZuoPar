#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__

#include "system/structure_learner.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/state.h"
#include "app/lexanalyzer/postagger/weight.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

typedef TransitionStructureLearner<Action, State, Weight> Learner;

} //  end for postagger
} //  end for lexicalanalyzer
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_LEARNER_H__
