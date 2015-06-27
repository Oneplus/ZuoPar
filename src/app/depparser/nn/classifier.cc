#include "app/depparser/nn/classifier.h"
#include "utils/logging.h"
#include <algorithm>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

// using namespace arma;

NeuralNetworkClassifier::NeuralNetworkClassifier()
  : initialized(false),
  embedding_size(0),
  hidden_layer_size(0),
  nr_objects(0),
  nr_feature_types(0),
  nr_classes(0),
  iter(0),
  activation(kCube) {
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

  batch_size = opt.batch_size;
  fix_embeddings = opt.fix_embeddings;
  dropout_probability = opt.dropout_probability;
  activation = kCube; if (opt.activation == "relu") { activation = kReLU; }
  lambda = opt.lambda;

  ada_eps = opt.ada_eps;
  ada_lr = opt.ada_lr;

  /*momentum_mu = opt.momentum_mu;
  momentum_stepsize = opt.momentum_stepsize;
  momentum_gamma = opt.momentum_gamma;
  momentum_lr = opt.momentum_lr;*/

  // Initialize the parameter.
  nr_feature_types = _nr_feature_types;
  nr_objects = _nr_objects;
  nr_classes = _nr_classes; // nr_deprels*2+1-NIL

  embedding_size = opt.embedding_size;
  hidden_layer_size = opt.hidden_layer_size;

  // Initialize the network
  // For the ReLU activation function, initialization follows Weiss (2014)
  int nrows = hidden_layer_size;
  int ncols = embedding_size * nr_feature_types;

  if (activation == kCube) {
    W1 = (2.* arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows+ ncols));
    b1 = arma::zeros<arma::vec>(nrows);
  } else if (activation == kReLU) {
    W1 = arma::randn<arma::mat>(nrows, ncols) * 0.01;
    b1 = arma::ones<arma::vec>(nrows) * 0.2;
  }

  if (activation == kReLU) {
    nrows = hidden_layer_size;
    ncols = hidden_layer_size;
    W10 = arma::randn<arma::mat>(nrows, ncols) * 0.01;
    b10 = arma::ones<arma::vec>(nrows) * 0.2;
  }

  nrows = _nr_classes;  //
  ncols = hidden_layer_size;

  if (activation == kCube) {
    W2 = (2.* arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows+ ncols));
  } else if (activation == kReLU) {
    W2 = arma::randn<arma::mat>(nrows, ncols) * 0.01;
  }

  // Initialized the embedding
  nrows = embedding_size;
  ncols= _nr_objects;

  if (activation == kCube) {
    E = (2.* arma::randu<arma::mat>(nrows, ncols) - 1.) * opt.init_range;
  } else if (activation == kReLU) {
    E = arma::randn<arma::mat>(nrows, ncols) * 0.01;
  }
  for (auto& embedding: embeddings) {
    int id = embedding[0];
    for (unsigned j = 1; j < embedding.size(); ++ j) {
      E(j-1, id) = embedding[j];
    }
  }

  grad_W1 = arma::zeros<arma::mat>(W1.n_rows, W1.n_cols);
  grad_b1 = arma::zeros<arma::vec>(b1.n_rows);
  if (activation == kReLU) {
    grad_W10 = arma::zeros<arma::mat>(W10.n_rows, W10.n_cols);
    grad_b10 = arma::zeros<arma::vec>(b10.n_rows);
  }
  grad_W2 = arma::zeros<arma::mat>(W2.n_rows, W2.n_cols);
  grad_E = arma::zeros<arma::mat>(E.n_rows, E.n_cols);

  // Initialized the precomputed features
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  size_t rank = 0;
  for (auto& fid: precomputed_features) { encoder[fid] = rank ++; }
  saved.zeros(hidden_layer_size, encoder.size());
  grad_saved.zeros(hidden_layer_size, encoder.size());

  initialize_gradient_histories();
  initialized = true;

  info();
  _INFO << "classifier: size of batch = " << batch_size;
  _INFO << "classifier: alpha = " << ada_lr;
  _INFO << "classifier: eps = " << ada_eps;
  _INFO << "classifier: lambda = " << lambda;
  _INFO << "classifier: fix embedding = " << fix_embeddings;
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
  if (activation == kCube) {
    hidden= hidden % hidden % hidden;
  } else if (activation == kReLU) {
    hidden= arma::clamp(hidden, 0, hidden.max());
    hidden= W10 * hidden+ b10;
    hidden= arma::clamp(hidden, 0, hidden.max());
  }

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
  if (activation == kReLU) {
    eg2W10 = arma::zeros<arma::mat>(W10.n_rows, W10.n_cols);
    eg2b10 = arma::zeros<arma::vec>(b10.n_rows);
  }
  eg2W2 = arma::zeros<arma::mat>(W2.n_rows, W2.n_cols);
}

void NeuralNetworkClassifier::take_momentum_asgd_step() {
  eg2W1 = momentum_mu * eg2W1 - momentum_lr * grad_W1; W1 += eg2W1;
  eg2b1 = momentum_mu * eg2b1 - momentum_lr * grad_b1; b1 += eg2b1;

  if (activation == kReLU) {
    eg2W10 = momentum_mu * eg2W10 - momentum_lr * grad_W10; W10 += eg2W10;
    eg2b10 = momentum_mu * eg2b10 - momentum_lr * grad_b10; b10 += eg2b10;
  }

  eg2W2 = momentum_mu * eg2W2 - momentum_lr * grad_W2; W2 += eg2W2;
  if (!fix_embeddings) {
    eg2E = momentum_mu * eg2E - momentum_lr * grad_E; E += eg2E;
  }

  ++ iter;
  if (iter % momentum_stepsize == 0) { momentum_lr *= momentum_gamma; }
}

void NeuralNetworkClassifier::take_adagrad_step() {
  eg2W1 += grad_W1 % grad_W1; W1 -= ada_lr * (grad_W1 / arma::sqrt(eg2W1 + ada_eps));
  eg2b1 += grad_b1 % grad_b1; b1 -= ada_lr * (grad_b1 / arma::sqrt(eg2b1 + ada_eps));

  if (activation == kReLU) {
    eg2W10 += grad_W10 % grad_W10; W10 -= ada_lr * (grad_W10 / arma::sqrt(eg2W10 + ada_eps));
    eg2b10 += grad_b10 % grad_b10; b10 -= ada_lr * (grad_b10 / arma::sqrt(eg2b10 + ada_eps));
  }

  eg2W2 += grad_W2 % grad_W2; W2 -= ada_lr * (grad_W2 / arma::sqrt(eg2W2 + ada_eps));

  if (!fix_embeddings) {
    eg2E += grad_E % grad_E; E -= ada_lr * (grad_E / arma::sqrt(eg2E + ada_eps));
  }

  ++ iter;
}

floatval_t NeuralNetworkClassifier::get_cost() const { return loss; }

floatval_t NeuralNetworkClassifier::get_accuracy() const { return accuracy; }

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
  size_t batch_size = end - begin;
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

    if (activation == kCube) {
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
    } else if (activation == kReLU) {
      floatval_t upper;
      upper = lin_hidden.max(); if (upper < 0) { upper = 0.; }
      arma::vec hidden1 = arma::clamp(lin_hidden, 0, upper);
      arma::vec lin_hidden2 = W10(dropout_mask, dropout_mask) * hidden1 + b10(dropout_mask);
      upper = lin_hidden2.max(); if (upper < 0) { upper = 0.; }
      arma::vec hidden2 = arma::clamp(lin_hidden2, 0, upper);
      arma::vec output = W2.cols(dropout_mask) * hidden2;

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
  }

  retval /= batch_size;

  retval += (lambda * .5 * (arma::dot(W1, W1)+ arma::dot(b1, b1)+ arma::dot(W2, W2)));
  if (activation == kReLU) {
    retval += (lambda * .5 * (arma::dot(W10, W10) + arma::dot(b10, b10)));
  }
  if (!fix_embeddings) {
    retval += (lambda * .5 * arma::dot(E, E));
  }

  return retval;
}


void NeuralNetworkClassifier::compute_cost_and_gradient(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end,
    const std::unordered_set<int>& features,
    bool debug) {
  size_t batch_size = end - begin;
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  grad_W1.zeros();
  grad_b1.zeros();
  grad_W2.zeros();
  grad_E.zeros();
  if (activation == kReLU) {
    grad_W10.zeros();
    grad_b10.zeros();
  }
  grad_saved.zeros();

  loss = 0; accuracy = 0;
  if (debug) { dropout_histories.clear(); }

  for (std::vector<Sample>::const_iterator sample = begin; sample != end; ++ sample) {
    const auto& attributes = sample->attributes;
    const auto& classes = sample->classes;

    arma::vec Y(classes);

    arma::uvec dropout_mask = arma::find(
        arma::randu<arma::vec>(hidden_layer_size) > dropout_probability );

    if (debug) { dropout_histories.push_back( dropout_mask ); }

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

    if (activation == kCube) {
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

      loss += (log(sum2) - log(sum1));
      if (classes[opt_class] == 1) { accuracy += 1; }

      // delta(3)
      arma::vec delta = -(Y(classes_mask) - output(classes_mask) / sum2) / batch_size;
      // gradW(3)
      grad_W2.submat(classes_mask, dropout_mask) += delta * hidden.t();
      // delta(2)
      grad_lin_hidden =
        3* (W2.submat(classes_mask, dropout_mask).t() * delta) % lin_hidden % lin_hidden;

    } else if (activation == kReLU) {
      floatval_t upper;
      upper = lin_hidden.max(); if (upper < 0) { upper = 0.; }
      arma::vec hidden1 = arma::clamp(lin_hidden, 0, upper);
      arma::vec lin_hidden2 = W10(dropout_mask, dropout_mask) * hidden1 + b10(dropout_mask);
      upper = lin_hidden2.max(); if (upper < 0) { upper = 0.; }
      arma::vec hidden2 = arma::clamp(lin_hidden2, 0, upper);
      arma::vec output = W2.cols(dropout_mask) * hidden2;

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

      loss += (log(sum2) - log(sum1));
      if (classes[opt_class] == 1) { accuracy += 1; }

      // delta(4)
      arma::vec delta = -(Y(classes_mask) - output(classes_mask) / sum2) / batch_size;
      // gradW(3)
      grad_W2.submat(classes_mask, dropout_mask) += delta * hidden2.t();
      // delta(3)
      arma::vec grad_lin_hidden2 =
        (W2.submat(classes_mask, dropout_mask).t() * delta) % arma::sign(hidden2);
      // gradW(2)
      grad_W10.submat(dropout_mask, dropout_mask) += grad_lin_hidden2 * hidden1.t();
      grad_b10(dropout_mask) += grad_lin_hidden2;
      // delta(2)
      grad_lin_hidden =
        (W10.submat(dropout_mask, dropout_mask).t() * grad_lin_hidden2) % arma::sign(hidden1);
    }

    grad_b1(dropout_mask) += grad_lin_hidden;

    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        grad_saved.submat(dropout_mask, _) += grad_lin_hidden;
      } else {
        arma::uvec __ = arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size);
        grad_W1.submat(dropout_mask, __) += grad_lin_hidden * E.col(aid).t();
        if (!fix_embeddings) {
          grad_E.col(aid) += W1.submat(dropout_mask, __).t() * grad_lin_hidden;
        }
      }
    }
  }

  // back-propgrade saved gradient.
  for (auto& fid: features) {
    auto rank = encoder[fid];
    auto aid = fid / nr_feature_types;
    auto off = (fid % nr_feature_types)*embedding_size;

    grad_W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) +=
      grad_saved.col(rank) * E.col(aid).t();

    if (!fix_embeddings) {
      grad_E.col(aid) +=
        W1.submat(0, off, hidden_layer_size-1, off+ embedding_size-1).t()
        * grad_saved.col(rank);
    }
  }

  loss /= batch_size;
  accuracy /= batch_size;

  // Add L2 regularization
  loss += (lambda * .5 * (arma::dot(W1, W1)+ arma::dot(b1, b1)+ arma::dot(W2, W2)));
  if (activation == kReLU) {
    loss += (lambda * .5 * (arma::dot(W10, W10) + arma::dot(b10, b10)));
  }
  if (!fix_embeddings) {
    loss += (lambda * .5 * arma::dot(E, E));
  }

  grad_W1 += lambda * W1;
  grad_b1 += lambda * b1;
  grad_W2 += lambda * W2;
  if (activation == kReLU) {
    grad_W10 += lambda * W10;
    grad_b10 += lambda * b10;
  }
  if (!fix_embeddings) { grad_E += lambda * E; }
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
      auto cost = compute_cost(begin, end, dropout_histories);
      W1(i, j) -= 2 * epsilon;
      cost -= compute_cost(begin, end, dropout_histories);
      auto num_grad = cost / (2 * epsilon);
      gradient_check_one_dimension(num_grad, grad_W1(i, j), "W1", i, j);
      diff += (num_grad - grad_W1(i, j)) * (num_grad - grad_W1(i, j));
      W1(i, j) += epsilon;
    }
  }

  _INFO << "gradient check: checking b1 ...";
  for (int i = 0; i < b1.n_rows; ++ i) {
    b1(i) += epsilon;
    auto cost = compute_cost(begin, end, dropout_histories);
    b1(i) -= 2 * epsilon;
    cost -= compute_cost(begin, end, dropout_histories);
    auto num_grad = cost / (2 * epsilon);
    gradient_check_one_dimension(num_grad, grad_b1(i), "b1", i, -1);
    diff += (num_grad - grad_b1(i)) * (num_grad - grad_b1(i));
    b1(i) += epsilon;
  }

  if (activation == kReLU) {
    _INFO << "gradient check: checking W1' ...";
    for (int i = 0; i < W1.n_rows; ++ i) {
      for (int j = 0; j < W1.n_cols; ++ j) {
        W10(i, j) += epsilon;
        auto cost = compute_cost(begin, end, dropout_histories);
        W10(i, j) -= 2 * epsilon;
        cost -= compute_cost(begin, end, dropout_histories);
        auto num_grad = cost / (2 * epsilon);
        gradient_check_one_dimension(num_grad, grad_W10(i, j), "W1", i, j);
        diff += (num_grad - grad_W10(i, j)) * (num_grad - grad_W10(i, j));
        W10(i, j) += epsilon;
      }
    }

    _INFO << "gradient check: checking b1' ...";
    for (int i = 0; i < b1.n_rows; ++ i) {
      b10(i) += epsilon;
      auto cost = compute_cost(begin, end, dropout_histories);
      b10(i) -= 2 * epsilon;
      cost -= compute_cost(begin, end, dropout_histories);
      auto num_grad = cost / (2 * epsilon);
      gradient_check_one_dimension(num_grad, grad_b10(i), "b1", i, -1);
      diff += (num_grad - grad_b10(i)) * (num_grad - grad_b10(i));
      b10(i) += epsilon;
    }
  }

  _INFO << "gradient check: checking W2 ...";
  for (int i = 0; i < W2.n_rows; ++ i) {
    for (int j = 0; j < W2.n_cols; ++ j) {
      W2(i, j) += epsilon;
      auto cost = compute_cost(begin, end, dropout_histories);
      W2(i, j) -= 2 * epsilon;
      cost -= compute_cost(begin, end, dropout_histories);
      auto num_grad = cost / (2 * epsilon);
      gradient_check_one_dimension(num_grad, grad_W2(i, j), "W2", i, j);
      diff += (num_grad - grad_W2(i, j)) * (num_grad - grad_W2(i, j));
      W2(i, j) -= epsilon;
    }
  }

  _INFO << "gradient check: total diff=" << diff;
}

void NeuralNetworkClassifier::save(std::ofstream& ofs) {
  E.save(ofs);
  W1.save(ofs);
  b1.save(ofs);
  W10.save(ofs);
  b10.save(ofs);
  W2.save(ofs);
  saved.save(ofs);
  boost::archive::text_oarchive oa(ofs);
  oa << activation;
  oa << precomputation_id_encoder;
}

void NeuralNetworkClassifier::load(std::ifstream& ifs) {
  E.load(ifs);
  W1.load(ifs);
  b1.load(ifs);
  W10.load(ifs);
  b10.load(ifs);
  W2.load(ifs);
  saved.load(ifs);
  boost::archive::text_iarchive ia(ifs);
  ia >> activation;
  ia >> precomputation_id_encoder;
  hidden_layer_size = b1.n_rows;
  nr_feature_types = W1.n_cols / E.n_rows;
  nr_classes = W2.n_rows;
  embedding_size = E.n_rows;
  info();
}

void NeuralNetworkClassifier::info() {
  _INFO << "classifier: E(" << E.n_rows << "," << E.n_cols << ")";
  _INFO << "classifier: W1(" << W1.n_rows << "," << W1.n_cols << ")";
  _INFO << "classifier: b1(" << b1.n_rows << ")";
  if (activation == kReLU) {
    _INFO << "classifier: W1'(" << W10.n_rows << "," << W10.n_cols << ")";
    _INFO << "classifier: b1'(" << b10.n_rows << ")";
  }
  _INFO << "classifier: W2(" << W2.n_rows << "," << W2.n_cols << ")";
  _INFO << "classifier: saved(" << saved.n_rows << "," << saved.n_cols << ")";
  _INFO << "classifier: precomputed size=" << precomputation_id_encoder.size();
  _INFO << "classifier: hidden layer size=" << hidden_layer_size;
  _INFO << "classifier: embedding size=" << embedding_size;
  _INFO << "classifier: number of classes=" << nr_classes;
  _INFO << "classifier: number of feature types=" << nr_feature_types;
}

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
