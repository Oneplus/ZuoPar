#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_WEIGHT_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_WEIGHT_H__

#include "types/common.h"
#include "model/meta_feature.h"
#include "model/feature.h"
#include "model/associated/feature_param_map_v2.h"
#include "model/associated/feature_param_map_collection.h"
#include "experimental/acl2015/multi_predicate_srl_v2/action.h"
#include "experimental/acl2015/multi_predicate_srl_v2/state.h"
#include "experimental/acl2015/multi_predicate_srl_v2/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

typedef Feature< std::string,        Action > StringFeature;
typedef Feature< UnigramMetaFeature, Action > UnigramFeature;
typedef Feature< BigramMetaFeature,  Action > BigramFeature;
typedef Feature< TrigramMetaFeature, Action > TrigramFeature;

typedef FeatureParameterMap<
  ScoreContext,
  Action,
  StringFeature
> StringFeatureMap;

typedef FeatureParameterMap<
  ScoreContext,
  Action,
  UnigramFeature
> UnigramFeatureMap;

typedef FeatureParameterMap<
  ScoreContext,
  Action,
  BigramFeature
> BigramFeatureMap;

typedef FeatureParameterMap<
  ScoreContext,
  Action,
  TrigramFeature
> TrigramFeatureMap;

typedef FeatureParameterCollection<
  ScoreContext,
  Action,
  StringFeatureMap
> StringFeatureCollection;

typedef FeatureParameterCollection<
  ScoreContext,
  Action,
  UnigramFeatureMap
> UnigramFeatureCollection;

typedef FeatureParameterCollection<
  ScoreContext,
  Action,
  BigramFeatureMap
> BigramFeatureCollection;

typedef FeatureParameterCollection<
  ScoreContext,
  Action,
  TrigramFeatureMap
> TrigramFeatureCollection;

class Weight
: public UnigramFeatureCollection,
  public BigramFeatureCollection,
  public TrigramFeatureCollection,
  public StringFeatureCollection {
public:
  //! The weight constructor.
  Weight();

  //!
  void vectorize(const ScoreContext& ctx, const ActionCollection& act,
      floatval_t scale, SparseVector* sparse_vector);

  //!
  void vectorize2(const State& state, const ActionCollection& act,
      floatval_t scale, SparseVector2* sparse_vector);

  //!
  void vectorize2(ScoreContext& ctx, const ActionCollection& act,
      floatval_t scale, SparseVector2* sparse_vector);

  //!
  floatval_t score(const State& ctx, const ActionCollection& collection,
      bool avg);

  //!
  floatval_t score(ScoreContext& ctx, const ActionCollection& collection,
      bool avg);

  //!
  floatval_t score(const ScoreContext& ctx, const Action& act, bool avg);

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

  //!
  void flush(int timestamp);

  //!
  bool save(std::ostream& os);

  //!
  bool load(std::istream& is);
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_WEIGHT_H__
