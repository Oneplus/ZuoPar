#ifndef __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_IMPL2_H__
#define __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_IMPL2_H__

#include "settings.h"
#include "types/math/sparse_vector.h"
#include "types/internal/packed_scores.h"
#include "model/pointwise_param.h"
#include "model/feature.h"
#include "utils/logging.h"
#include <boost/functional/hash.hpp>

#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == use_dense_hash_map)
# include <google/dense_hash_map>
# include "utils/serialization/dense_hash_map.h"
#else
# include <unordered_map>
# include "utils/serialization/unordered_map.h"
#endif

namespace ZuoPar {

/**
 * FeaturePointwiseParamMap is the class use to store the model parameters 
 * of the feature. One FeaturePointwiseParamMap maintains several Features.
 * These features are extracted from the same feature template.
 *
 * Map [ Feature ] = Parameter
 */
template <
  class _FeatureType,
  class _ScoreContextType,
  class _ActionType
>
class FeatureParameterMapImpl2 {
private:
  //! Define the parameter type.
  typedef MachineLearning::PointwiseParameter param_t;

  //! Define the mapping type.
#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == use_dense_hash_map)
  typedef google::dense_hash_map<
    _FeatureType,
    param_t,
    boost::hash<_FeatureType>
  > map_t;
#else
  typedef std::unordered_map<
    _FeatureType,
    param_t,
    boost::hash<_FeatureType>
  > map_t;
#endif

  //! Define the cache type.
  typedef std::vector<_FeatureType> cache_t;

  //! Define the functor type.
  typedef std::function<void(const _ScoreContextType&, const _ActionType&,
      cache_t&)> extractor_t;

  //! Define the packed score type.
  typedef PackedScores<_ActionType> packed_score_t;
public:
  /**
   * The constructor
   *
   *  @param[in]  extractor_  The extraction functor.
   */
  FeatureParameterMapImpl2(extractor_t extractor_): extractor(extractor_) {
#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == use_dense_hash_map)
    rep.set_empty_key(_FeatureType());
#endif
  }

  /**
   * Convert the pointwised feature into vector.
   *
   *  @param[in]  ctx           The score context.
   *  @param[in]  act           The action.
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[in]  offset        The offset for counting vector.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _ScoreContextType& ctx, const _ActionType& act,
      floatval_t scale, int gid, SparseVector* sparse_vector) {
    //! Use to cache extracted features.
    cache_t cache;
    extractor(ctx, act, cache);
    for (const _FeatureType& c: cache) {
      std::size_t seed = 0;
      boost::hash_combine(seed, gid);
      boost::hash_combine(seed, c);
      (*sparse_vector)[seed] += scale;
    }
  }

  //! The version 2
  void vectorize2(const _ScoreContextType& ctx, const _ActionType& act,
      floatval_t scale, int gid, SparseVector2* sparse_vector) {
    cache_t cache;
    extractor(ctx, act, cache);
    for (const _FeatureType& c: cache) {
      SparseVector2Key key = SparseVector2Key(gid,
          boost::hash_value<_FeatureType>(c));
      (*sparse_vector)[key] += scale;
    }
  }

  //! The version 3
  void vectorize3(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, int gid, SparseVector3* sparse_vector) const {
    cache_t cache;
    extractor(ctx, cache);
    for (const _FeatureType& c: cache) {
      SparseVector3Key key = SparseVector3Key(gid,
          boost::hash_value<_FeatureType>(c), 0);
      (*sparse_vector)[key] += scale;
    }
  }

  //! The size of the feature parameter map.
  std::size_t size() const { return rep.size(); }

  /**
   * Get the score for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  act   The action.
   *  @param[in]  avg   If avg is true return the averaged parameter, else
   *                    return the non-averaged parameter.
   *  @param[in]  default_return_value  The default return value.
   *  @return     floatval_t  The score.
   */
  floatval_t score(const _ScoreContextType& ctx, const _ActionType& act,
      bool avg) const {
    cache_t cache;
    extractor(ctx, act, cache);
    floatval_t ret = 0.;
    for (const _FeatureType& c: cache) {
      typename map_t::const_iterator itx = rep.find(c);
      if (itx == rep.end()) { continue; }
      ret += itx->second.dot(avg);
    }
    return ret;
  }

  /**
   * Batchly calculate the scores.
   *
   *  @param[in]  ctx     The scoring context.
   *  @param[in]  actions The actions.
   *  @param[in]  avg     If avg is true return the averaged parameter, else
   *                      return the non-averaged parameter.
   *  @param[out] result  The return value.
   */
  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg, packed_score_t& result) const {
    for (const _ActionType& a: actions) {
      cache_t cache;
      extractor(ctx, a, cache);
      for (const _FeatureType& c: cache) {
        typename map_t::const_iterator itx = rep.find(c);
        if (itx == rep.end()) { continue; }
        result[a] += itx->second.dot(avg);
      }
    }
  }

  /**
   * Update the parameter for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  act   The action
   *  @param[in]  now   The current timestamp.
   *  @param[in]  scale The updated scale.
   */
  void update(const _ScoreContextType& ctx, const _ActionType& act,
      int now, floatval_t scale = 1.) {
    cache_t cache;
    extractor(ctx, act, cache);
    for (const _FeatureType& c: cache) {
      param_t& param = rep[c];
      param.add(now, scale);
    }
  }

  /**
   * Flush the last updated time for all the parameters.
   *
   *  @param[in]  now   The timestamp.
   */
  void flush(int now) {
    for (typename map_t::iterator itx = rep.begin(); itx != rep.end();
        ++ itx) {
      itx->second.flush(now);
    }
  }

  /**
   * Saving the score map to the archive.
   *
   *  @param[in]  oa    The output archive stream.
   */
  void save(boost::archive::text_oarchive& oa) const { oa << rep; }

  /**
   * Loading the score map from the archive.
   *
   *  @param[in] ia   The input archive stream.
   */
  void load(boost::archive::text_iarchive& ia) { ia >> rep; }

private:
  //! The mapping facility.
  map_t rep;

  //! The feature extracting functor.
  extractor_t extractor;
};

} //  namespace zuopar

#endif  //  end for __ZUOPAR_MODEL_ASSOCIATED_FEATURE_PARAM_MAP_V2_H__
