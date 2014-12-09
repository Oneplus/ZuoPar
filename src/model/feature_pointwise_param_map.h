#ifndef __ZUOPAR_MODEL_FEATURE_POINTWISE_PARAM_MAP_H__
#define __ZUOPAR_MODEL_FEATURE_POINTWISE_PARAM_MAP_H__

#include "settings.h"
#include "ml/pointwise_param.h"
#include "model/feature.h"
#include "utils/logging.h"
#include "utils/serialization/unordered_map.h"

namespace ZuoPar {

/**
 * FeaturePointwiseParamMap is the class use to store the model parameters 
 * of the feature. One FeaturePointwiseParamMap maintains several Features.
 * These features are extracted from the same feature template.
 *
 *
 *
 *
 */
template <class _FeaturePrefixType,
          class _ScoreContextType,
          class _ActionType>
class FeaturePointwiseParamMap {
private:
  typedef Feature<_FeaturePrefixType, _ActionType> feature_t;
  //! Define the parameter type.
  typedef MachineLearning::PointwiseParameter param_t;
  //! Define the mapping type.
  typedef boost::unordered_map<feature_t, param_t> map_t;
  //! Define the cache type.
  typedef std::vector<_FeaturePrefixType> cache_t;
  //! Define the functor type.
  typedef std::function<void(const _ScoreContextType&, cache_t&)> extractor_t;

public:
  FeaturePointwiseParamMap(extractor_t _extractor): extractor(_extractor) {}

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
      bool avg, floatval_t default_return_value = 0.) {
    cache.clear();
    extractor(ctx, cache);
    //extractor(ctx, act);
    floatval_t ret = 0.;
    for (int i = 0; i < cache.size(); ++ i) {
      const feature_t& entry = feature_t(cache[i], act);
      // _TRACE << "score " << (void *)this << ": score (try) " << entry;

      typename map_t::const_iterator itx = payload.find(entry);
      if (itx != payload.end()) {
        // _TRACE << "score: score (hit) " << entry;
        if (avg) {
          ret += itx->second.w_sum;
        } else {
          ret += itx->second.w;
        }
      }
    }
    return ret;
  }

  /**
   * Get
   */
  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      boost::unordered_map<_ActionType, floatval_t>& result) {
    cache.clear();
    extractor(ctx, cache);
    for (int i = 0; i < cache.size(); ++ i) {
      feature_t entry(cache[i], actions[0]);
      for (int j = 0; j < actions.size(); ++ j) {
        const _ActionType& act= actions[j];
        if (j > 0) {
          entry.replace_action(act);
        }

        typename map_t::const_iterator itx = payload.find(entry);
        if (itx == payload.end()) {
          continue;
        }
        // _TRACE << "score: score (hit) " << entry;
        if (avg) {
          result[act] += itx->second.w_sum;
        } else {
          result[act] += itx->second.w;
        }
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
    cache.clear();
    extractor(ctx, cache);
    for (int i = 0; i < cache.size(); ++ i) {
      const feature_t& entry = feature_t(cache[i], act);
      param_t& param = payload[entry];
      param.add(now, scale);
    }
  }

  /**
   * Flush the last updated time for all the parameters.
   *
   *  @param[in]  now   The timestamp.
   */
  void flush(int now) {
    for (typename map_t::iterator itx = payload.begin();
        itx != payload.end(); ++ itx) {
      itx->second.flush(now);
    }
  }

  /**
   * Saving the score map to the archive.
   *
   *  @param[in]  oa    The output archive stream.
   */
  void save(boost::archive::text_oarchive& oa) {
    oa << payload;
  }

  /**
   * Loading the score map from the archive.
   *
   *  @param[in] ia   The input archive stream.
   */
  void load(boost::archive::text_iarchive& ia) {
    ia >> payload;
  }

private:
  //!
  map_t payload;

  //! Use to cache extracted features.
  cache_t cache;

  //!
  extractor_t extractor;
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_FEATURE_POINTWISE_PARAM_MAP_H__
