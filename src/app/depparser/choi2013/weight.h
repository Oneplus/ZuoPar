#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_MODEL_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_MODEL_H__

#include "types/common.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/state.h"
#include "app/depparser/choi2013/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

class Weight: public FeatureParameterCollection<State,
  ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_MODEL_H__
