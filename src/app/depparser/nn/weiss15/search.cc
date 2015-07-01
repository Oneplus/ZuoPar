#include "app/depparser/nn/action_utils.h"
#include "app/depparser/nn/weiss15/search.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

StructuredDecoder::StructuredDecoder(int n, int root, int beam_size, 
    bool avg, UpdateStrategy us, Model* model)
  : n_rels(n), root_id(root),
  TransitionSystem<Action, State, Model>(beam_size, avg, us, model) {
}

void StructuredDecoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  if (!source.buffer_empty()) {
    actions.push_back(ActionFactory::make_shift());
  }
  if (source.stack_size() == 2) {
    if (source.buffer_empty()) {
      actions.push_back(ActionFactory::make_right_arc(root_id));
    }
  } else if (source.stack_size() > 2) {
    for (size_t l = 0; l < n_rels; ++ l) {
      if (l == root_id) { continue; }
      actions.push_back(ActionFactory::make_left_arc(l));
      actions.push_back(ActionFactory::make_right_arc(l));
    }
  }
}

void StructuredDecoder::transit(const State& source, const Action& act,
    const floatval_t& score, State* target) {
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
  target->score = score;
}

bool StructuredDecoder::terminated() { return false; }

void StructuredDecoder::set_avg(bool avg) { use_avg = avg; }

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
