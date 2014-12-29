#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_WEIGHT_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_WEIGHT_H__

#include "types/common.h"
#include "model/associated/pointwise/feature_param_map_collection.h"
#include "experimental/acl2015/mono_srl/action.h"
#include "experimental/acl2015/mono_srl/state.h"
#include "experimental/acl2015/mono_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

class Weight
: public FeaturePointwiseParameterCollection<
  State,
  ScoreContext,
  Action
> {
public:
  //! The weight constructor.
  Weight();
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_WEIGHT_H__
