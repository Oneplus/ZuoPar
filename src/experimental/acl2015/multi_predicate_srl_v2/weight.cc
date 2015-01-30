#include <boost/assert.hpp>
#include "experimental/acl2015/multi_predicate_srl_v2/weight.h"
#include "experimental/acl2015/multi_predicate_srl_v2/action.h"
#include "experimental/acl2015/multi_predicate_srl_v2/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

#define U(_1) do { \
  UnigramFeatureCollection::repo.push_back( \
      UnigramFeatureMap([](const ScoreContext& ctx, const Action& act, \
          std::vector<UnigramFeature>& cache) -> void { \
        cache.push_back( UnigramFeature(ctx._1, act) ); \
        }) \
      ); \
} while (0);

#define B(_1, _2) do { \
  BigramFeatureCollection::repo.push_back( \
      BigramFeatureMap([](const ScoreContext& ctx, const Action& act, \
          std::vector<BigramFeature>& cache) -> void { \
        cache.push_back( BigramFeature(BigramMetaFeature(ctx._1, ctx._2), act) ); \
        }) \
      ); \
} while (0);

#define T(_1, _2, _3) do { \
  TrigramFeatureCollection::repo.push_back( \
      TrigramFeatureMap([](const ScoreContext& ctx, const Action& act, \
          std::vector<TrigramFeature>& cache) -> void { \
        cache.push_back( TrigramFeature(TrigramMetaFeature(ctx._1, ctx._2, ctx._3), act) ); \
        }) \
      ); \
} while (0);

Weight::Weight() {
  U(w_2); U(w_1); U(w0); U(w1); U(w2);
  B(w_2, w_1); B(w_1, w0); B(w0, w1); B(w1, w2);
  U(w_pred); U(vc_pred); U(position); U(dist);
  B(w0, w_pred); B(vc_pred, position); B(w_pred, position); B(w0, position);
  T(w0, w_pred, position); T(w0, vc_pred, position);

  U(p_2); U(p_1); U(p0); U(p1); U(p2);
  B(p_2, p_1); B(p_1, p0); B(p0, p1); B(p1, p2);
  U(p_pred); U(p_pred1); U(p_pred_1);
  StringFeatureCollection::repo.push_back(
      StringFeatureMap([](const ScoreContext& ctx, const Action& act,
          std::vector<StringFeature>& cache) -> void {
        cache.push_back( StringFeature(*ctx.path, act) );
        })
      );
  B(p_pred, position); B(p0, position);
  T(p0, p_pred, position); T(p0, vc_pred, position);
  U(t_1);
  T(w0, w_pred, label_set); T(p0, p_pred, label_set);
}

#undef U
#undef B
#undef T

void
Weight::vectorize(const ScoreContext& ctx,
    const ActionCollection& collection,
    floatval_t scale, SparseVector* sparse_vector) {
  BOOST_ASSERT_MSG(false, "Exact vectorization was not implemented.");
}

void
Weight::vectorize2(const State& state,
    const ActionCollection& collection,
    floatval_t scale, SparseVector2* sparse_vector) {
  ScoreContext ctx(state);
  vectorize2(ctx, collection, scale, sparse_vector);
}

void
Weight::vectorize2(ScoreContext& ctx,
    const ActionCollection& collection,
    floatval_t scale, SparseVector2* sparse_vector) {
  for (size_t i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    UnigramFeatureCollection::vectorize2(ctx, act, scale, sparse_vector);
    BigramFeatureCollection::vectorize2(ctx, act, scale, sparse_vector);
    TrigramFeatureCollection::vectorize2(ctx, act, scale, sparse_vector);
    StringFeatureCollection::vectorize2(ctx, act, scale, sparse_vector);
  }
}

floatval_t
Weight::score(const ScoreContext& ctx, const Action& act, bool avg) {
  floatval_t ret = 0;
  ret += UnigramFeatureCollection::score(ctx, act, avg);
  ret += BigramFeatureCollection::score(ctx, act, avg);
  ret += TrigramFeatureCollection::score(ctx, act, avg);
  ret += StringFeatureCollection::score(ctx, act, avg);
  return ret;
}

floatval_t
Weight::score(const State& state, const ActionCollection& collection,
    bool avg) {
  ScoreContext ctx(state);
  return score(ctx, collection, avg);
}

floatval_t
Weight::score(ScoreContext& ctx, const ActionCollection& collection,
    bool avg) {
  //! Should only be invoked in the learner.
  floatval_t ret = 0;
  for (size_t i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    ret += score(ctx, act, avg);
  }
  return ret;
}

void
Weight::batchly_score(const ScoreContext& ctx,
    const std::vector<ActionCollection>& actions,
    bool avg, PackedScores<ActionCollection>& result) {
  BOOST_ASSERT_MSG(false, "Not implemented! should not be invoked!");
}

void
Weight::update(const State& state, const ActionCollection& collection,
    int timestamp, floatval_t scale) {
  ScoreContext ctx(state);
  update(ctx, collection, timestamp, scale);
}

void
Weight::update(ScoreContext& ctx, const ActionCollection& collection,
    int timestamp, floatval_t scale) {
  for (int i = 0; i < collection.size(); ++ i) {
    const Action& act = collection[i];
    ctx.reset_predicate_rank(i);
    _TRACE << "weight: updating " << i << ", " << act << " by " << scale;
    UnigramFeatureCollection::update(ctx, act, timestamp, scale);
    BigramFeatureCollection::update(ctx, act, timestamp, scale);
    TrigramFeatureCollection::update(ctx, act, timestamp, scale);
    StringFeatureCollection::update(ctx, act, timestamp, scale);
  }
}

void
Weight::flush(int timestamp) {
  UnigramFeatureCollection::flush(timestamp);
  BigramFeatureCollection::flush(timestamp);
  TrigramFeatureCollection::flush(timestamp);
  StringFeatureCollection::flush(timestamp);
}

bool
Weight::save(std::ostream& os) {
  UnigramFeatureCollection::save(os);
  BigramFeatureCollection::save(os);
  TrigramFeatureCollection::save(os);
  StringFeatureCollection::save(os);
  return true;
}

bool
Weight::load(std::istream& is) {
  UnigramFeatureCollection::load(is);
  BigramFeatureCollection::load(is);
  TrigramFeatureCollection::load(is);
  StringFeatureCollection::load(is);
  return true;
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
