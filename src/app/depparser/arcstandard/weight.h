#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_MODEL_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_MODEL_H__

#include "types/common.h"
#include "model/associated/feature_param_map_collection.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/score_context.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class Weight: public FeatureParameterCollection<State,
  ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_MODEL_H__
