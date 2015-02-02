#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__

#include "types/common.h"
#include "model/fast_meta/meta_feature_param_map_collection.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "experimental/hc_search/arceager/action.h"
#include "experimental/hc_search/arceager/state.h"
#include "experimental/hc_search/arceager/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

class HeuristicWeight
: public FeatureParameterCollection<State, HeuristicScoreContext, Action> {
public:
  //! The weight constructor.
  HeuristicWeight();
};

class CostWeight
: public MetaFeatureParameterCollection<State, CostScoreContext> {
public:
  //! The weight constructor.
  CostWeight();
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_MODEL_H__
