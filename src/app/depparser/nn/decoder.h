#ifndef __ZUOPAR_APP_DEPPARSER_NN_DECODER_H__
#define __ZUOPAR_APP_DEPPARSER_NN_DECODER_H__

#include "app/depparser/nn/state.h"
#include "app/depparser/nn/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

class Decoder {
private:
  size_t L;
public:
  //
  Decoder();

  void set_number_of_relations(size_t l);

  //
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //
  void transit(const State& source, const Action& act, State* target);

  //
  std::vector<int> transform(const std::vector<Action>& actions);

  //
  void transform(const std::vector<Action>& actions, std::vector<int>& classes);

  int transform(const Action& act);

  Action transform(int act);

  //
  size_t number_of_transitions() const;
};

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_DECODER_H__
