#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/action_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

Decoder::Decoder(): L(0) {}

void Decoder::set_number_of_relations(size_t l) { L = l; }

void Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  if (!source.buffer_empty()) {
    actions.push_back(ActionFactory::make_shift());
  }

  if (source.stack_size() >= 2) {
    for (size_t l = 0; l < L; ++ l) {
      actions.push_back(ActionFactory::make_left_arc(l));
      actions.push_back(ActionFactory::make_right_arc(l));
    }
  }
}

void Decoder::transit(const State& source, const Action& act, State* target) {
  int deprel;
  if (ActionUtils::is_shift(act)) {
    target->shift(source);
  } else if (ActionUtils::is_left_arc(act, deprel)) {
    target->left_arc(source, deprel);
  } else if (ActionUtils::is_right_arc(act, deprel)) {
    target->right_arc(source, deprel);
  } else {
    _ERROR << "unknown transition in transit" << act;
  }
}

std::vector<int> Decoder::transform(const std::vector<Action>& actions) {
  std::vector<int> classes;
  for (auto act: actions) { classes.push_back( transform(act) ); }
  return classes;
}

void Decoder::transform(const std::vector<Action>& actions,
    std::vector<int>& classes) {
  classes.clear();
  for (auto act: actions) { classes.push_back( transform(act) ); }
}

int Decoder::transform(const Action& act) {
  int deprel;
  if (ActionUtils::is_shift(act)) { return 0; }
  else if (ActionUtils::is_left_arc(act, deprel))  { return 1 + deprel; }
  else if (ActionUtils::is_right_arc(act, deprel)) { return L + 1 + deprel; }
  else { _ERROR << "unknown transition in transform(Action&): " << act; }
  return -1;
}

Action Decoder::transform(int act) {
  if (act == 0) { return ActionFactory::make_shift(); }
  else if (act < 1+L) { return ActionFactory::make_left_arc(act- 1); }
  else if (act < 1+2*L) { return ActionFactory::make_right_arc(act- 1- L); }
  else { _ERROR << "unknown transition in transform(int&): " << act; }
  return Action();
}

size_t Decoder::number_of_transitions() const { return L*2+1; }

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
