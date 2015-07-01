#include <boost/assert.hpp>
#include "experimental/multi_predicate_srl/weight.h"
#include "experimental/multi_predicate_srl/action.h"
#include "experimental/multi_predicate_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

Weight::Weight() {
  ZUOPAR_FEATURE_MAP_REGIST_U1(w_2);
  ZUOPAR_FEATURE_MAP_REGIST_U1(w_1);
  ZUOPAR_FEATURE_MAP_REGIST_U1(w0);
  ZUOPAR_FEATURE_MAP_REGIST_U1(w1);
  ZUOPAR_FEATURE_MAP_REGIST_U1(w2);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w_2, w_1);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w_1, w0);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w0, w1); 
  ZUOPAR_FEATURE_MAP_REGIST_B11(w1, w2);
  ZUOPAR_FEATURE_MAP_REGIST_U1(w_pred);
  ZUOPAR_FEATURE_MAP_REGIST_U1(vc_pred);
  ZUOPAR_FEATURE_MAP_REGIST_U1(position);
  ZUOPAR_FEATURE_MAP_REGIST_U1(dist);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w0, w_pred);
  ZUOPAR_FEATURE_MAP_REGIST_B11(vc_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_B11(w0, position);
  ZUOPAR_FEATURE_MAP_REGIST_T111(w0, w_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_T111(w0, vc_pred, position);

  ZUOPAR_FEATURE_MAP_REGIST_U1(p_2);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p_1);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p0);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p1);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p2);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p_2, p_1);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p_1, p0);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p0, p1);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p1, p2);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p_pred);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p_pred1);
  ZUOPAR_FEATURE_MAP_REGIST_U1(p_pred_1);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_B11(p0, position);
  ZUOPAR_FEATURE_MAP_REGIST_T111(p0, p_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_T111(p0, vc_pred, position);
  ZUOPAR_FEATURE_MAP_REGIST_U1(t_1);
  ZUOPAR_FEATURE_MAP_REGIST_T111(w0, w_pred, label_set);
  ZUOPAR_FEATURE_MAP_REGIST_T111(p0, p_pred, label_set);

  sfeat_map_repo.push_back(sf_map_t(
        [](const ScoreContext& ctx, std::vector<sfp_t>& cache)->void {
        cache.push_back( sfp_t(*ctx.path) );
        }));
}

void Weight::vectorize(const ScoreContext& ctx,
    const ActionCollection& collection,
    const floatval_t& scale, SparseVector* sparse_vector) {
  BOOST_ASSERT_MSG(false, "Exact vectorization was not implemented.");
}

void Weight::vectorize2(const State& state,
    const ActionCollection& collection,
    const floatval_t& scale, SparseVector2* sparse_vector) {
  ScoreContext ctx(state);
  vectorize2(ctx, collection, scale, sparse_vector);
}

void Weight::vectorize2(ScoreContext& ctx,
    const ActionCollection& collection,
    const floatval_t& scale, SparseVector2* sparse_vector) {
  for (size_t i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    col::vectorize2(ctx, act, scale, sparse_vector);
  }
}

void Weight::vectorize3(const State& state,
    const ActionCollection& collection,
    const floatval_t& scale, SparseVector3* sparse_vector) {
  ScoreContext ctx(state);
  vectorize3(ctx, collection, scale, sparse_vector);
}

void Weight::vectorize3(ScoreContext& ctx,
    const ActionCollection& collection,
    const floatval_t& scale, SparseVector3* sparse_vector) {
  for (size_t i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    col::vectorize3(ctx, act, scale, sparse_vector);
  }
}

floatval_t Weight::scorex(const ScoreContext& ctx, const Action& act,
    bool avg) const {
  return col::score(ctx, act, avg);
}

floatval_t Weight::score(const State& state, const ActionCollection& collection,
    bool avg) const {
  ScoreContext ctx(state);
  return score(ctx, collection, avg);
}

floatval_t Weight::score(ScoreContext& ctx, const ActionCollection& collection,
    bool avg) const {
  //! Should only be invoked in the learner.
  floatval_t ret = 0;
  for (size_t i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    ret += col::score(ctx, act, avg);
  }
  return ret;
}

void Weight::batchly_score(const ScoreContext& ctx,
    const std::vector<ActionCollection>& actions,
    bool avg, PackedScores<ActionCollection>& result) {
  BOOST_ASSERT_MSG(false, "Not implemented! should not be invoked!");
}

void Weight::update(const State& state, const ActionCollection& collection,
    int timestamp, floatval_t scale) {
  ScoreContext ctx(state);
  update(ctx, collection, timestamp, scale);
}

void Weight::update(ScoreContext& ctx, const ActionCollection& collection,
    int timestamp, floatval_t scale) {
  for (int i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    _TRACE << "weight: updating " << i << ", " << act << " by " << scale;
    col::update(ctx, act, timestamp, scale);
  }
}

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar
