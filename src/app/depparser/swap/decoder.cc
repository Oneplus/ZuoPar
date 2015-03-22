#include "app/depparser/swap/decoder.h"
#include "app/depparser/swap/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

Decoder::Decoder(int nr, int root, int position,
    int beam_size, bool avg, UpdateStrategy strategy, Weight* weight)
  : nr_deprels(nr), root_tag(root), root_position(position),
  TransitionSystem<Action, State, ScoreContext, Weight>(beam_size, avg, strategy, weight) {
}

void Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  if (source.is_complete()) {
    // 
    actions.push_back(ActionFactory::make_idle());
    return;
  }
  
  if (!source.buffer_empty()) {
    actions.push_back(ActionFactory::make_shift());
  }
  if (source.stack_size() >= 2) {
    //
    for (deprel_t l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
      if (l != root_tag) { continue; }
      if (!(root_position == kLeft && source.top1 == 0)) {
        actions.push_back(ActionFactory::make_left_arc(l));
      }
      if (!(root_position == kRight && source.top0 == source.ref->size()- 1)) {
        actions.push_back(ActionFactory::make_right_arc(l));
      }
    }
    if (source.top1 < source.top0) {
      if (!(root_position == kLeft && source.top1 == 0) &&
          !(root_position == kRight && source.top0 == source.ref->size()- 1)) {
        actions.push_back(ActionFactory::make_swap());
      }
    }
  }
}

void Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  int deprel;
  if (ActionUtils::is_idle(act)) { target->idle(source); }
  else if (ActionUtils::is_shift(act)) { target->shift(source); }
  else if (ActionUtils::is_left_arc(act, deprel))  { target->left_arc(source, deprel); }
  else if (ActionUtils::is_right_arc(act, deprel)) { target->right_arc(source, deprel); }
  else if (ActionUtils::is_swap(act)) { target->swap(source); }
  else { _ERROR << "unknown transition"; exit(1); }
  target->score = score;
}

bool Decoder::terminated() {
  int sz = lattice_size[step];
  bool all_terminated = (true && sz > 0);
  for (int i = 0; i < sz; ++ i) {
    const State* p = lattice_heads[step]+ i;
    if (!p->is_complete()) { all_terminated = false; }
  }
  return all_terminated;
}

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
