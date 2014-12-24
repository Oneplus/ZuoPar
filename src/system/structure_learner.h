#ifndef __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__
#define __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__

#include <vector>
#include <cmath>
#include "utils/logging.h"

namespace ZuoPar {

template <class _ActionType,
          class _StateType,
          class _ModelType>
class TransitionStructureLearner {
public:
  enum LearningAlgorithm{
    kPassiveAgressive = 0,
    kAveragePerceptron
  };

public:
  /**
   * The initializer of transition structure learner
   *
   *  @param[in]  m     The model
   *  @param[in]  algo  The learning algorithm.
   */
  TransitionStructureLearner(_ModelType* m,
      LearningAlgorithm algo= kAveragePerceptron)
    : model(m),
    algorithm(algo) {
  }

  /**
   * Set time stamp to the learner.
   *
   *  @param[in]  ts    The timestamp
   */
  void set_timestamp(int ts) {
    timestamp = ts;
  }

  void learn(const _StateType* predict_state, const _StateType* correct_state) {
    if (predict_state == correct_state) {
      return;
    }

    predict_state_chain.clear();
    for (const _StateType* p = predict_state; p; p = p->previous) {
      predict_state_chain.push_back(p);
    }

    correct_state_chain.clear();
    for (const _StateType* p = correct_state; p; p = p->previous) {
      correct_state_chain.push_back(p);
    }

    BOOST_ASSERT(predict_state_chain.size() == correct_state_chain.size());
    for (int i = predict_state_chain.size() - 1; i >= 0; -- i) {
      _TRACE << "learn: from " << predict_state_chain[i] << " by "
        << predict_state_chain[i]->last_action;
    }

    for (int i = correct_state_chain.size() - 1; i >= 0; -- i) {
      _TRACE << "learn: from " << correct_state_chain[i] << " by "
        << correct_state_chain[i]->last_action;
    }

    int i;
    for (i = predict_state_chain.size()- 1; i >= 0; -- i) {
      if (predict_state_chain[i]->last_action != correct_state_chain[i]->last_action) {
        break;
      }
    }

    if (algorithm == kAveragePerceptron) {
      learn_average_perceptron(i+ 1);
    } else if (algorithm == kPassiveAgressive) {
      learn_passive_aggressive(i+ 1);
    }
  }

  void flush() {
    model->flush(timestamp);
  }
private:
  int timestamp;

  void learn_average_perceptron(int last) {
     for (int i = last; i > 0; -- i) {
      const _ActionType& predict_action = predict_state_chain[i- 1]->last_action;
      const _ActionType& correct_action = correct_state_chain[i- 1]->last_action;
      _TRACE << "learn: update " << (void *)predict_state_chain[i] << ", "
        << predict_action << " by -1";
      _TRACE << "learn: update " << (void *)correct_state_chain[i] << ", "
        << correct_action << " by +1";
      model->update((*predict_state_chain[i]), predict_action, timestamp, -1.);
      model->update((*correct_state_chain[i]), correct_action, timestamp, 1.);
    }
  }

  /**
   * Train the parameters with passive aggressive algorithm. According to 
   * K. Crammer et. al (2006), the one-best MIRA (also named passive aggressive)
   * has close form solution on derivation.
   *
   * w_t+1 = w_t + \tau_t * x_t, where \tau_t = l_t / || x_t ||^2
   *
   * For the learning guided search problem, we set the number of wrongly actions
   * and ther score as loss l_t
   */
  void learn_passive_aggressive(int last) {
    double error = last; //
    double score = 0.;
    double norm = 0.;

    SparseVector2 correct_vector;
    SparseVector2 predict_vector;

    for (int i = last; i > 0; -- i) {
      const _ActionType& predict_action = predict_state_chain[i- 1]->last_action;
      const _ActionType& correct_action = correct_state_chain[i- 1]->last_action;
      score += model->score((*correct_state_chain[i]), correct_action, false);
      score -= model->score((*predict_state_chain[i]), predict_action, false);
      model->vectorize2((*correct_state_chain[i]), correct_action, 1., &correct_vector);
      model->vectorize2((*predict_state_chain[i]), predict_action, -1, &predict_vector);
    }

    for (SparseVector2::const_iterator i = correct_vector.begin();
        i != correct_vector.end(); ++ i) {
      SparseVector2::const_iterator j = predict_vector.find(i->first);
      if (j == predict_vector.end()) {
        //Trick.
        norm += std::fabs(i->second);
      } else {
        if (std::fabs(i->second - j->second) > 1e-8) {
          norm += std::fabs(i->second - j->second);
        }
      }
    }
    for (SparseVector2::const_iterator i = predict_vector.begin();
        i != predict_vector.end(); ++ i) {
      SparseVector2::const_iterator j = correct_vector.find(i->first);
      if (j == correct_vector.end()) {
        norm += std::fabs(i->second);
      }
    }

    _TRACE << "learn: norm = " << norm;
    double step = 0.;
    if (norm < 1e-8) {
      step = 0;
    } else {
      step = (error - score) /norm;
    }

    for (int i = last; i > 0; -- i) {
      const _ActionType& predict_action = predict_state_chain[i- 1]->last_action;
      const _ActionType& correct_action = correct_state_chain[i- 1]->last_action;
      _TRACE << "learn: update " << (void *)predict_state_chain[i] << ", "
        << predict_action << " by " << (-step);
      _TRACE << "learn: update " << (void *)correct_state_chain[i] << ", "
        << correct_action << " by " << step;
      model->update((*predict_state_chain[i]), predict_action, timestamp, -step);
      model->update((*correct_state_chain[i]), correct_action, timestamp, step);
    }
  }

  LearningAlgorithm algorithm;
  _ModelType* model;
  std::vector<const _StateType *> predict_state_chain;
  std::vector<const _StateType *> correct_state_chain;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__