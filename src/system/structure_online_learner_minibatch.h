#ifndef __ZUOPAR_SYSTEM_STRUCTURE_ONLINE_LEARNER_MINIBATCH_H__
#define __ZUOPAR_SYSTEM_STRUCTURE_ONLINE_LEARNER_MINIBATCH_H__

#include <vector>
#include <boost/thread.hpp>
#include "utils/logging.h"
#include "system/structure_online_learner.h"

namespace ZuoPar {

template <
  class _ActionType,
  class _StateType,
  class _ModelType,
  class _LossFunctionType
>
class TransitionStructureOnlineLearnerMiniBatch
: public TransitionStructureOnlineLearner<
  _ActionType, _StateType, _ModelType, _LossFunctionType
> {
public:
  //!
  TransitionStructureOnlineLearnerMiniBatch(_ModelType* model)
    : TransitionStructureOnlineLearner<
      _ActionType, _StateType, _ModelType, _LossFunctionType
    >(model) {
    clear();
  }

  void clear() {
    predict_state_chain_batch.clear();
    correct_state_chain_batch.clear();
  }

  void regist(const _StateType* predict_state, const _StateType* correct_state) {
    boost::lock_guard<boost::mutex> lock(mtx);

    if (predict_state == correct_state) { return; }

    std::vector<const _StateType*> predict_states;
    std::vector<const _StateType*> correct_states;
    for (const _StateType* p = predict_state; p; p = p->previous) {
      predict_states.push_back(p);
    }
    for (const _StateType* p = correct_state; p; p = p->previous) {
      correct_states.push_back(p);
    }

    BOOST_ASSERT(predict_states.size() == correct_states.size());
    int i;
    for (i = predict_states.size()- 1; i >= 0; -- i) {
      if (predict_states[i]->last_action != correct_states[i]->last_action) {
        break;
      }
    }

    predict_state_chain_batch.push_back( std::vector<_StateType>(i+ 2) );
    correct_state_chain_batch.push_back( std::vector<_StateType>(i+ 2) );

    for (i = i+ 1; i >= 0; -- i) {
      predict_state_chain_batch.back()[i] = (*predict_states[i]);
      correct_state_chain_batch.back()[i] = (*correct_states[i]);
    }

    _TRACE << "minibatch: regist is done.";
  }

  void learn() {
    BOOST_ASSERT(predict_state_chain_batch.size() == correct_state_chain_batch.size());
    for (int b = 0; b < predict_state_chain_batch.size(); ++ b) {
      this->predict_state_chain.clear();
      this->correct_state_chain.clear();

      BOOST_ASSERT(predict_state_chain_batch[b].size() == correct_state_chain_batch[b].size());

      for (int i = 0; i < predict_state_chain_batch[b].size(); ++ i) {
        this->predict_state_chain.push_back(&predict_state_chain_batch[b][i]);
        this->correct_state_chain.push_back(&correct_state_chain_batch[b][i]);
      }

      this->execute(this->predict_state_chain.size() - 1);
    }
  }
private:
  std::vector< std::vector<_StateType> > predict_state_chain_batch;
  std::vector< std::vector<_StateType> > correct_state_chain_batch;

  boost::mutex mtx;
};

} //  namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_STRUCTURE_LEARNER_MINIBATCH_H__
