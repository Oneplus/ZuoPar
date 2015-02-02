#include "experimental/hc_search/arceager/decoder.h"
#include "experimental/hc_search/arceager/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

Decoder::Decoder(int nr, int beam_size, bool avg, UpdateStrategy strategy, HeuristicWeight* weight)
  : nr_deprels(nr),
  TransitionSystem<
    Action,
    State,
    HeuristicScoreContext,
    HeuristicWeight
  >(beam_size, avg, strategy, weight) {
}

void
Decoder::get_possible_actions(const State& source,
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
        for (deprel_t l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          actions.push_back(ActionFactory::make_right_arc(l));
        }
      }

      if (source.heads[source.top0] != -1) {
        actions.push_back(ActionFactory::make_reduce());
      } else {
        for (deprel_t l = eg::TokenAlphabet::END+ 1; l < nr_deprels; ++ l) {
          actions.push_back(ActionFactory::make_left_arc(l));
        }
      }
    }
  }
}

void
Decoder::transit(const State& source, const Action& act, const floatval_t& score,
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

void
Decoder::get_results_in_beam(std::vector<const State*>& results,
    int round) {
  _INFO << round << " " << lattice_heads.size();
  for (int i = 0; i < lattice_size[round]; ++ i) {
    results.push_back(lattice_heads[round]+ i);
  }
}

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar
