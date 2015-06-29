#include "app/depparser/nn/chen14/classifier.h"
#include "utils/logging.h"
#include <algorithm>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/timer/timer.hpp>

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Chen2014 {

NeuralNetworkClassifier::NeuralNetworkClassifier()
  : initialized(false),
  embedding_size(0),
  hidden_layer_size(0),
  nr_objects(0),
  nr_feature_types(0),
  nr_classes(0),
  iter(0) {
}

void NeuralNetworkClassifier::initialize(
    int _nr_objects,
    int _nr_classes,
    int _nr_feature_types,
    const LearnOption& opt,
    const std::vector< std::vector<floatval_t> >& embeddings,
    const std::vector<int>& precomputed_features
    ) {
  if (initialized) {
    _ERROR << "classifier: weight should not be initialized twice!";
    return;
  }

  nr_threads = opt.nr_threads;

  fix_embeddings = opt.fix_embeddings;
  dropout_probability = opt.dropout_probability;
  lambda = opt.lambda;

  ada_eps = opt.ada_eps;
  ada_lr = opt.ada_lr;

  // Initialize the parameter.
  nr_feature_types = _nr_feature_types;
  nr_objects = _nr_objects;
  nr_classes = _nr_classes; // nr_deprels*2+1-NIL

  embedding_size = opt.embedding_size;
  hidden_layer_size = opt.hidden_layer_size;

  // Initialize the network
  int nrows = hidden_layer_size;
  int ncols = embedding_size * nr_feature_types;

  W1 = (2.* arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows+ ncols));
  b1 = arma::zeros<arma::vec>(nrows);

  nrows = _nr_classes;  //
  ncols = hidden_layer_size;

  W2 = (2.* arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows+ ncols));

  // Initialized the embedding
  nrows = embedding_size;
  ncols= _nr_objects;

  E = (2.* arma::randu<arma::mat>(nrows, ncols) - 1.) * opt.init_range;

  for (auto& embedding: embeddings) {
    int id = embedding[0];
    for (unsigned j = 1; j < embedding.size(); ++ j) {
      E(j-1, id) = embedding[j];
    }
  }

  // Initialized the precomputed features
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  size_t rank = 0;
  for (auto& fid: precomputed_features) { encoder[fid] = rank ++; }
  saved.zeros(hidden_layer_size, encoder.size());

  cost.init(E.n_rows, E.n_cols, W1.n_rows, W1.n_cols, b1.n_rows,
      W2.n_rows, W2.n_cols, hidden_layer_size, encoder.size());

  costs.resize(nr_threads);
  for (auto& c: costs) {
    c.init(E.n_rows, E.n_cols, W1.n_rows, W1.n_cols, b1.n_rows,
        W2.n_rows, W2.n_cols, hidden_layer_size, encoder.size());
  }

  initialize_gradient_histories();
  initialized = true;
  info(true);
}

void NeuralNetworkClassifier::score(const std::vector<int>& attributes,
    std::vector<floatval_t>& retval) {
  const std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  arma::vec hidden = arma::zeros<arma::vec>(hidden_layer_size);

  for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
    auto& aid = attributes[i];
    auto fid = aid * nr_feature_types + i;
    auto rep = encoder.find(fid);
    if (rep != encoder.end()) {
      hidden+= saved.col(rep->second);
    } else {
      // W1[0:hidden_layer, off:off+embedding_size] * E[fid:]'
      hidden+= W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) * E.col(aid);
    }
  }

  hidden+= b1;
  hidden= hidden % hidden % hidden;

  arma::vec output = W2 * hidden;
  retval.resize(nr_classes, 0.);
  for (int i = 0; i < nr_classes; ++ i) { retval[i] = output(i); }
}

void NeuralNetworkClassifier::compute_cost_and_gradient(
    std::vector<Sample>::const_iterator begin,
    std::vector<Sample>::const_iterator end,
    bool debug) {
  if (!initialized) {
    _ERROR << "classifier: should not run the learning algorithm"
      " with un-initialized classifier.";
    return;
  }

  // precomputing
  std::unordered_set<int> precomputed_features;
  get_precomputed_features(begin, end, precomputed_features);
  precomputing(precomputed_features);

  // calculate cost and gradient
  compute_cost_and_gradient(begin, end, precomputed_features, debug);

  if (debug) { gradient_check(begin, end); }
}

void NeuralNetworkClassifier::initialize_gradient_histories() {
  eg2E = arma::zeros<arma::mat>(E.n_rows, E.n_cols);
  eg2W1 = arma::zeros<arma::mat>(W1.n_rows, W1.n_cols);
  eg2b1 = arma::zeros<arma::vec>(b1.n_rows);
  eg2W2 = arma::zeros<arma::mat>(W2.n_rows, W2.n_cols);
}

void NeuralNetworkClassifier::take_adagrad_step() {
  eg2W1 += cost.grad_W1 % cost.grad_W1;
  W1 -= ada_lr * cost.grad_W1 / arma::sqrt(eg2W1 + ada_eps);

  eg2b1 += cost.grad_b1 % cost.grad_b1;
  b1 -= ada_lr * cost.grad_b1 / arma::sqrt(eg2b1 + ada_eps);

  eg2W2 += cost.grad_W2 % cost.grad_W2;
  W2 -= ada_lr * cost.grad_W2 / arma::sqrt(eg2W2 + ada_eps);

  if (!fix_embeddings) {
    eg2E += cost.grad_E % cost.grad_E;
    E -= ada_lr * cost.grad_E / arma::sqrt(eg2E + ada_eps);
  }

  ++ iter;
}

floatval_t NeuralNetworkClassifier::get_cost() const { return cost.loss; }

floatval_t NeuralNetworkClassifier::get_accuracy() const { return cost.accuracy; }

void NeuralNetworkClassifier::get_precomputed_features(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end,
    std::unordered_set<int>& retval) {
  const std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  for (std::vector<Sample>::const_iterator sample = begin; sample != end; ++ sample) {
    for (auto i = 0; i < sample->attributes.size(); ++ i) {
      auto fid = sample->attributes[i]* nr_feature_types + i;
      if (encoder.find(fid) != encoder.end()) { retval.insert(fid); }
    }
  }
  _TRACE << "classifier: percentage of necessary precomputation: "
    << (double)retval.size() / encoder.size() * 100 << "%";
}

void NeuralNetworkClassifier::precomputing() {
  std::unordered_set<int> features;
  for (auto& rep: precomputation_id_encoder) { features.insert(rep.first); }
  precomputing(features);
}

void NeuralNetworkClassifier::precomputing(
    const std::unordered_set<int>& features) {
  saved.zeros();
  for (auto fid: features) {
    auto rank = precomputation_id_encoder[fid];
    auto aid = fid / nr_feature_types;
    auto off = (fid % nr_feature_types)*embedding_size;
    saved.col(rank) =
      W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) * E.col(aid);
  }
  _TRACE << "classifier: precomputed " << features.size();
}

floatval_t NeuralNetworkClassifier::compute_cost(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end,
    const std::vector< arma::uvec >& dropout_histories) {
  batch_size = end - begin;
  if (dropout_histories.size() != batch_size) {
    _ERROR << "gradient check: histories size not equals to batch size";
  }
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  floatval_t retval = 0.; int n = 0;
  for (std::vector<Sample>::const_iterator sample = begin; sample != end; ++ sample, ++ n) {
    const auto& attributes = sample->attributes;
    const auto& classes = sample->classes;

    arma::vec Y(classes);
    arma::uvec dropout_mask = dropout_histories[n];

    arma::vec lin_hidden = arma::zeros<arma::vec>(dropout_mask.n_rows);
    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        lin_hidden += saved.submat(dropout_mask, _);
      } else {
        arma::uvec __ = arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size);
        lin_hidden += (W1.submat(dropout_mask, __)* E.col(aid));
      }
    }

    lin_hidden += b1(dropout_mask);
    arma::vec grad_lin_hidden;
    arma::uvec classes_mask = arma::find(Y >= 0);

    arma::vec hidden = lin_hidden % lin_hidden % lin_hidden;
    arma::vec output = W2.cols(dropout_mask) * hidden;

    int opt_class = -1, correct_class = -1;
    for (auto i = 0; i < nr_classes; ++ i) {
      if (classes[i] >= 0 && (opt_class < 0 || output(i) > output(opt_class))) {
        opt_class = i; }
      if (classes[i] == 1) { correct_class = i; }
    }

    floatval_t best = output(opt_class);

    output(classes_mask) = arma::exp(output(classes_mask) - best);
    floatval_t sum1 = output(correct_class);
    floatval_t sum2 = arma::sum(output(classes_mask));

    retval += (log(sum2) - log(sum1));
  }

  retval /= batch_size;

  retval += (lambda * .5 * (arma::dot(W1, W1)+ arma::dot(b1, b1)+ arma::dot(W2, W2)));
  if (!fix_embeddings) {
    retval += (lambda * .5 * arma::dot(E, E));
  }

  return retval;
}

void NeuralNetworkClassifier::back_propagate_saved_one_thread() {
  int fid;
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  while (queue2.pop(fid)) {
    auto rank = encoder[fid];
    auto aid = fid / nr_feature_types;
    auto off = (fid % nr_feature_types)*embedding_size;

    cost.grad_W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) +=
      cost.grad_saved.col(rank) * E.col(aid).t();

    if (!fix_embeddings) {
      cost.grad_E.col(aid) +=
        W1.submat(0, off, hidden_layer_size-1, off+ embedding_size-1).t()
        * cost.grad_saved.col(rank);
    }
  }
}

void NeuralNetworkClassifier::compute_cost_and_gradient_one_thread(
    CostAndGradient* result) {
  result->zeros();
  job_t data;
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  while (queue.pop(data)) {
    const auto& attributes = data.first->attributes;
    const auto& classes = data.first->classes;

    arma::vec Y(classes);
    arma::uvec dropout_mask;
    if (data.second >= 0) {
      dropout_mask = dropout_histories[data.second];
    } else {
      dropout_mask = arma::find(
          arma::randu<arma::vec>(hidden_layer_size) > dropout_probability );
    }

    arma::vec lin_hidden = arma::zeros<arma::vec>(dropout_mask.n_rows);
    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        lin_hidden += saved.submat(dropout_mask, _);
      } else {
        arma::uvec __ = arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size);
        lin_hidden += (W1.submat(dropout_mask, __)* E.col(aid));
      }
    }

    lin_hidden += b1(dropout_mask);
    arma::vec grad_lin_hidden;
    arma::uvec classes_mask = arma::find(Y >= 0);

    arma::vec hidden = lin_hidden % lin_hidden % lin_hidden;
    arma::vec output = W2.cols(dropout_mask) * hidden;

    int opt_class = -1, correct_class = -1;
    for (auto i = 0; i < nr_classes; ++ i) {
      if (classes[i] >= 0 && (opt_class < 0 || output(i) > output(opt_class))) {
        opt_class = i; }
      if (classes[i] == 1) { correct_class = i; }
    }

    floatval_t best = output(opt_class);
    output(classes_mask) = arma::exp(output(classes_mask) - best);

    floatval_t sum1 = output(correct_class);
    floatval_t sum2 = arma::sum(output(classes_mask));

    result->loss += (log(sum2) - log(sum1));
    if (classes[opt_class] == 1) { result->accuracy += 1; }

    arma::vec delta = -(Y(classes_mask) - output(classes_mask) / sum2) / batch_size;
    result->grad_W2.submat(classes_mask, dropout_mask) += delta * hidden.t();
    grad_lin_hidden =
      3* (W2.submat(classes_mask, dropout_mask).t() * delta) % lin_hidden % lin_hidden;

    result->grad_b1(dropout_mask) += grad_lin_hidden;
    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        result->grad_saved.submat(dropout_mask, _) += grad_lin_hidden;
      } else {
        arma::uvec __ = arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size);
        result->grad_W1.submat(dropout_mask, __) += grad_lin_hidden * E.col(aid).t();
        if (!fix_embeddings) {
          result->grad_E.col(aid) += W1.submat(dropout_mask, __).t() * grad_lin_hidden;
        }
      }
    }
  }
}

void NeuralNetworkClassifier::compute_cost_and_gradient(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end,
    const std::unordered_set<int>& features,
    bool debug) {
  batch_size = end - begin;
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  boost::timer::cpu_timer t;
  dropout_histories.clear();
  auto n = 0;

  for (std::vector<Sample>::const_iterator sample = begin; sample != end; ++ sample, ++ n) {
    dropout_histories.push_back(arma::find(
          arma::randu<arma::vec>(hidden_layer_size) > dropout_probability)
        );
    queue.push(job_t(&(*sample), n));
  }

  boost::thread_group threads;
  for (auto t = 0; t < nr_threads; ++ t) {
    threads.create_thread(
        boost::bind(
          &NeuralNetworkClassifier::compute_cost_and_gradient_one_thread,
          this,
          &costs[t]
          )
        );
  }
  threads.join_all();

  // collect the results.
  cost.zeros();
  for (auto t = 0; t < nr_threads; ++ t) { cost.merge(costs[t]); }

  // back-propgrade saved gradient. It's risky because update grad_E.col
  // may introduce data racing.
  /*for (auto& fid: features) { queue2.push(fid); }

  for (auto t = 0; t < nr_threads; ++ t) {
    threads.create_thread(
        boost::bind(
          &NeuralNetworkClassifier::back_propagate_saved_one_thread,
          this
          )
        );
  }
  threads.join_all();*/
  for (auto& fid: features) {
    auto rank = encoder[fid];
    auto aid = fid / nr_feature_types;
    auto off = (fid % nr_feature_types)*embedding_size;

    cost.grad_W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) +=
      cost.grad_saved.col(rank) * E.col(aid).t();

    if (!fix_embeddings) {
      cost.grad_E.col(aid) += W1.submat(0, off,
          hidden_layer_size-1, off+ embedding_size-1).t() * cost.grad_saved.col(rank);
    }
  }

  cost.loss /= batch_size;
  cost.accuracy /= batch_size;

  // Add L2 regularization
  cost.loss += (lambda * .5 * (arma::dot(W1, W1)+ arma::dot(b1, b1)+ arma::dot(W2, W2)));
  if (!fix_embeddings) { cost.loss += (lambda * .5 * arma::dot(E, E)); }

  cost.grad_W1 += lambda * W1;
  cost.grad_b1 += lambda * b1;
  cost.grad_W2 += lambda * W2;
  if (!fix_embeddings) { cost.grad_E += lambda * E; }
}

void NeuralNetworkClassifier::gradient_check_one_dimension(
    const floatval_t& num_grad, const floatval_t& grad,
    const std::string& name, int x, int y) {
  auto reldiff = abs(num_grad - grad) / std::max(std::max(1, abs(num_grad)), (abs(grad)));
  if (reldiff > 1e-5) {
    if (y == -1) {
      _WARN << "Gradient check failed at " << name << "(" << x << ") "
        << "your gradient: " << grad << " numerical grad: " << num_grad;
    } else {
      _WARN << "Gradient check failed at " << name << "(" << x << "," << y << ") "
        << "your gradient: " << grad << " numerical grad: " << num_grad;
    }
  }
}

void NeuralNetworkClassifier::gradient_check(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end) {
  _INFO << "gradient check: checking W1 ...";
  floatval_t epsilon = 1e-6;
  floatval_t diff = 0.;
  for (int i = 0; i < W1.n_rows; ++ i) {
    for (int j = 0; j < W1.n_cols; ++ j) {
      W1(i, j) += epsilon;
      auto c = compute_cost(begin, end, dropout_histories);
      W1(i, j) -= 2 * epsilon;
      c -= compute_cost(begin, end, dropout_histories);
      auto num_grad = c / (2 * epsilon);
      gradient_check_one_dimension(num_grad, cost.grad_W1(i, j), "W1", i, j);
      diff += (num_grad - cost.grad_W1(i, j)) * (num_grad - cost.grad_W1(i, j));
      W1(i, j) += epsilon;
    }
  }

  _INFO << "gradient check: checking b1 ...";
  for (int i = 0; i < b1.n_rows; ++ i) {
    b1(i) += epsilon;
    auto c = compute_cost(begin, end, dropout_histories);
    b1(i) -= 2 * epsilon;
    c -= compute_cost(begin, end, dropout_histories);
    auto num_grad = c / (2 * epsilon);
    gradient_check_one_dimension(num_grad, cost.grad_b1(i), "b1", i, -1);
    diff += (num_grad - cost.grad_b1(i)) * (num_grad - cost.grad_b1(i));
    b1(i) += epsilon;
  }

  _INFO << "gradient check: checking W2 ...";
  for (int i = 0; i < W2.n_rows; ++ i) {
    for (int j = 0; j < W2.n_cols; ++ j) {
      W2(i, j) += epsilon;
      auto c = compute_cost(begin, end, dropout_histories);
      W2(i, j) -= 2 * epsilon;
      c -= compute_cost(begin, end, dropout_histories);
      auto num_grad = c / (2 * epsilon);
      gradient_check_one_dimension(num_grad, cost.grad_W2(i, j), "W2", i, j);
      diff += (num_grad - cost.grad_W2(i, j)) * (num_grad - cost.grad_W2(i, j));
      W2(i, j) -= epsilon;
    }
  }

  _INFO << "gradient check: total diff=" << diff;
}

void NeuralNetworkClassifier::save(std::ofstream& ofs) {
  E.save(ofs);
  W1.save(ofs);
  b1.save(ofs);
  W2.save(ofs);
  saved.save(ofs);
  boost::archive::text_oarchive oa(ofs);
  oa << precomputation_id_encoder;
}

void NeuralNetworkClassifier::load(std::ifstream& ifs) {
  E.load(ifs);
  W1.load(ifs);
  b1.load(ifs);
  W2.load(ifs);
  saved.load(ifs);
  boost::archive::text_iarchive ia(ifs);
  ia >> precomputation_id_encoder;
  hidden_layer_size = b1.n_rows;
  nr_feature_types = W1.n_cols / E.n_rows;
  nr_classes = W2.n_rows;
  embedding_size = E.n_rows;
  info(false);
}

void NeuralNetworkClassifier::info(bool detailed) {
  _INFO << "classifier: E(" << E.n_rows << "," << E.n_cols << ")";
  _INFO << "classifier: W1(" << W1.n_rows << "," << W1.n_cols << ")";
  _INFO << "classifier: b1(" << b1.n_rows << ")";
  _INFO << "classifier: W2(" << W2.n_rows << "," << W2.n_cols << ")";
  _INFO << "classifier: saved(" << saved.n_rows << "," << saved.n_cols << ")";
  _INFO << "classifier: precomputed size=" << precomputation_id_encoder.size();
  _INFO << "classifier: hidden layer size=" << hidden_layer_size;
  _INFO << "classifier: embedding size=" << embedding_size;
  _INFO << "classifier: number of classes=" << nr_classes;
  _INFO << "classifier: number of feature types=" << nr_feature_types;

  if (detailed) {
    _INFO << "classifier: alpha = " << ada_lr;
    _INFO << "classifier: eps = " << ada_eps;
    _INFO << "classifier: lambda = " << lambda;
    _INFO << "classifier: fix embedding = " << fix_embeddings;
  }
}

} //  namespace chen2014
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
