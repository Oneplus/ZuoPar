#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_WEIGHT_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_WEIGHT_H__

#include "types/common.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/state.h"
#include "app/semchunker/mono/score_context.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

class Weight
: public FeatureParameterCollection<State, ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_WEIGHT_H__
