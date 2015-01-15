#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_WEIGHT_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_WEIGHT_H__

#include "types/common.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"
#include "experimental/acl2015/mono_predicate_srl/state.h"
#include "experimental/acl2015/mono_predicate_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

class Weight
: public FeatureParameterCollection<
  State,
  ScoreContext,
  Action
> {
public:
  //! The weight constructor.
  Weight();
};

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_WEIGHT_H__
