#ifndef __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_H__
#define __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_H__

#include "settings.h"
#include "types/math/sparse_vector.h"
#include "types/internal/packed_scores.h"
#include "model/pointwise_param.h"
#include "utils/logging.h"
#include <boost/functional/hash.hpp>
#include <boost/serialization/map.hpp>

#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == dense_hash_map)
# include <google/dense_hash_map>
# include "utils/serialization/dense_hash_map.h"
#else
# include <unordered_map>
# include <boost/serialization/unordered_map.hpp> // boost ~1_57 required.
# warning ("use std::unordered_map which is slow")
#endif

namespace ZuoPar {

/**
 * FeaturePointwiseParamMap is the class use to store the model parameters
 * of the feature. One FeaturePointwiseParamMap maintains several Features.
 * These features are extracted from the same feature template.
 *
 * Map [ Feature ][ Action ] = Parameter
 *
 * 2015-03-07
 * ----------
 * Benchmark shows Map[ Feature ][ Action ] performs 13/9 times faster than
 * the Map[ FeatureAction ] implementation.
 */
template <
  class _MetaFeatureType,
  class _ScoreContextType,
  class _ActionType
>
class FeatureParameterMap {
private:
  //! Define the parameter type.
  typedef MachineLearning::PointwiseParameter param_t;

  //! Define the mapping type.
  //
  // 2015-03-08
  // ----------
  // Note: I've tried an alternative solution of google::dense_hash_map, but
  // the memory performance is rather poor owing to the default allocator in
  // google::dense_hash_map. Using the std::unordered_map scarifies the speed
  // performance a bit but greatly reduce the memory consumption.
  //
  // 2015-03-10
  // ----------
  // According to a benchmark, that std::map performs generally better than 
  // the std::unordered_map when the size of keys is small (less than 100).
  //
  // 2015-03-19
  // ----------
  // Considering the CoNLLX dataset, which has up to 200 actions, I switch
  // from map to unordered_map.
  //typedef std::map<_ActionType, param_t> entry_t;
  typedef std::unordered_map<
    _ActionType, param_t, boost::hash<_ActionType>
  > entry_t;

#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == dense_hash_map)
  typedef google::dense_hash_map<
    _MetaFeatureType, entry_t, boost::hash<_MetaFeatureType>
  > map_t;
#else
  typedef std::unordered_map<
    _MetaFeatureType, entry_t, boost::hash<_MetaFeatureType>
  > map_t;
#endif

  //! Define the cache type.
  typedef std::vector<_MetaFeatureType> cache_t;

  //! Define the functor type.
  typedef std::function<void(const _ScoreContextType&, cache_t&)> extractor_t;

  //! Define the packed score type.
  typedef PackedScores<_ActionType> packed_score_t;

public:
  /**
   * The constructor
   *
   *  @param[in]  extractor_  The extraction functor.
   */
  FeatureParameterMap(extractor_t extractor_): extractor(extractor_) {
#if defined(UNORDERED_MAP_IMPL) && (UNORDERED_MAP_IMPL == dense_hash_map)
    rep.set_empty_key(_MetaFeatureType());
#endif
  }

  /**
   * Convert the pointwised feature into a sparse vector. Actually, this is
   * a very slow implementation.
   *
   *  @param[in]  ctx           The score context
   *  @param[in]  act           The action
   *  @param[in]  scale         Increase the value in sparse_vector by scale.
   *  @param[in]  offset        The offset for counting vector.
   *  @param[out] sparse_vector The sparse vector.
   */
  void vectorize(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, int gid, SparseVector* sparse_vector) const {
    //! Use to cache extracted features.
    cache_t cache;
    extractor(ctx, cache);
    for (const _MetaFeatureType& c: cache) {
      std::size_t seed = 0;
      boost::hash_combine(seed, gid);
      boost::hash_combine(seed, c);
      boost::hash_combine(seed, act);
      (*sparse_vector)[seed] += scale;
    }
  }

  //! The version 2
  void vectorize2(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, int gid, SparseVector2* sparse_vector) const {
    cache_t cache;
    extractor(ctx, cache);
    for (const _MetaFeatureType& c: cache) {
      std::size_t seed = 0;
      boost::hash_combine(seed, act);
      boost::hash_combine(seed, c);

      SparseVector2Key key = SparseVector2Key(gid, seed);
      (*sparse_vector)[key] += scale;
    }
  }

  //! The version 3
  void vectorize3(const _ScoreContextType& ctx, const _ActionType& act,
      const floatval_t& scale, int gid, SparseVector3* sparse_vector) const {
    cache_t cache;
    extractor(ctx, cache);
    for (const _MetaFeatureType& c: cache) {
      SparseVector3Key key = SparseVector3Key(gid,
          boost::hash_value<_MetaFeatureType>(c),
          boost::hash_value<_ActionType>(act));
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
   *  @return     floatval_t  The score.
   */
  floatval_t score(const _ScoreContextType& ctx, const _ActionType& act,
      bool avg) const {
    cache_t cache;
    extractor(ctx, cache);
    floatval_t ret = 0.;
    for (const _MetaFeatureType& c: cache) {
      typename map_t::const_iterator result1 = rep.find(c);
      if (result1 == rep.end()) { continue; }

      const entry_t& entry = result1->second;
      typename entry_t::const_iterator result2 = entry.find(act);
      if (result2 == entry.end()) { continue; }
      ret += result2->second.dot(avg);
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
    cache_t cache;
    extractor(ctx, cache);
    for (const _MetaFeatureType& c: cache) {
      typename map_t::const_iterator result1 = rep.find(c);
      if (result1 == rep.end()) { continue; }
      const entry_t& entry = result1->second;
      // It's tricky to iterate over all the actions, but it provides faster
      // speed. It also take the advantage of ordered iteration, ordered 
      // insertation.
      for (typename entry_t::const_iterator i = entry.begin();
          i != entry.end(); ++ i) {
        const _ActionType& act = i->first;
        result[act] += i->second.dot(avg);
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
      int now, const floatval_t& scale = 1.) {
    cache_t cache;
    extractor(ctx, cache);
    for (const _MetaFeatureType& c: cache) {
      typename map_t::iterator result1 = rep.find(c);
      if (result1 != rep.end()) {
        entry_t& entry = result1->second;
        typename entry_t::iterator result2 = entry.find(act);
        if (result2 != entry.end()) {
          param_t& param = result2->second;
          param.add(now, scale);
        } else {
          entry[act] = param_t(scale, scale, now);
        }
      } else {
        rep[c][act] = param_t(scale, scale, now);
      }
    }
  }

  /**
   * Flush the last updated time for all the parameters.
   *
   *  @param[in]  now   The timestamp.
   */
  void flush(int now) {
    for (typename map_t::iterator result1 = rep.begin();
        result1 != rep.end(); ++ result1) {
      entry_t& entry = result1->second;
      for (typename entry_t::iterator result2 = entry.begin();
          result2 != entry.end(); ++ result2) {
        param_t& param = result2->second;
        param.flush(now);
      }
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

} //  namespace zuopar

#endif  //  end for __ZUOPAR_MODEL_FAST_ASSOCIATED_FEATURE_PARAM_MAP_H__
