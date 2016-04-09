#include <iostream>
#include <fstream>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include "utils/logging.h"
#include "frontend/common_pipe_utils.h"
#include "app/seqlabeler/trans/multi_pipe.h"
#include "app/seqlabeler/trans/action_utils.h"

namespace ZuoPar {
namespace SequenceLabeler {

MultiPipe::MultiPipe(const boost::program_options::variables_map& vm)
  : minibatch_learner(0), Pipe(vm) {
}

void MultiPipe::multi_learn() {
  if (!setup(conf["train"].as<std::string>(), dataset, true)) {
    return;
  }
  if (!conf.count("devel") || !setup(conf["devel"].as<std::string>(), devel_dataset, false)) {
    _WARN << "[PIP] development data is not loaded";
  }
  _INFO << "[PIP] " << attributes_alphabet.size() << " attribute(s) is detected.";
  _INFO << "[PIP] " << tags_alphabet.size() << " tag(s) is detected.";
  _INFO << "[RPT] batch size = " << batch_size;
  _INFO << "[RPT] number of threads = " << num_threads;

  load_constrain();
  decoder_pool.resize(num_threads);
  for (int i = 0; i < num_threads; ++ i) {
    decoder_pool[i] = new Decoder(tags_alphabet.size(), trans, conf["beam"].as<unsigned>(),
      false, get_update_strategy(conf["strategy"].as<std::string>()), weight);
  }

  minibatch_learner = new MinibatchLearner(weight);
  std::size_t N = dataset.size();
  
  std::string model_path = FrontEnd::get_model_name("seqlabel_mulit", conf);
  double best_score = 0.;
  unsigned n_seen = 0;
  int n_batches = ((N % batch_size) == 0 ? (N / batch_size) : (N / batch_size + 1));
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    std::random_shuffle(dataset.begin(), dataset.end());
    
    for (std::size_t batch_id = 0; batch_id < n_batches; ++ batch_id) {
      //! Producer
      ++n_seen;
      std::size_t start = batch_id * batch_size;
      std::size_t end = std::min(N, (batch_id + 1) * batch_size);
      for (std::size_t n = start; n < end; ++ n) { queue.push(&dataset[n]); }

      boost::thread_group decoder_threads;
      last_free_decoder_id = 0;
      for (int i = 0; i < num_threads; ++ i) {
        decoder_threads.create_thread(boost::bind(&MultiPipe::decode, this));
      }
      decoder_threads.join_all();
      
      minibatch_learner->set_timestamp(n_seen);
      minibatch_learner->learn();
      minibatch_learner->clear();
      
      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "[PIP] finish learning batch#" << n_seen % n_batches << "/" << n_seen / n_batches;
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        minibatch_learner->flush();
        double score = evaluate(devel_dataset);
        decoder->reset_use_avg();
        _INFO << "[PIP] evaluate at instance #" << n_seen << ", score: " << score;
        if (score > best_score) {
          _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
          save_model(model_path);
          best_score = score;
        }
      }
    }
    minibatch_learner->flush();
    _INFO << "[PIP] iter " << iter + 1 << " #errors: " << minibatch_learner->errors();
    minibatch_learner->clear_errors();
    double score = evaluate(devel_dataset);
    decoder->reset_use_avg();
    _INFO << "[PIP] evaluate at the end of iteration#" << iter + 1 << " score: " << score;
    if (score > best_score) {
      _INFO << "[PIP] NEW best model is achieved, save to " << model_path;
      save_model(model_path);
      best_score = score;
    }
  }
}

void MultiPipe::decode() {
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

Decoder* MultiPipe::get_decoder() {
  boost::lock_guard<boost::mutex> lock(mtx);
  BOOST_ASSERT(last_free_decoder_id < decoder_pool.size());
  return decoder_pool[last_free_decoder_id++];
}

} //  namespace sequencelabeler
} //  namespace zuopar
