#ifndef __ZUOPAR_SYSTEM_ONLINE_LEARNER_H__
#define __ZUOPAR_SYSTEM_ONLINE_LEARNER_H__

#include "utils/logging.h"
#include "types/math/sparse_vector.h"
#include "system/options.h"

namespace ZuoPar {

template <
  class _StateType,
  class _ModelType
>
class OnlineLearner {
public:
  /**
   * The initializer of transition structure learner
   *
   *  @param[in]  m     The model
   *  @param[in]  algo  The learning algorithm.
   */
  OnlineLearner(_ModelType* m, LearningAlgorithm algo = kAveragePerceptron)
    : model(m), nr_errors(0), algorithm(algo) {}

  /**
   * Set time stamp to the learner.
   *
   *  @param[in]  ts    The timestamp
   */
  void set_timestamp(int ts) {  timestamp = ts; }

  void learn(const _StateType* predict_state,
      const _StateType* correct_state,
      const floatval_t& margin,
      const floatval_t* predict_score = NULL,
      const floatval_t* correct_score = NULL) {
    if (predict_state == correct_state) { return; }
    ++ nr_errors;

    if (algorithm == kAveragePerceptron) {
      _TRACE << "learn: update " << (void *)predict_state << " by " << -1;
      _TRACE << "learn: update " << (void *)correct_state << " by " << 1;
      model->update((*predict_state), timestamp, -1.);
      model->update((*correct_state), timestamp, 1.);
    } else if (algorithm == kPassiveAgressive) {
      floatval_t error = margin; //
      floatval_t score = 0.;
      floatval_t norm = 0.;

      SparseVector2 updated_vector;
      if (correct_score == NULL || predict_score == NULL) {
        score += model->score((*correct_state), false);
        score -= model->score((*predict_state), false);
      } else {
        score += (*correct_score);
        score -= (*predict_score);
      }
      /*model->vectorize2((*correct_state), 1., &updated_vector);
      model->vectorize2((*predict_state), -1., &updated_vector);

      for (SparseVector2::const_iterator i = updated_vector.begin();
          i != updated_vector.end(); ++ i) {
        norm += (i->second * i->second);
      }*/
      norm = model->diff_norm((*correct_state), (*predict_state));

      _TRACE << "learn: norm = " << norm;
      floatval_t step = 0.;
      if (norm < 1e-8 || (error - score) < 0.) {
        step = 0;
      } else {
        step = (error - score) / norm;
      }

      _TRACE << "learn: update " << (void *)predict_state << " by " << (-step);
      _TRACE << "learn: update " << (void *)correct_state << " by " << step;
      model->update((*correct_state), timestamp, step);
      model->update((*predict_state), timestamp, -step);
    }
  }

  void flush() { model->flush(timestamp); }
  int errors() const { return nr_errors; }

protected:
  int timestamp;
  int nr_errors;

  LearningAlgorithm algorithm;
  _ModelType* model;

};

} //  namespace zuopar
#endif  //  end for __ZUOPAR_SYSTEM_ONLINE_LEARNER_H__
