#include <iostream>
#include <fstream>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include "utils/logging.h"
#include "app/seqlabeler/multi_pipe.h"
#include "app/seqlabeler/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

MultiPipe::MultiPipe(const MultiLearnOption& opts)
  : minibatch_learner(0), Pipe(static_cast<LearnOption>(opts)) {
  _INFO << "::MULTI-LEARN:: mode is activated.";
  this->batch_size = opts.batch_size;
  this->num_threads = opts.num_threads;
}

void
MultiPipe::run() {
  if (!setup()) {
    return;
  }
  _INFO << "report: batch size = " << batch_size;
  _INFO << "report: number of threads = " << num_threads;

  decoder_pool.resize(num_threads);
  for (int i = 0; i < num_threads; ++ i) {
    decoder_pool[i] = new Decoder(tags_alphabet.size(),
        beam_size, early_update, weight);
  }

  minibatch_learner = new MinibatchLearner(weight);

  std::size_t N = dataset.size();
  int nr_batches = (N % batch_size == 0? N / batch_size: N/batch_size+ 1);
  for (std::size_t batch_id = 0; batch_id < nr_batches; ++ batch_id) {
    //! Producer
    std::size_t start = batch_id* batch_size;
    std::size_t end = std::min(N, (batch_id+ 1) * batch_size);
    for (std::size_t n = start; n < end; ++ n) {
      queue.push(&dataset[n]);
    }

    boost::thread_group decoder_threads;
    last_free_decoder_id = 0;
    for (int i = 0; i < num_threads; ++ i) {
      decoder_threads.create_thread(boost::bind(&MultiPipe::decode, this));
    }
    decoder_threads.join_all();

    minibatch_learner->set_timestamp(start+ 1);
    minibatch_learner->learn();
    minibatch_learner->clear();

    _INFO << "pipe: finish learning batch#" << batch_id;
  }

  minibatch_learner->set_timestamp(N);
  minibatch_learner->flush();

  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save model.";
  } else {
    attributes_alphabet.save(mfs);
    tags_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "pipe: model saved to " << model_path;
  }
}

void
MultiPipe::decode() {
  Decoder* decoder = get_decoder();
  const SequenceInstance* data;
  while (queue.pop(data)) {
    int max_nr_actions = data->size();
    //_TRACE << "nr actions: " << max_nr_actions;

    State init_state(data);
    std::vector<Action> actions;
    ActionUtils::get_oracle_actions((*data), actions);
    Decoder::const_decode_result_t result = decoder->decode(init_state,
        actions, max_nr_actions);

    minibatch_learner->regist(result.first, result.second);
  }
}

Decoder*
MultiPipe::get_decoder() {
  boost::lock_guard<boost::mutex> lock(mtx);
  BOOST_ASSERT(last_free_decoder_id < decoder_pool.size());
  return decoder_pool[last_free_decoder_id++];
}

} //  namespace sequencelabeler
} //  namespace zuopar
