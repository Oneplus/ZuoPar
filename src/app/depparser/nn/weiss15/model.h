#ifndef __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_MODEL_H__
#define __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_MODEL_H__

#include "app/depparser/nn/extractor.h"
#include "app/depparser/nn/state.h"
#include "app/depparser/nn/action.h"
#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/weiss15/classifier.h"
#include "types/internal/packed_scores.h"
#include "system/structure_online_learner.h"
#include "types/math/sparse_vector.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

class Model {
private:
  Extractor* extractor;
  NeuralNetworkClassifier* classifier;
  Decoder* decoder;

  bool initialized;
  arma::mat w;
  arma::mat avg_w;
public:
  Model();

  void zeros();

  void initialize(Extractor* extractor,
      NeuralNetworkClassifier* classifier,
      Decoder* decoder);

  void vectorize3(const State& stat, const Action& act,
      const floatval_t& scale, SparseVector3* sparse_vector);

  floatval_t score(const State& stat, const Action& act, bool avg) const;

  void batchly_score(const State& ctx, const std::vector<Action>& actions,
      bool avg, PackedScores<Action>& scores) const;

  void update(const State& stat, const Action& act, int ts, const floatval_t& s);

  void average();

  void save(std::ofstream& ifs);

  void load(std::ifstream& ofs);
};

struct Loss {
  floatval_t operator ()(const State* predict, const State* correct) {
    floatval_t retval = 0.;
    for (auto i = 0; i < predict->buffer; ++ i) {
      if (predict->heads[i] != correct->heads[i])           { retval += 2.; }
      else if (predict->deprels[i] != correct->deprels[i])  { retval += 1.; }
    }
    return retval;
  }
};

typedef TransitionStructureOnlineLearner<Action, State, Model, Loss> Learner;

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_MODEL_H__
