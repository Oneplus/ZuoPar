#ifndef __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_SERACH_H__
#define __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_SERACH_H__

#include "app/depparser/nn/state.h"
#include "app/depparser/nn/action.h"
#include "app/depparser/nn/weiss15/model.h"
#include "system/system.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

class StructuredDecoder: public TransitionSystem<Action, State, Model> {
private:
  int n_rels;
  int root_id;

public:
  StructuredDecoder(int n, int root, int beam_size, bool avg,
      UpdateStrategy us, Model* model);

  void get_possible_actions(const State& source, std::vector<Action>& actions);

  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

  bool terminated();

  void set_avg(bool avg);
};

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_WEISS2015_SERACH_H__
