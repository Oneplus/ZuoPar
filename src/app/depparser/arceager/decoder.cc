#include "app/depparser/arceager/decoder.h"
#include "app/depparser/arceager/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

Decoder::Decoder(int n, int root, int position,
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
    if (!ActionUtils::is_reduce(source.last_action)) {
      if (source.stack.empty() || source.buffer < len - 1) {
        actions.push_back(ActionFactory::make_shift());
      }
    }

    if (!source.stack.empty()) {
      if (source.buffer < len - 1 || source.nr_empty_heads == 1) {
        for (deprel_t l = eg::TokenAlphabet::END+ 1; l < n_deprels; ++ l) {
          if (l != root_tag) {
            actions.push_back(ActionFactory::make_right_arc(l));
          }
        }
      }

      if (source.heads[source.top0] != -1) {
        actions.push_back(ActionFactory::make_reduce());
      } else {
        for (deprel_t l = eg::TokenAlphabet::END+ 1; l < n_deprels; ++ l) {
          if (l != root_tag) {
            actions.push_back(ActionFactory::make_left_arc(l));
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
