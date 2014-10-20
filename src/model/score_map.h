#ifndef __ZUOPAR_MODEL_SCORE_MAP_H__
#define __ZUOPAR_MODEL_SCORE_MAP_H__

#include "settings.h"
#include "ml/perceptron/param.h"
#include "utils/serialization/unordered_map.h"

namespace ZuoPar {

template <class _ScoreType,
          class _ScoreContextType,
          class _ActionType>
class ScoreMap {
private:
  //! Define the parameter type.
  typedef MachineLearning::Perceptron::Parameter param_t;
  //! Define the mapping type.
  typedef boost::unordered_map<_ScoreType, param_t> map_t;
  //! Define the cache type.
  typedef std::vector<_ScoreType> cache_t;
  //! Define the functor type.
  typedef std::function<void(const _ScoreContextType&,
      const _ActionType&, cache_t&)> extractor_t;

public:
  ScoreMap(extractor_t _extractor): extractor(_extractor) {}

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
    extractor(ctx, act, cache);
    //extractor(ctx, act);
    floatval_t ret = 0.;
    for (int i = 0; i < cache.size(); ++ i) {
      const _ScoreType& entry = cache[i];
      typename map_t::const_iterator itx = payload.find(entry);
      if (itx != payload.end()) {
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
    extractor(ctx, act, cache);
    for (int i = 0; i < cache.size(); ++ i) {
      const _ScoreType& entry = cache[i];
      param_t& param = payload[entry];
      param.add(now, scale);
    }
  }

  /**
   *
   *
   *
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
   *
   *
   */
  void save(boost::archive::text_oarchive& oa) {
    oa << payload;
  }

  /**
   * Loading the score map from the archive.
   *
   *  @param[in] ia   The input archive.
   */
  void load(boost::archive::text_iarchive& ia) {
    ia >> payload;
  }

private:
  map_t payload;
  cache_t cache;
  extractor_t extractor;
};


}

#endif  //  end for __ZGEN_SHIFTREDUCE_MODEL_SCORE_MAP_H__
