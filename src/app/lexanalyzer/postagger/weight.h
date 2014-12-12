#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__

#include "types/common.h"
#include "model/string/string_feature_param_map_collection.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/state.h"
#include "app/lexanalyzer/postagger/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

class Weight: public StringFeaturePointwiseParameterCollection<State,
  ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__
