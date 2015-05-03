#include "app/depparser/nn/classifier.h"
#include "utils/logging.h"
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

// using namespace arma;

Cost::Cost(): loss(0), accuracy(0) {}

void Cost::emplace(const floatval_t& _loss,
    const floatval_t& _accuracy,
    const arma::mat& _grad_W1,
    const arma::vec& _grad_b1,
    const arma::mat& _grad_W2,
    const arma::mat& _grad_E,
    const history_t& _histories) {
  loss = _loss;
  accuracy = _accuracy;
  grad_W1 = _grad_W1;
  grad_b1 = _grad_b1;
  grad_W2 = _grad_W2;
  grad_E = _grad_E;
  dropout_histories = _histories;
}

void Cost::merge(const Cost& other, bool debug) {
  loss += other.loss;
  accuracy += other.accuracy;
  grad_W1 += other.grad_W1;
  grad_b1 += other.grad_b1;
  grad_W2 += other.grad_W2;
  grad_E += other.grad_E;

  if (debug) {
    dropout_histories.insert(dropout_histories.end(),
        other.dropout_histories.begin(), other.dropout_histories.end());
  }
}

NeuralNetworkClassifier::NeuralNetworkClassifier()
  : initialized(false),
  dataset(nullptr),
  cursor(0),
  embedding_size(0),
  hidden_layer_size(0),
  nr_objects(0),
  nr_feature_types(0),
  nr_classes(0),
  debug(false) {
}

void NeuralNetworkClassifier::initialize(
    int nr_forms, int nr_postags, int nr_deprels,
    const LearnOption& opt,
    const std::vector< std::vector<floatval_t> >& embeddings,
    const Dataset* _dataset,
    const std::vector<int>& precomputed_features
    ) {
  if (initialized) {
    _ERROR << "classifier: weight should not be initialized twice!";
    return;
  }

  batch_size = opt.batch_size;
  nr_threads = opt.nr_threads;
  fix_embeddings = opt.fix_embeddings;
  // debug = opt.debug;
  dropout_probability = opt.dropout_probability;
  lambda = opt.lambda;
  ada_eps = opt.ada_eps;
  ada_alpha = opt.ada_alpha;

  // Initialize the parameter.
  nr_feature_types = opt.nr_feature_types;
  nr_objects = nr_forms + nr_postags + nr_deprels;
  nr_classes = nr_deprels*2-1; // nr_deprels*2+1-NIL

  embedding_size = opt.embedding_size;
  hidden_layer_size = opt.hidden_layer_size;

  // Initialize the network
  int nrows = hidden_layer_size;
  int ncols = embedding_size * nr_feature_types;
  W1 = (2. * arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows + ncols));
  b1 = (2. * arma::randu<arma::vec>(nrows)- 1.) * sqrt(6./ (nrows + ncols));

  nrows = nr_classes;  //
  ncols = hidden_layer_size;
  W2 = (2. * arma::randu<arma::mat>(nrows, ncols)- 1.) * sqrt(6./ (nrows+ ncols));

  // Initialized the embedding
  nrows = embedding_size;
  ncols= (nr_forms + nr_postags + nr_deprels);

  E = (2 * arma::randu<arma::mat>(nrows, ncols) - 1) * opt.init_range;
  for (auto& embedding: embeddings) {
    int id = embedding[0];
    for (unsigned j = 1; j < embedding.size(); ++ j) {
      E(j-1, id) = embedding[j];
    }
  }

  grad_W1 = arma::zeros<arma::mat>(W1.n_rows, W1.n_cols);
  grad_b1 = arma::zeros<arma::vec>(b1.n_rows);
  grad_W2 = arma::zeros<arma::mat>(W2.n_rows, W2.n_cols);
  grad_E = arma::zeros<arma::mat>(E.n_rows, E.n_cols);

  // Initialized the precomputed features
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  size_t rank = 0;
  for (auto& fid: precomputed_features) { encoder[fid] = rank ++; }
  saved.zeros(hidden_layer_size, encoder.size());
  grad_saved.zeros(hidden_layer_size, encoder.size());

  // Config the dataset.
  dataset = _dataset;

  //
  initialize_gradient_histories();

  initialized = true;

  info();
  _INFO << "classifier: size of batch = " << batch_size;
  _INFO << "classifier: alpha = " << ada_alpha;
  _INFO << "classifier: eps = " << ada_eps;
  _INFO << "classifier: lambda = " << lambda;
  _INFO << "classifier: fix embedding = " << fix_embeddings;
}

void NeuralNetworkClassifier::score(const std::vector<int>& attributes,
    std::vector<floatval_t>& retval) {
  const std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
  arma::vec hidden_layer = arma::zeros<arma::vec>(hidden_layer_size);

  for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
    auto& aid = attributes[i];
    auto fid = aid * nr_feature_types + i;
    auto rep = encoder.find(fid);
    if (rep != encoder.end()) {
      hidden_layer += saved.col(rep->second);
    } else {
      // W1[0:hidden_layer, off:off+embedding_size] * E[fid:]'
      hidden_layer += W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) *
        E.col(aid);
    }
  }

  hidden_layer = hidden_layer + b1;
  hidden_layer = hidden_layer % hidden_layer % hidden_layer;

  arma::vec output = W2 * hidden_layer;
  retval.resize(nr_classes, 0.);
  for (int i = 0; i < nr_classes; ++ i) { retval[i] = output(i); }
}

void NeuralNetworkClassifier::compute_ada_gradient_step() {
  if (!initialized) {
    _ERROR << "classifier: should not run the learning algorithm"
      " with un-initialized classifier.";
    return;
  }

  const std::vector<Sample>& entire_samples = dataset->samples;
  // Get a batch of samples.

  std::vector<Sample>::const_iterator begin = entire_samples.begin() + cursor;
  std::vector<Sample>::const_iterator end = entire_samples.end();
  if (cursor + batch_size < entire_samples.size()) {
    end = entire_samples.begin() + cursor + batch_size;
    cursor += batch_size;
  } else {
    cursor = 0;
  }

  // precomputing
  std::unordered_set<int> precomputed_features;
  get_precomputed_features(begin, end, precomputed_features);
  precomputing(precomputed_features);

  // calculate gradient
  grad_saved.zeros();
  compute_gradient(begin, end, end- begin);
  compute_saved_gradient(precomputed_features);

  //
  add_l2_regularization();
}

void NeuralNetworkClassifier::initialize_gradient_histories() {
  eg2E = arma::zeros<arma::mat>(E.n_rows, E.n_cols);
  eg2W1 = arma::zeros<arma::mat>(W1.n_rows, W1.n_cols);
  eg2W2 = arma::zeros<arma::mat>(W2.n_rows, W2.n_cols);
  eg2b1 = arma::zeros<arma::vec>(b1.n_rows);
}

void NeuralNetworkClassifier::take_ada_gradient_step() {
  eg2W1 += grad_W1 % grad_W1;
  W1 -= ada_alpha * grad_W1 / arma::sqrt(eg2W1 + ada_eps);

  eg2b1 += grad_b1 % grad_b1;
  b1 -= ada_alpha * grad_b1 / arma::sqrt(eg2b1 + ada_eps);

  eg2W2 += grad_W2 % grad_W2;
  W2 -= ada_alpha * grad_W2 / arma::sqrt(eg2W2 + ada_eps);

  if (!fix_embeddings) {
    eg2E += grad_E % grad_E;
    E -= ada_alpha * grad_E / arma::sqrt(eg2E + ada_eps);
  }
}

floatval_t NeuralNetworkClassifier::get_cost() { return loss; }

floatval_t NeuralNetworkClassifier::get_accuracy() { return accuracy; }

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
  for (auto fid: features) {
    auto rank = precomputation_id_encoder[fid];
    auto aid = fid / nr_feature_types;
    auto off = (fid % nr_feature_types)*embedding_size;
    saved.col(rank) =
      W1.submat(0, off, hidden_layer_size-1, off+embedding_size-1) * E.col(aid);
  }
  // _INFO << "classifier: precomputed " << features.size();
}

void NeuralNetworkClassifier::compute_gradient(
    std::vector<Sample>::const_iterator& begin,
    std::vector<Sample>::const_iterator& end,
    size_t batch_size) {
  const std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;

  grad_W1.zeros();
  grad_b1.zeros();
  grad_W2.zeros();
  grad_E.zeros();

  loss = 0; accuracy = 0;

  Cost::history_t histories;
  for (std::vector<Sample>::const_iterator sample = begin; sample != end; ++ sample) {
    auto& attributes = sample->attributes;
    auto& classes = sample->classes;

    arma::vec Y(classes);

    arma::uvec dropout_mask = arma::find(
        arma::randu<arma::vec>(hidden_layer_size) < dropout_probability );

    if (debug) {
      std::vector<int> history;
      for (auto i = 0; i < dropout_mask.n_rows; ++ i) { history.push_back(dropout_mask(i)); }
      histories.push_back(history);
    }

    arma::vec hidden_layer = arma::zeros<arma::vec>(dropout_mask.n_rows);

    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        hidden_layer += saved.submat(dropout_mask, _);
      } else {
        hidden_layer += (W1.submat(dropout_mask,
              arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size)) *
            E.col(aid)
           );
      }
    }

    hidden_layer += b1(dropout_mask);
    arma::vec cubic_hidden_layer = hidden_layer % hidden_layer % hidden_layer;

    // Mat(nr_classes, hidden_layer_size) * Vec(hidden_layer_size)
    // arma::vec output = W2.cols(dropout_mask) * cubic_hidden_layer(dropout_mask);
    arma::vec output = W2.cols(dropout_mask) * cubic_hidden_layer;
    int opt_class = -1, correct_class = -1;
    for (auto i = 0; i < nr_classes; ++ i) {
      if (opt_class < 0 || output(i) > output(opt_class)) { opt_class = i; }
      if (classes[i] == 1) { correct_class = i; }
    }

    arma::uvec classes_mask = arma::find(Y >= 0);
    floatval_t best = output(opt_class);
    output(classes_mask) = arma::exp(output(classes_mask) - best);
    floatval_t sum1 = output(correct_class);
    floatval_t sum2 = arma::sum(output(classes_mask));

    loss += (log(sum2) - log(sum1));
    if (classes[opt_class] == 1) { accuracy += 1; }

    // delta(classes_mask, 1)
    arma::vec delta =
      -(Y(classes_mask) - output(classes_mask) / sum2) / batch_size;

    grad_W2.submat(classes_mask, dropout_mask) += delta * cubic_hidden_layer.t();

    arma::vec grad_cubic_hidden_layer =
      W2.submat(classes_mask, dropout_mask).t() * delta;

    arma::vec grad_hidden_layer = 3 * grad_cubic_hidden_layer
                                  % hidden_layer
                                  % hidden_layer;

    grad_b1(dropout_mask) += grad_hidden_layer;

    for (auto i = 0, off = 0; i < attributes.size(); ++ i, off += embedding_size) {
      auto& aid = attributes[i];
      auto fid = aid * nr_feature_types + i;
      auto rep = encoder.find(fid);
      if (rep != encoder.end()) {
        arma::uvec _ = { rep->second };
        grad_saved.submat(dropout_mask, _) += grad_hidden_layer;
      } else {
        arma::uvec __ = arma::linspace<arma::uvec>(off, off+embedding_size-1, embedding_size);
        grad_W1.submat(dropout_mask, __) += grad_hidden_layer * E.col(aid).t();
        if (!fix_embeddings) {
          grad_E.col(aid) += W1.submat(dropout_mask, __).t() * grad_hidden_layer;
        }
      }
    }
  }

  loss /= batch_size;
  accuracy /= batch_size;
  // retval.emplace(loss, accuracy, grad_W1, grad_b1, grad_W2, grad_E, histories);
}

void NeuralNetworkClassifier::compute_saved_gradient(
    const std::unordered_set<int>& features) {
  std::unordered_map<int, size_t>& encoder = precomputation_id_encoder;
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
}

void NeuralNetworkClassifier::add_l2_regularization() {
  loss += (lambda * .5 * (
        arma::dot(W1, W1) +
        arma::dot(b1, b1) +
        arma::dot(W2, W2)));
  if (!fix_embeddings) { loss += (lambda * .5 * arma::dot(E, E)); }

  grad_W1 += lambda * W1;
  grad_b1 += lambda * b1;
  grad_W2 += lambda * W2;
  if (!fix_embeddings) { grad_E += lambda * E; }
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
  info();
}

void NeuralNetworkClassifier::info() {
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
}

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
