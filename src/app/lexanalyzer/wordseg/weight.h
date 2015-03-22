#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_MODEL_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_MODEL_H__

#include "types/common.h"
#include "model/feature.h"
#include "model/associated/feature_param_map_impl1.h"
#include "model/associated/feature_param_map_impl2.h"
#include "model/associated/feature_param_map_collection.h"
#include "app/lexanalyzer/wordseg/action.h"
#include "app/lexanalyzer/wordseg/state.h"
#include "app/lexanalyzer/wordseg/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

typedef Feature< std::string, Action > StringFeature;

typedef FeatureParameterMapImpl1<
  std::string, ScoreContext, Action
> StringFeatureMapImpl1;

typedef FeatureParameterMapImpl2<
  StringFeature, ScoreContext, Action
> StringFeatureMapImpl2;

typedef FeatureParameterCollection<
  StringFeatureMapImpl1, ScoreContext, Action
> StringFeatureCollectionImpl1;

typedef FeatureParameterCollection<
  StringFeatureMapImpl2, ScoreContext, Action
> StringFeatureCollectionImpl2;

class Weight
: public StringFeatureCollectionImpl1,
  public StringFeatureCollectionImpl2 {
public:
  Weight();

  void vectorize(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector* sparse_vector);

  void vectorize2(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector2* sparse_vector);

  void vectorize3(const ScoreContext& ctx, const Action& act, floatval_t scale,
      SparseVector3* sparse_vector);

  floatval_t score(const ScoreContext& ctx, const Action& act,
      bool avg) const;

  void batchly_score(const ScoreContext& ctx, const std::vector<Action>& actions,
      bool avg, PackedScores<Action>& result) const;

  void update(const ScoreContext& ctx, const Action& act, int timestamp,
      floatval_t scale);

  void flush(int timestamp);

  bool save(std::ostream& os) const;

  bool load(std::istream& is);
};

} //  namespace chinesewordsegmentor
} //  namespace lexicalanalyzer
} //  namespace zuopar

#endif  // end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_MODEL_H__
