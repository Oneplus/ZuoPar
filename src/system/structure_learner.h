#ifndef __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__
#define __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__

#include <vector>
#include "utils/logging.h"

namespace ZuoPar {

template <class _ActionType,
          class _StateType,
          class _ModelType>
class TransitionStructureLearner {
public:
  TransitionStructureLearner(_ModelType* m)
    : model(m) {
  }

  void set_timestamp(int ts) {
    timestamp = ts;
  }

  void learn(const _StateType* predict_state,
      const _StateType* correct_state) {
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

    //BOOST_ASSERT(predict_state_chain.size() == correct_state_chain.size());
    int i;
    for (i = predict_state_chain.size()- 1; i >= 0; -- i) {
      if (predict_state_chain[i]->last_action != correct_state_chain[i]->last_action) {
        break;
      }
    }

    for (i = i+ 1; i > 0; -- i) {
      const _ActionType& predict_action = predict_state_chain[i- 1]->last_action;
      const _ActionType& correct_action = correct_state_chain[i- 1]->last_action;
      _TRACE << "learn: update " << (void *)predict_state_chain[i] << ", " << predict_action
        << " by -1";
      _TRACE << "learn: update " << (void *)correct_state_chain[i] << ", " << correct_action
        << " by +1";
      model->update((*predict_state_chain[i]), predict_action, timestamp, -1.);
      model->update((*correct_state_chain[i]), correct_action, timestamp, 1.);
    }
  }

  void flush() {
    model->flush(timestamp);
  }
private:
  int timestamp;

  _ModelType* model;
  std::vector<const _StateType *> predict_state_chain;
  std::vector<const _StateType *> correct_state_chain;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_H__
