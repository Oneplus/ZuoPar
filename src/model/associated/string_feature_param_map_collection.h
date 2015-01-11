#ifndef __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__
#define __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__

#include "types/internal/packed_scores.h"
#include "model/associated/feature_param_map.h"
#include "model/associated/feature_param_map_collection.h"
#include <vector>

namespace ZuoPar {

template <
  class _StateType,
  class _ScoreContextType,
  class _ActionType
>
class StringFeatureParameterCollection
: public FeatureParameterCollection<
  _StateType,
  _ScoreContextType,
  _ActionType
> {
public:
  //! Instantiate the unigram score type
  typedef Feature<std::string, _ActionType> feature_t;
  //! Instantiate the unigram mapping
  typedef FeatureParameterMap< std::string, _ScoreContextType, _ActionType > map_t;
  //! Define the packed score type.
  typedef PackedScores<_ActionType> packed_score_t;
public:
  StringFeatureParameterCollection() {}

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
    for (std::size_t i = 0; i < sfeat_map_repo.size(); ++ i) {
      ret += sfeat_map_repo[i].score(ctx, act, avg, 0.);
    }
    ret += FeatureParameterCollection<_StateType,
            _ScoreContextType,
            _ActionType>::score(ctx, act, avg);
    return ret;
  }


  void batchly_score(const _ScoreContextType& ctx,
      const std::vector<_ActionType>& actions,
      bool avg,
      packed_score_t& result) {
    for (std::size_t i = 0; i < sfeat_map_repo.size(); ++ i) {
      sfeat_map_repo[i].batchly_score(ctx, actions, avg, result);
    }
    FeatureParameterCollection<_StateType,
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
    for (std::size_t i = 0; i < sfeat_map_repo.size(); ++ i) {
      sfeat_map_repo[i].update(ctx, act, timestamp, scale);
    }
    FeatureParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::update(ctx, act, timestamp, scale);
  }

  /**
   */
  void flush(int timestamp) {
    for (int i = 0; i < sfeat_map_repo.size(); ++ i) {
      sfeat_map_repo[i].flush(timestamp);
    }
    FeatureParameterCollection<_StateType, _ScoreContextType,
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
    for (int i = 0; i < sfeat_map_repo.size(); ++ i) {
      sfeat_map_repo[i].save(oa);
    }
    FeatureParameterCollection<_StateType, _ScoreContextType,
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
    for (int i = 0; i < sfeat_map_repo.size(); ++ i) {
      sfeat_map_repo[i].load(ia);
    }
    FeatureParameterCollection<_StateType, _ScoreContextType,
      _ActionType>::load(ia);
    return true;
  }

protected:
  //! The unigram score mapping repository.
  std::vector< map_t > sfeat_map_repo;
};

} //  end for zuopar

#endif  //  end for __ZUOPAR_MODEL_STRING_STRING_FEATURE_MAP_COLLECTION_H__
