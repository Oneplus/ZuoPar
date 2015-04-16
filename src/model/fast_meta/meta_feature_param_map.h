#ifndef __ZUOPAR_MODEL_FAST_META_META_FEATURE_PARAM_MAP_H__
#define __ZUOPAR_MODEL_FAST_META_META_FEATURE_PARAM_MAP_H__

#include "settings.h"
#include "types/math/sparse_vector.h"
#include "model/pointwise_param.h"
#include "utils/logging.h"
#include <boost/functional/hash.hpp>

#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == dense_hash_map)
# include <google/dense_hash_map>
# include "utils/serialization/dense_hash_map.h"
#else
# include <unordered_map>
# include "utils/serialization/unordered_map.h"
# warning ("use std::unordered_map which is slow")
#endif

namespace ZuoPar {

template<
  class _MetaFeatureType,
  class _ScoreContextType
>
class MetaFeatureParameterMap {
private:
  //! Define the parameter type.
  typedef MachineLearning::PointwiseParameter param_t;

  //! Define the mapping type.
#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == dense_hash_map)
  typedef google::dense_hash_map<
    _MetaFeatureType, param_t, boost::hash<_MetaFeatureType>
  > map_t;
#else
  typedef std::unordered_map<
    _MetaFeatureType, param_t, boost::hash<_MetaFeatureType>
  > map_t;
#endif
  //! Define the feature type.
  typedef _MetaFeatureType feature_t;

  //! Define the cache type.
  typedef std::vector<_MetaFeatureType> cache_t;

  //! Define the functor type.
  typedef std::function<void(const _ScoreContextType&, cache_t&)> extractor_t;

public:
  /**
   * The constructor
   *
   *  @param[in]  extractor_  The extraction functor.
   */
  MetaFeatureParameterMap(extractor_t extractor_)
    : extractor(extractor_) {
#if defined(UNORDERED_MAP_IMPL) and (UNORDERED_MAP_IMPL == dense_hash_map)
    rep.set_empty_key(feature_t());
#endif
  }

  /**
   * Convert the pointwised feature into vector.
   *
   *  @param[in]  ctx           The score context.
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[in]  offset        The offset for counting vector.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _ScoreContextType& ctx, const floatval_t& scale,
      int gid, SparseVector* sparse_vector) const {
    //! Use to cache extracted features.
    cache_t cache;
    extractor(ctx, cache);
    for (const feature_t& entry: cache) {
      std::size_t seed = 0;
      boost::hash_combine(seed, gid);
      boost::hash_combine(seed, entry);
      (*sparse_vector)[seed] += scale;
    }
  }

  /**
   * Convert the pointwised feature into sparse vector2 (a faster version of
   * representing sparse vector).
   *
   *  @param[in]  ctx           The score context.
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[in]  gid           The offset for counting vector.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize2(const _ScoreContextType& ctx, const floatval_t& scale,
      int gid, SparseVector2* sparse_vector) const {
    cache_t cache;
    extractor(ctx, cache);
    for (const feature_t& entry: cache) {
      const std::pair<int, std::size_t>& key =
        std::make_pair(gid, boost::hash_value<feature_t>(entry));
      (*sparse_vector)[key] += scale;
    }
  }

  //! The size of the feature parameter map.
  std::size_t size() const { return rep.size(); }

  /**
   * Get the score for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  avg   If avg is true return the averaged parameter, else
   *                    return the non-averaged parameter.
   *  @param[in]  default_return_value  The default return value.
   *  @return     floatval_t  The score.
   */
  floatval_t score(const _ScoreContextType& ctx, bool avg) const {
    cache_t cache;
    extractor(ctx, cache);
    floatval_t ret = 0.;
    for (const feature_t& entry: cache) {
      typename map_t::const_iterator itx = rep.find(entry);
      if (itx != rep.end()) {
        ret += itx->second.dot(avg);
      }
    }
    return ret;
  }

  /**
   * Update the parameter for the (context, action) pair
   *
   *  @param[in]  ctx   The scoring context.
   *  @param[in]  now   The current timestamp.
   *  @param[in]  scale The updated scale.
   */
  void update(const _ScoreContextType& ctx, int now, floatval_t scale = 1.) {
    cache_t cache;
    extractor(ctx, cache);
    for (const feature_t& entry: cache) {
      param_t& param = rep[entry];
      param.add(now, scale);
    }
  }

  /**
   * Flush the last updated time for all the parameters.
   *
   *  @param[in]  now   The timestamp.
   */
  void flush(int now) {
    for (typename map_t::iterator itx = rep.begin();
        itx != rep.end(); ++ itx) {
      itx->second.flush(now);
    }
  }

  /**
   * Saving the score map to the archive.
   *
   *  @param[in]  oa    The output archive stream.
   */
  void save(boost::archive::text_oarchive& oa) const {
    oa << rep;
  }

  /**
   * Loading the score map from the archive.
   *
   *  @param[in] ia   The input archive stream.
   */
  void load(boost::archive::text_iarchive& ia) {
    ia >> rep;
  }

private:
  //! The mapping facility.
  map_t rep;

  //! The feature extracting functor.
  extractor_t extractor;
};

}

#endif  //  end for __ZUOPAR_MODEL_META_META_FEATURE_PARAM_MAP_H__
