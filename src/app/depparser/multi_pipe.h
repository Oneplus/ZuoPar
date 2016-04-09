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
  DependencyMultiPipe(const boost::program_options::variables_map& vm)
    : minibatch_learner(0),
    DependencyPipe<
      Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
    >(vm) {
    _INFO << "report: batch size = " << vm["batch"].as<unsigned>();
    _INFO << "report: number of threads = " << vm["threads"].as<unsigned>();
  }

  //!
  void learn() {
    if (!this->setup(vm["train"].as<std::string>(), dataset, true)) { return;  }

    unsigned n_threads = conf["threads"].as<unsigned>();
    unsigned batch_size = conf["batch"].as<unsigned>();
 
    decoder_pool.resize(n_threads);
    deprel_t root_tag = this->deprels_alphabet.encode(this->root.c_str());
    for (int i = 0; i < n_threads; ++ i) {
      decoder_pool[i] = new Decoder(
        this->deprels_alphabet.size(), root_tag,
        this->conf["beam"].as<unsigned>(), false,
        get_update_stragey(conf["update"].as<std::string>()),
        this->weight);
    }

    minibatch_learner = new MinibatchLearner(this->weight);
    unsigned n_seen = 0, N = this->dataset.size();
    unsigned n_batches = (N % batch_size == 0 ? N / batch_size: N / batch_size+ 1);
    for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
      for (unsigned batch_id = 0; batch_id < nr_batches; ++batch_id) {
        //! Producer
        unsigned start = batch_id * batch_size;
        unsigned end = (batch_id + 1) * batch_size; if (end > N) { end = N; }
        for (unsigned n = start; n < end; ++n) {
          queue.push(&(this->dataset[n]));
        }
        boost::thread_group decoder_threads;
        last_free_decoder_id = 0;
        for (unsigned i = 0; i < n_threads; ++i) {
          decoder_threads.create_thread(
            boost::bind(&DependencyMultiPipe<
            Action, ActionUtils, State, Weight, Decoder, Learner,
            MinibatchLearner, MaxNumberOfActionsFunction>::decode,
            this));
        }
        decoder_threads.join_all();
        minibatch_learner->set_timestamp(n_seen);
        minibatch_learner->learn();
        minibatch_learner->clear();
        if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
          _INFO << "pipe: processed " << n_seen % n_batches <<
            "/" << n_seen / n_batches << " instances.";
        }
        if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
          minibatch_learner->flush();
          double score = evaluate(devel_dataset);
          decoder->reset_use_avg();
          _INFO << "pipe: evaluate score: " << score;
          if (score > best_score) {
            _INFO << "pipe: NEW best model is achieved, save to " << model_path;
            save_model(model_path);
            best_score = score;
          }
        }
      }

      minibatch_learner->flush();
      _INFO << "pipe: iter" << iter + 1 << " #errros: " << minibatch_learner->errors();
      minibatch_learner->clear_errors();
      double score = evaluate(devel_dataset);
      decoder->reset_use_avg();
      _INFO << "pipe: evaluate at the end of iteration#" << iter + 1 << " score: " << score;
      if (score > best_score) {
        _INFO << "pipe: NEW best model is achieved, save to " << model_path;
        save_model(model_path);
        best_score = score;
      }
    }
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
  CoNLLXDependencyMultiPipe(const boost::program_options::variables_map& vm)
    : minibatch_learner(0),
    CoNLLXDependencyPipe<
      Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberOfActionsFunction
    >(vm) {
    _INFO << "report: batch size = " << vm["batch"].as<unsigned>();
    _INFO << "report: number of threads = " << vm["threads"].as<unsigned>();
  }

  //!
  void learn() {
    if (!this->setup(this->conf["train"].as<std::string>(), dataset, true)) {
      return;
    }
    if (this->conf.count("devel")) {
      this->setup(this->conf["devel"].as<std::string>(), devel_dataset, false); 
    }
    unsigned n_threads = this->conf["threads"].as<unsigned>();
    unsigned batch_size = this->conf["batch"].as<unsigned>();

    decoder_pool.resize(n_threads);
    deprel_t root_tag = this->deprels_alphabet.encode(this->root.c_str());

    for (int i = 0; i < n_threads; ++ i) {
      decoder_pool[i] = new Decoder(
          this->deprels_alphabet.size(), root_tag, Decoder::kLeft,
          this->conf["beam"].as<unsigned>(),
          false,
          get_update_strategy(this->conf["update"].as<std::string>()),
          this->weight);
    }

    minibatch_learner = new MinibatchLearner(this->weight);
    std::string model_path = FrontEnd::get_model_name(signature, this->conf);
    unsigned n_seen = 0, N = this->dataset.size();
    int n_batches = (N % batch_size == 0 ? N / batch_size : N / batch_size + 1);
    double best_score = 0.;
    for (unsigned iter = 0; iter < this->conf["maxiter"].as<unsigned>(); ++iter) {
      std::random_shuffle(dataset.begin(), dataset.end());
      for (unsigned batch_id = 0; batch_id < n_batches; ++ batch_id) {
        //! Producer
        n_seen++;
        unsigned start = batch_id * batch_size;
        unsigned end = ((batch_id + 1) * batch_size); if (end > N) { end = N; }
        for (std::size_t n = start; n < end; ++ n) {
          queue.push(&(this->dataset[n]));
        }
        boost::thread_group decoder_threads;
        last_free_decoder_id = 0;
        for (int i = 0; i < n_threads; ++ i) {
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
        if (n_seen % this->conf["report_stops"].as<unsigned>() == 0) {
          _INFO << "pipe: finish learning batch#" << n_seen % n_batches << "/" << n_seen / n_batches;
        }
        if (n_seen % this->conf["evaluate_stops"].as<unsigned>() == 0) {
          learner->flush();
          double score = evaluate(devel_dataset);
          decoder->reset_use_avg();
          _INFO << "pipe: evaluate score: " << score;
          if (score > best_score) {
            _INFO << "pipe: NEW best model is achieved, save to " << model_path;
            save_model(model_path);
            best_score = score;
          }
        }
      }
      
      _INFO << "pipe: learn " << n_batches << " batches";
      minibatch_learner->set_timestamp(n_seen);
      minibatch_learner->flush();
      _INFO << "pipe: nr errors: " << minibatch_learner->errors();
      double score = evaluate(devel_dataset);
      if (score > best_score) {
        _INFO << "pipe: NEW best model is achieved, save to " << model_path;
        save_model(model_path);
        best_score = score;
      }
    }
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

  int last_free_decoder_id; //! The index of the last free decoder in decoder pool.
  std::vector<Decoder *> decoder_pool;  //! The pool of decoders
  MinibatchLearner* minibatch_learner;  //! The learner
  boost::lockfree::queue<const CoNLLXDependency*> queue;  //! The job queue
  boost::mutex mtx;  //!  The mutex
};


} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_MULTI_PIPE_H__
