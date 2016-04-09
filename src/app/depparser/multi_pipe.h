#ifndef __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__

#include <algorithm>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lockfree/queue.hpp>
#include "utils/logging.h"
#include "frontend/common_opt.h"
#include "app/depparser/pipe.h"

namespace ZuoPar {
namespace DependencyParser {

namespace eg = ZuoPar::Engine;
namespace fe = ZuoPar::FrontEnd;

template <
  class Action,
  class ActionUtils,
  class State,
  class Weight,
  class Decoder,
  class Learner,
  class MinibatchLearner,
  class MaxNumberOfActionsFunction
>
class DependencyMultiPipe: public DependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
> {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  DependencyMultiPipe(const MultiLearnOption& opts)
    : minibatch_learner(0),
    DependencyPipe<
      Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
    >(static_cast<const fe::LearnOption&>(opts)) {
    _INFO << "::MULTI-LEARN:: mode is activated.";
    this->root = opts.root;
    this->batch_size = opts.batch_size;
    this->num_threads = opts.num_threads;
    _INFO << "report: batch size = " << batch_size;
    _INFO << "report: number of threads = " << num_threads;
  }

  //!
  void run() {
    if (!this->setup()) {
      return;
    }
    decoder_pool.resize(num_threads);
    deprel_t root_tag = this->deprels_alphabet.encode(this->root.c_str());
    for (int i = 0; i < num_threads; ++ i) {
      decoder_pool[i] = new Decoder(
          this->deprels_alphabet.size(), root_tag, this->beam_size, false,
          this->update_strategy, this->weight);
    }

    minibatch_learner = new MinibatchLearner(this->weight);
    std::size_t N = this->dataset.size();
    std::vector<std::size_t> ranks;
    for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
    while (this->shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

    int nr_batches = (N % batch_size == 0? N / batch_size: N/batch_size+ 1);
    for (std::size_t batch_id = 0; batch_id < nr_batches; ++ batch_id) {
      //! Producer
      std::size_t start = batch_id* batch_size;
      std::size_t end = std::min(N, (batch_id+ 1) * batch_size);
      for (std::size_t n = start; n < end; ++ n) {
        queue.push(&(this->dataset[n]));
      }
      boost::thread_group decoder_threads;
      last_free_decoder_id = 0;
      for (int i = 0; i < num_threads; ++ i) {
        decoder_threads.create_thread(
            boost::bind(&DependencyMultiPipe<
              Action, ActionUtils, State, Weight, Decoder, Learner,
              MinibatchLearner, MaxNumberOfActionsFunction>::decode,
              this));
      }
      decoder_threads.join_all();
      minibatch_learner->set_timestamp(batch_id+ 1);
      minibatch_learner->learn();
      minibatch_learner->clear();
      if ((batch_id+ 1) % this->display_interval == 0) {
        _INFO << "pipe: finish learning batch#" << batch_id + 1;
      }
    }

    _INFO << "pipe: learn " << nr_batches << " batches";
    minibatch_learner->set_timestamp(nr_batches);
    minibatch_learner->flush();
    _INFO << "pipe: nr errors: " << minibatch_learner->errors();
    this->save_model(this->model_path);
  }
private:
  //!
  void decode() {
    Decoder* decoder = get_decoder();
    const Dependency* data;
    while (queue.pop(data)) {
      int max_nr_actions = data->size()* 2- 1;
      //_TRACE << "nr actions: " << max_nr_actions;
      State init_state(data);
      std::vector<Action> actions;
      ActionUtils::get_oracle_actions((*data), actions);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);
      minibatch_learner->regist(result.first, result.second);
    }
  }

  //!
  Decoder* get_decoder() {
    boost::lock_guard<boost::mutex> lock(mtx);
    BOOST_ASSERT(last_free_decoder_id < decoder_pool.size());
    return decoder_pool[last_free_decoder_id++];
  }

  //! The size of mini batch.
  int batch_size;

  //! The number of threads in.
  int num_threads;

  //! The index of the last free decoder in decoder pool.
  int last_free_decoder_id;

  std::vector<Decoder *> decoder_pool;  //! The pool of decoders
  MinibatchLearner* minibatch_learner;  //! The learner

  //! The job queue
  boost::lockfree::queue<const Dependency*> queue;

  //!
  boost::mutex mtx;
};

// The CoNLLXPipe ----------

template <
  class Action,
  class ActionUtils,
  class State,
  class Weight,
  class Decoder,
  class Learner,
  class MinibatchLearner,
  class MaxNumberOfActionsFunction
>
class CoNLLXDependencyMultiPipe: public CoNLLXDependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
> {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CoNLLXDependencyMultiPipe(const MultiLearnOption& opts)
    : minibatch_learner(0),
    CoNLLXDependencyPipe<
      Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
    >(static_cast<const fe::LearnOption&>(opts)) {
    _INFO << "::MULTI-LEARN:: mode is activated.";
    this->root = opts.root;
    this->batch_size = opts.batch_size;
    this->num_threads = opts.num_threads;
    _INFO << "report: batch size = " << batch_size;
    _INFO << "report: number of threads = " << num_threads;
  }

  //!
  void run() {
    if (!this->setup(this->reference_path, true)) { return; }
    decoder_pool.resize(num_threads);
    deprel_t root_tag = this->deprels_alphabet.encode(this->root.c_str());
    for (int i = 0; i < num_threads; ++ i) {
      decoder_pool[i] = new Decoder(
          this->deprels_alphabet.size(), root_tag, Decoder::kLeft, this->beam_size, false,
          this->update_strategy, this->weight);
    }

    minibatch_learner = new MinibatchLearner(this->weight);
    std::size_t N = this->dataset.size();
    std::vector<std::size_t> ranks;
    for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
    while (this->shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

    int nr_batches = (N % batch_size == 0? N / batch_size: N/batch_size+ 1);
    for (std::size_t batch_id = 0; batch_id < nr_batches; ++ batch_id) {
      //! Producer
      std::size_t start = batch_id* batch_size;
      std::size_t end = std::min(N, (batch_id + 1) * batch_size);
      for (std::size_t n = start; n < end; ++ n) {
        queue.push(&(this->dataset[n]));
      }
      boost::thread_group decoder_threads;
      last_free_decoder_id = 0;
      for (int i = 0; i < num_threads; ++ i) {
        decoder_threads.create_thread(
            boost::bind(&CoNLLXDependencyMultiPipe<
              Action, ActionUtils, State, Weight, Decoder, Learner,
              MinibatchLearner, MaxNumberOfActionsFunction>::decode,
              this));
      }
      decoder_threads.join_all();
      minibatch_learner->set_timestamp(batch_id+ 1);
      minibatch_learner->learn();
      minibatch_learner->clear();
      if ((batch_id+ 1) % this->display_interval == 0) {
        _INFO << "pipe: finish learning batch#" << batch_id + 1;
      }
    }

    _INFO << "pipe: learn " << nr_batches << " batches";
    minibatch_learner->set_timestamp(nr_batches);
    minibatch_learner->flush();
    _INFO << "pipe: nr errors: " << minibatch_learner->errors();
    this->save_model(this->model_path);
  }
private:
  //!
  void decode() {
    Decoder* decoder = get_decoder();
    const CoNLLXDependency* data;
    while (queue.pop(data)) {
      int max_nr_actions = data->size()* 2- 1;
      //_TRACE << "nr actions: " << max_nr_actions;
      State init_state(data);
      std::vector<Action> actions;
      ActionUtils::get_oracle_actions((*data), actions);
      typename Decoder::const_decode_result_t result = decoder->decode(init_state,
          actions, max_nr_actions);
      minibatch_learner->regist(result.first, result.second);
    }
  }

  //!
  Decoder* get_decoder() {
    boost::lock_guard<boost::mutex> lock(mtx);
    BOOST_ASSERT(last_free_decoder_id < decoder_pool.size());
    return decoder_pool[last_free_decoder_id++];
  }

  int batch_size;           //! The size of mini batch.
  int num_threads;          //! The number of threads in.
  int last_free_decoder_id; //! The index of the last free decoder in decoder pool.

  std::vector<Decoder *> decoder_pool;  //! The pool of decoders
  MinibatchLearner* minibatch_learner;  //! The learner

  boost::lockfree::queue<const CoNLLXDependency*> queue;  //! The job queue
  boost::mutex mtx;  //!  The mutex
};


} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__
