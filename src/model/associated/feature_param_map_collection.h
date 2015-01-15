#ifndef __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__

#include "types/internal/packed_scores.h"
#include "types/math/sparse_vector.h"
#include "model/meta_feature.h"
#include <vector>

namespace ZuoPar {

template <
  class _ScoreContextType,
  class _ActionType,
  class _FeatureMapType
>
class FeatureParameterCollection {
public:
  //! Define the packed score type.
  typedef PackedScores<_ActionType> packed_score_t;
public:
  FeatureParameterCollection() {}

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  ctx           The score context
   *  @param[in]  act           The action
   *  @param[in]  avg           Specify to use averaged parameter.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _ScoreContextType& ctx, const _ActionType& act, floatval_t scale,
      SparseVector* sparse_vector) {
    int global_id = 0;
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].vectorize(ctx, act, scale, global_id, sparse_vector);
      global_id += repo[i].size();
    }
  }

  /**
   * Convert the pointwised feature collections into vector.
   *
   *  @param[in]  ctx           The score context
   *  @param[in]  act           The action
   *  @param[in]  avg           Specify to use averaged parameter.
   *  @param[out] sparse_vector The version 2 sparse vector.
   */
  void vectorize2(const _ScoreContextType& ctx, const _ActionType& act, floatval_t scale,
      SparseVector2* sparse_vector) {
    int global_id = 0;
    for (int i = 0; i < repo.size(); ++ i, ++ global_id) {
      repo[i].vectorize2(ctx, act, scale, global_id, sparse_vector);
    }
  }

  /**
   * Get score for the state context.
   *
   *  @param[in]  ctx   The input state context.
   *  @param[in]  act   The action.
   *  @param[in]  avg   The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state
   *                           context.
   */
  floatval_t score(const _ScoreContextType& ctx, const _ActionType& act, bool avg) {
    floatval_t ret = 0;
    for (int i = 0; i < repo.size(); ++ i) {
      ret += repo[i].score(ctx, act, avg, 0.);
    }

    return ret;
  }

  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      packed_score_t& result) {
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].batchly_score(ctx, actions, avg, result);
    }
  }

  /**
   * Update the model with the state context.
   *
   *  @param[in]  ctx       The input state context.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const _ScoreContextType& ctx, const _ActionType& act, int timestamp,
      floatval_t scale) {
    for (std::size_t i = 0; i < repo.size(); ++ i) {
      repo[i].update(ctx, act, timestamp, scale);
    }
  }

  /**
   * flush the score.
   */
  void flush(int timestamp) {
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].flush(timestamp);
    }
  }

  /**
   * Save the model into the output stream.
   *
   *  @param[out] os    The output stream.
   *  @return     bool  If successfully saved, return true; otherwise return
   *                    false.
   */
  bool save(std::ostream& os) {
    boost::archive::text_oarchive oa(os);
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].save(oa);
    }
    return true;
  }

  /**
   * Load the model fron input stream.
   *
   *  @param[in]  is    The input stream.
   *  @return     bool  If successfully loaded, return true; otherwise return
   *                    false.
   */
  bool load(std::istream& is) {
    boost::archive::text_iarchive ia(is);
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].load(ia);
    }
    return true;
  }

protected:
  //! The unigram score mapping repository.
  std::vector< _FeatureMapType > repo;
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_COLLECTION_H__
