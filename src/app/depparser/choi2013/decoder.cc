#include "app/depparser/choi2013/decoder.h"
#include "app/depparser/choi2013/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

Decoder::Decoder(int nr, int root,
    int beam_size, bool avg, UpdateStrategy strategy, Weight* weight)
  : nr_deprels(nr), root_tag(root),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, avg, strategy, weight) {
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  if (source.is_complete()) {
    // When the parsing finished, the future action should be IDLE.
    actions.push_back(ActionFactory::make_idle());
  } else if (source.stack.size() > 1 && source.buffer_empty()) {
    // buffer is empty, only the REDUCE action is possible.
    actions.push_back(ActionFactory::make_reduce());
  } else if (source.reach_last_token()) {
    // reach the last token
    if (source.stack_empty()) {
      // stack is empty (deque should be empty two), the last action to shift dummy root
      // onto the stack.
      actions.push_back(ActionFactory::make_shift());
    } else {
      if (!source.stack_top_has_head()) {
        // stack top i is not linked a head, which means i should be linked a ROOT arc to
        // the dummy root by LEFT-ARC.
        for (auto l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          if (l == root_tag) { continue; }
          actions.push_back(ActionFactory::make_left_arc(l));
        }
      } else {
        actions.push_back(ActionFactory::make_reduce());
      }

      if (source.nr_empty_heads == 1) {
        for (auto l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          if (l == root_tag) { continue; }
          actions.push_back(ActionFactory::make_right_arc(l));
        }
      }
    }
  } else {
    if (source.stack_empty()) {
      // stack is empty, only SHIFT is possible.
      actions.push_back(ActionFactory::make_shift());
    } else {
      actions.push_back(ActionFactory::make_shift());
      actions.push_back(ActionFactory::make_no_pass());

      if (source.stack_top_has_head()) {
        actions.push_back(ActionFactory::make_reduce());
      }

      if (!source.stack_top_has_head() &&
          !source.is_descendant(source.top0, source.buffer)) {
        //
        for (auto l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          if (l == root_tag) { continue; }
          actions.push_back(ActionFactory::make_left_arc(l));
          actions.push_back(ActionFactory::make_left_pass(l));
        }
      }

      if (!source.buffer_front_has_head() &&
          !source.is_descendant(source.buffer, source.top0)) {
        for (auto l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          if (l == root_tag) { continue; }
          actions.push_back(ActionFactory::make_right_arc(l));
          actions.push_back(ActionFactory::make_right_pass(l));
        }
      }
    }
  }
}

void Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  int deprel;
  if (ActionUtils::is_idle(act))                    { target->idle(source);     }
  else if (ActionUtils::is_shift(act))              { target->shift(source);    }
  else if (ActionUtils::is_reduce(act))             { target->reduce(source);   }
  else if (ActionUtils::is_no_pass(act))            { target->no_pass(source);  }
  else if (ActionUtils::is_left_arc(act, deprel))   { target->left_arc(source, deprel);   }
  else if (ActionUtils::is_right_arc(act, deprel))  { target->right_arc(source, deprel);  }
  else if (ActionUtils::is_left_pass(act, deprel))  { target->left_pass(source, deprel);  }
  else if (ActionUtils::is_right_pass(act, deprel)) { target->right_pass(source, deprel); }
  else { _ERROR << "unknown transition: " << act; exit(1); }
  target->score = score;
}

bool Decoder::terminated() {
  if (guided) {
    return correct_state->is_complete();
  }

  int sz = lattice_size[step];
  bool all_terminated = (true && sz > 0);
  for (int i = 0; i < sz; ++ i) {
    const State* p = lattice_heads[step]+ i;
    if (!p->is_complete()) { all_terminated = false; }
  }
  return all_terminated;
}

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar
