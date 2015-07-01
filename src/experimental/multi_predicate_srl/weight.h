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

typedef FeatureParameterCollection<State, ScoreContext, Action> col;
class Weight: public col {
public:
  //! The weight constructor.
  Weight();

  void vectorize(const ScoreContext& ctx, const ActionCollection& act,
      const floatval_t& scale, SparseVector* sparse_vector);

  void vectorize2(const State& state, const ActionCollection& act,
      const floatval_t& scale, SparseVector2* sparse_vector);

  void vectorize2(ScoreContext& ctx, const ActionCollection& act,
      const floatval_t& scale, SparseVector2* sparse_vector);

  void vectorize3(const State& state, const ActionCollection& act,
      const floatval_t& scale, SparseVector3* sparse_vector);

  void vectorize3(ScoreContext& ctx, const ActionCollection& act,
      const floatval_t& scale, SparseVector3* sparse_vector);

  //!
  floatval_t scorex(const ScoreContext& ctx, const Action& act,
      bool avg) const;

  //!
  floatval_t score(ScoreContext& ctx, const ActionCollection& collection,
      bool avg) const;

  //!
  floatval_t score(const State& ctx, const ActionCollection& act,
      bool avg) const;

  //! Not implemented, should not be invoked.
  void batchly_score(const ScoreContext& ctx,
      const std::vector<ActionCollection>& actions,
      bool avg, PackedScores<ActionCollection>& result);

  //!
  void update(const State& state, const ActionCollection& collection,
      int timestamp, floatval_t scale);

  //!
  void update(ScoreContext& ctx, const ActionCollection& collection,
      int timestamp, floatval_t scale);
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_WEIGHT_H__
