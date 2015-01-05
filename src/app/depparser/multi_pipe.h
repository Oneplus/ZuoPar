#ifndef __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__

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
  class MinibatchLearner
>
class CommonDependencyMultiPipe: public CommonDependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner> {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  CommonDependencyMultiPipe(const fe::MultiLearnOption& opts)
    : minibatch_learner(0),
    CommonDependencyPipe<
      Action,
      ActionUtils,
      State,
      Weight,
      Decoder,
      Learner
    >(static_cast<fe::LearnOption>(opts)) {
    _INFO << "::MULTI-LEARN:: mode is activated.";
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
    for (int i = 0; i < num_threads; ++ i) {
      decoder_pool[i] = new Decoder(
          this->deprels_alphabet.size(),
          this->beam_size,
          this->early_update,
          this->weight);
    }

    minibatch_learner = new MinibatchLearner(this->weight);
    std::size_t N = this->dataset.size();
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
            boost::bind(&CommonDependencyMultiPipe<
              Action,
              ActionUtils,
              State,
              Weight,
              Decoder,
              Learner,
              MinibatchLearner>::decode, this));
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

  //! The decoders
  std::vector<Decoder *> decoder_pool;

  //! The learner
  MinibatchLearner* minibatch_learner;

  //!
  boost::lockfree::queue<const Dependency*> queue;

  //!
  boost::mutex mtx;
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__
