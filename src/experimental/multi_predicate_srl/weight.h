#ifndef __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_WEIGHT_H__
#define __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_WEIGHT_H__

#include "types/common.h"
#include "model/meta_feature.h"
#include "model/feature.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "experimental/multi_predicate_srl/action.h"
#include "experimental/multi_predicate_srl/state.h"
#include "experimental/multi_predicate_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

class Weight: public FeatureParameterCollection<State,
   ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_WEIGHT_H__
