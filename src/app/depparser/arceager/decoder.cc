#include "app/depparser/arceager/decoder.h"
#include "app/depparser/arceager/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

Decoder::Decoder(int n, int root, RootPosition position,
    int beam_size, bool avg, UpdateStrategy strategy, Weight* weight)
  : BasicDecoder(n, root, position),
  TransitionSystem<Action, State, Weight>(beam_size, avg, strategy, weight) {
}

void Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  int len = source.ref->size();

  if (source.buffer_empty()) {
    actions.push_back(ActionFactory::make_reduce());
  } else {
    if (source.buffer < len- 1) { //empty shift is illegal.
      actions.push_back(ActionFactory::make_shift());
    }

    if (!source.stack.empty()) {
      // Try to right arc
      if (source.top0 != 0) {
        for (deprel_t l = eg::TokenAlphabet::END+ 1; l < n_deprels; ++ l) {
          if (l != root_deprel) { actions.push_back(ActionFactory::make_right_arc(l)); }
        }
      } else {
        actions.push_back(ActionFactory::make_right_arc(root_deprel));
      }

      if (source.heads[source.top0] != -1) {
        actions.push_back(ActionFactory::make_reduce());
      } else {
        if (source.top0 != 0) {
          for (deprel_t l = eg::TokenAlphabet::END+ 1; l < n_deprels; ++ l) {
            if (l != root_deprel) { actions.push_back(ActionFactory::make_left_arc(l)); }
          }
        }
      }
    }
  }
}

void Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  int deprel;
  if (ActionUtils::is_shift(act)) {
    target->shift(source);
  } else if (ActionUtils::is_reduce(act)) {
    target->reduce(source);
  } else if (ActionUtils::is_left_arc(act, deprel)) {
    target->left_arc(source, deprel);
  } else if (ActionUtils::is_right_arc(act, deprel)) {
    target->right_arc(source, deprel);
  } else {
    _ERROR << "unknown transition";
    exit(1);
  }
  target->score = score;
}

bool Decoder::terminated() {
  // ban the terminate function, totally relay on max number of actions.
  return false;
}

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
