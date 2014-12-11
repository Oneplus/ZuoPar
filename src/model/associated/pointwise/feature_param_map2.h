#ifndef __ZUOPAR_MODEL_FEATURE_PREFIX_ACTION_POINTWISE_PARAM_MAP_H__
#define __ZUOPAR_MODEL_FEATURE_PREFIX_ACTION_POINTWISE_PARAM_MAP_H__

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
 * The FeaturePointwiseParamMap is a 2-level map, it can be illustrated as.
 *
 *  Map [ FeaturePrefix ] [ Action ] = Parameter
 *
 * Previous implementation use the `Map [ Feature ] = parameter` structure,
 * and it runs slow.
 */
template <class _FeaturePrefixType,
          class _ScoreContextType,
          class _ActionType>
class FeaturePrefixActionPointwiseParamMap {
private:
  //! Define the parameter type.
  typedef MachineLearning::PointwiseParameter param_t;
  //! Define the map entry type.
  typedef boost::unordered_map<_ActionType, param_t> map_entry_t;
  //! Define the 2nd-level mapping type
  typedef boost::unordered_map<_FeaturePrefixType, map_entry_t> map_t;
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
    floatval_t ret = 0.;
    for (int i = 0; i < cache.size(); ++ i) {
      typename map_t::const_iterator itx = payload.find(cache[i]);
      if (itx == payload.end()) {
        continue;
      }

      typename map_entry_t::const_iterator itx2 = itx->second.find(act);
      if (itx2 == itx->second.end()) {
        continue;
      }

      if (avg) {
        ret += itx2->second.w_sum;
      } else {
        ret += itx2->second.w;
      }
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
      bool avg,
      boost::unordered_map<_ActionType, floatval_t>& result) {
    cache.clear();
    extractor(ctx, cache);
    for (int i = 0; i < cache.size(); ++ i) {
      typename map_t::const_iterator itx = payload.find(cache[i]);
      if (itx == payload.end()) {
        continue;
      }

      for (int j = 0; j < actions.size(); ++ j) {
        const _ActionType& act= actions[j];
        typename map_entry_t::const_iterator itx2 = itx->second.find(act);
        if (itx2 == itx->second.end()) {
          continue;
        }

        if (avg) {
          result[act] += itx2->second.w_sum;
        } else {
          result[act] += itx2->second.w;
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
      param_t& param = payload[cache[i]][act];
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
      for (typename map_entry_t::iterator itx2 = itx->second.begin();
          itx2 != itx->second.end(); ++ itx2) {
        itx2->second.flush(now);
      }
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
  //! The mapping payload.
  map_t payload;

  //! Use to cache extracted features.
  cache_t cache;

  //! The feature extractor.
  extractor_t extractor;
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_FEATURE_PREFIX_ACTION_POINTWISE_PARAM_MAP_H__
