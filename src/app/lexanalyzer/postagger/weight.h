#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__

#include "types/common.h"
#include "model/meta_feature.h"
#include "model/feature.h"
#include "model/associated/feature_param_map_v2.h"
#include "model/associated/feature_param_map_collection.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/state.h"
#include "app/lexanalyzer/postagger/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

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

  void vectorize(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector* sparse_vector) {}

  void vectorize2(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector2* sparse_vector) {}

  void vectorize3(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector3* sparse_vector) {}

  floatval_t score(const ScoreContext& ctx, const Action& act, bool avg) {
    floatval_t ret = 0;
    ret += UnigramFeatureCollection::score(ctx, act, avg);
    ret += BigramFeatureCollection::score(ctx, act, avg);
    ret += TrigramFeatureCollection::score(ctx, act, avg);
    ret += StringFeatureCollection::score(ctx, act, avg);
    return ret;
  }

  void batchly_score(const ScoreContext& ctx, const std::vector<Action>& actions,
      bool avg, PackedScores<Action>& result) {
    UnigramFeatureCollection::batchly_score(ctx, actions, avg, result);
    BigramFeatureCollection::batchly_score(ctx, actions, avg, result);
    TrigramFeatureCollection::batchly_score(ctx, actions, avg, result);
    StringFeatureCollection::batchly_score(ctx, actions, avg, result);
  }

  void update(const ScoreContext& ctx, const Action& act, int timestamp,
      floatval_t scale) {
    UnigramFeatureCollection::update(ctx, act, timestamp, scale);
    BigramFeatureCollection::update(ctx, act, timestamp, scale);
    TrigramFeatureCollection::update(ctx, act, timestamp, scale);
    StringFeatureCollection::update(ctx, act, timestamp, scale);
  }

  void flush(int timestamp) {
    UnigramFeatureCollection::flush(timestamp);
    BigramFeatureCollection::flush(timestamp);
    TrigramFeatureCollection::flush(timestamp);
    StringFeatureCollection::flush(timestamp);
  }

  bool save(std::ostream& os) {
    UnigramFeatureCollection::save(os);
    BigramFeatureCollection::save(os);
    TrigramFeatureCollection::save(os);
    StringFeatureCollection::save(os);
    return true;
  }

  bool load(std::istream& is) {
    UnigramFeatureCollection::load(is);
    BigramFeatureCollection::load(is);
    TrigramFeatureCollection::load(is);
    StringFeatureCollection::load(is);
    return true;
  }
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_MODEL_H__
