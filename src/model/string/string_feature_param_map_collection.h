#ifndef __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__

#include "model/associated/pointwise/feature_param_map.h"
#include "model/associated/pointwise/feature_param_map_collection.h"
#include <vector>
#include <boost/unordered_map.hpp>

namespace ZuoPar {

template <class _StateType,
          class _ScoreContextType,
          class _ActionType>
class StringFeaturePointwiseParameterCollection: \
  public FeaturePointwiseParameterCollection<_StateType,
                                             _ScoreContextType,
                                             _ActionType> {
public:
  //! Instantiate the unigram score type
  typedef Feature<std::string, _ActionType> feature_t;
  //! Instantiate the unigram mapping
  typedef FeaturePointwiseParameterMap< std::string, _ScoreContextType, _ActionType > map_t;

public:
  StringFeaturePointwiseParameterCollection() {}

  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state.
   */
  floatval_t score(const _StateType& state, const _ActionType& act, bool avg) {
    _ScoreContextType ctx(state);
    return (score(ctx, act, avg));
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
    ret += FeaturePointwiseParameterCollection<_StateType,
            _ScoreContextType,
            _ActionType>::score(ctx, act, avg);
    return ret;
  }


  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      boost::unordered_map<_ActionType, floatval_t>& result) {
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].batchly_score(ctx, actions, avg, result);
    }
    FeaturePointwiseParameterCollection<_StateType,
      _ScoreContextType,
      _ActionType>::batchly_score(ctx, actions, avg, result);
  }

  /**
   * Update the model with the state.
   *
   *  @param[in]  state     The input state.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const _StateType& state, const _ActionType& act, int timestamp,
      floatval_t scale) {
    _ScoreContextType ctx(state);
    update(ctx, act, timestamp, scale);
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
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].update(ctx, act, timestamp, scale);
    }
    FeaturePointwiseParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::update(ctx, act, timestamp, scale);
  }

  /**
   */
  void flush(int timestamp) {
    for (int i = 0; i < repo.size(); ++ i) {
      repo[i].flush(timestamp);
    }
    FeaturePointwiseParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::flush(timestamp);
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
    FeaturePointwiseParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::save(oa);
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
    FeaturePointwiseParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::load(ia);
    return true;
  }

protected:
  //! The unigram score mapping repository.
  std::vector< map_t > repo;
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__
