#ifndef __ZUOPAR_MODEL_MODEL_H__
#define __ZUOPAR_MODEL_MODEL_H__

#include "score.h"
#include "score_map.h"
#include <vector>

namespace ZuoPar {

template <class StateType,
          class ScoreContextType,
          class ActionType>
class Model {
public:
  //! Instantiate the unigram score type
  typedef UnigramScore<ActionType> us_t;
  //! Instantiate the bigram score type
  typedef BigramScore<ActionType>  bs_t;
  //! Instantiate the trigram score type
  typedef TrigramScore<ActionType> ts_t;
  //! Instantiate the unigram mapping
  typedef ScoreMap< us_t, ScoreContextType, ActionType > us_map_t;
  //! Instantiate the bigram mapping
  typedef ScoreMap< bs_t, ScoreContextType, ActionType > bs_map_t;
  //! Instantiate the trigram mapping
  typedef ScoreMap< ts_t, ScoreContextType, ActionType > ts_map_t;

public:
  Model();

  /**
   * Get score for the state.
   *
   *  @param[in]  state   The state
   *  @param[in]  act     The action
   *  @param[in]  avg     The average parameter.
   *  @return     floatval_t  The score of applying the action act to the state.
   */
  floatval_t score(const StateType& state, const ActionType& act, bool avg) {
    ScoreContextType ctx(state);
    return score(ctx, act, avg);
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
  floatval_t score(const ScoreContextType& ctx, const ActionType& act, bool avg) {
    floatval_t ret = 0;
    for (int i = 0; i < uscore_repo.size(); ++ i) {
      ret += uscore_repo[i].score(ctx, act, avg, 0.);
    }

    for (int i = 0; i < bscore_repo.size(); ++ i) {
      ret += bscore_repo[i].score(ctx, act, avg, 0.);
    }

    for (int i = 0; i < tscore_repo.size(); ++ i) {
      ret += tscore_repo[i].score(ctx, act, avg, 0.);
    }
    return ret;
  }

  /**
   * Update the model with the state.
   *
   *  @param[in]  state     The input state.
   *  @param[in]  act       The action.
   *  @param[in]  timestamp The updated timestamp.
   *  @param[in]  scale     The updated scale.
   */
  void update(const StateType& state, const ActionType& act, int timestamp,
      floatval_t scale) {
    ScoreContextType ctx(state);
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
  void update(const ScoreContextType& ctx, const ActionType& act, int timestamp,
      floatval_t scale) {
    for (int i = 0; i < uscore_repo.size(); ++ i) {
      uscore_repo[i].update(ctx, act, timestamp, scale);
    }

    for (int i = 0; i < bscore_repo.size(); ++ i) {
      bscore_repo[i].update(ctx, act, timestamp, scale);
    }

    for (int i = 0; i < tscore_repo.size(); ++ i) {
      tscore_repo[i].update(ctx, act, timestamp, scale);
    }
  }

  /**
   */
  void flush(int timestamp) {
    for (int i = 0; i < uscore_repo.size(); ++ i) {
      uscore_repo[i].flush(timestamp);
    }
    for (int i = 0; i < bscore_repo.size(); ++ i) {
      bscore_repo[i].flush(timestamp);
    }
    for (int i = 0; i < tscore_repo.size(); ++ i) {
      tscore_repo[i].flush(timestamp);
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
    for (int i = 0; i < uscore_repo.size(); ++ i) {
      uscore_repo[i].save(oa);
    }
    for (int i = 0; i < bscore_repo.size(); ++ i) {
      bscore_repo[i].save(oa);
    }
    for (int i = 0; i < tscore_repo.size(); ++ i) {
      tscore_repo[i].save(oa);
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
    for (int i = 0; i < uscore_repo.size(); ++ i) {
      uscore_repo[i].load(ia);
    }

    for (int i = 0; i < bscore_repo.size(); ++ i) {
      bscore_repo[i].load(ia);
    }

    for (int i = 0; i < tscore_repo.size(); ++ i) {
      tscore_repo[i].load(ia);
    }
    return true;
  }

private:
  //! The unigram score mapping repository.
  std::vector< us_map_t > uscore_repo;
  //! The bigram score mapping repository.
  std::vector< bs_map_t > bscore_repo;
  //! The trigram score mapping repository.
  std::vector< ts_map_t > tscore_repo;

  //! The unigram score caching facility.
  std::vector< us_t > uscore_cache;
  //! The bigram score cachine facility.
  std::vector< bs_t > bscore_cache;
  //! The trigram score caching facility.
  std::vector< ts_t > tscore_cache;
};

}

#endif  //  end for __ZUOPAR_MODEL_MODEL_H__
