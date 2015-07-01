#include "app/depparser/nn/weiss15/model.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

Model::Model()
  : extractor(NULL), classifier(NULL), decoder(NULL), initialized(false) {}

void Model::initialize(Extractor* e,
    NeuralNetworkClassifier* c,
    Decoder* d) {
  if (initialized) {
    _WARN << "Model should not be initialized twice";
  }

  extractor = e;
  classifier = c;
  decoder = d;

  initialized = true;
}

void Model::zeros() {
  if (!initialized) {
    _WARN << "should initialize first.";
  }
  w = arma::zeros<arma::mat>(decoder->number_of_transitions(),
      classifier->n_perceptron());

  avg_w = w;
}

void Model::vectorize3(const State& stat, const Action& act,
    const floatval_t& scale, SparseVector3* sv) {
  unsigned l = decoder->transform(act);
  std::vector<int> attributes;
  extractor->get_features(stat, attributes);
  std::vector<floatval_t> input;
  classifier->perceptron(attributes, input);
  for (auto i = 0; i < input.size(); ++ i) {
    (*sv)[std::make_tuple(i, boost::hash_value<Action>(act), 0)] += input[i] * scale;
  }
}

floatval_t Model::score(const State& s, const Action& act, bool avg) const {
  unsigned l = decoder->transform(act);
  std::vector<int> attributes;
  extractor->get_features(s, attributes);
  std::vector<floatval_t> input;
  classifier->perceptron(attributes, input);
  return arma::sum((avg? avg_w.row(l): w.row(l)) * arma::vec(input));
}

void Model::batchly_score(const State& s,
    const std::vector<Action>& actions,
    bool avg, PackedScores<Action>& scores) const {
  std::vector<int> attributes;
  extractor->get_features(s, attributes);
  std::vector<floatval_t> input;
  classifier->perceptron(attributes, input);
  arma::vec output = (w * arma::vec(input));
  for (const auto& act: actions) {
    scores[act] = output(decoder->transform(act));
  }
}

void Model::update(const State& s,
    const Action& act, int ts, const floatval_t& f) {
  unsigned l = decoder->transform(act);
  std::vector<int> attributes;
  extractor->get_features(s, attributes);
  std::vector<floatval_t> input;
  classifier->perceptron(attributes, input);
  w.row(l) += f * arma::rowvec(input);
}

void Model::average() { avg_w += w; }

void Model::save(std::ofstream& ofs) {
  w.save(ofs);
  avg_w.save(ofs);
}

void Model::load(std::ifstream& ifs) {
  w.load(ifs);
  avg_w.load(ifs);
  _INFO << w.n_rows << "," << w.n_cols;
  _INFO << avg_w.n_rows << "," << avg_w.n_cols;
}

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
