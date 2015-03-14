#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_MODEL_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_MODEL_H__

#include "types/common.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "app/depparser/swap/action.h"
#include "app/depparser/swap/state.h"
#include "app/depparser/swap/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

class Weight: public FeatureParameterCollection<State,
  ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_MODEL_H__
