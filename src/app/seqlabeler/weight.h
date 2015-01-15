#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_MODEL_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_MODEL_H__

#include "types/common.h"
#include "model/fast_associated/feature_param_map_collection.h"
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/state.h"
#include "app/seqlabeler/score_context.h"

namespace ZuoPar {
namespace SequenceLabeler {

class Weight: public FeatureParameterCollection<State,
  ScoreContext, Action> {
public:
  //! The weight constructor.
  Weight();
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_MODEL_H__
