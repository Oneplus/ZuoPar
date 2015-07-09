#include <boost/assert.hpp>
#include "app/depparser/arceager/action.h"
#include "app/depparser/arceager/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

bool ActionUtils::is_shift(const Action& act) { return (act.name() == Action::kShift); }
bool ActionUtils::is_reduce(const Action& act) { return (act.name() == Action::kReduce); }

bool ActionUtils::is_left_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kLeftArc) {
    deprel = act.rel();
    return true;
  }
  deprel = 0;
  return false;
}

bool ActionUtils::is_right_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kRightArc) {
    deprel = act.rel();
    return true;
  }
  deprel = 0;
  return false;
}

void ActionUtils::get_oracle_actions(const Dependency& instance,
    std::vector<Action>& actions) {
  const int len = instance.size();
  std::vector<int> sigma;
  std::vector<int> heads(len, -1);
  int beta = 0;

  while (!(sigma.size() == 0 && beta == len)) {
    get_oracle_actions_onestep(instance, sigma, heads, beta, len, actions);
  }
}

void ActionUtils::get_oracle_actions_onestep(const Dependency& instance,
    std::vector<int>& sigma,
    std::vector<int>& heads,
    int& beta,
    const int len,
    std::vector<Action>& actions) {
  if (beta == len) {
    actions.push_back(ActionFactory::make_reduce());
    sigma.pop_back();
    return;
  }

  if (sigma.size() > 0) {
    int top = sigma.back();
    while (heads[top] != -1) { top = heads[top]; }

    if (instance.heads[top] == beta) {
      if (top == sigma.back()) {
        actions.push_back(ActionFactory::make_left_arc(instance.deprels[top]));
        heads[top] = beta;
      } else {
        actions.push_back(ActionFactory::make_reduce());
      }
      sigma.pop_back();
      return;
    }
  }

  if (instance.heads[beta] == -1 || instance.heads[beta] > beta) {
    sigma.push_back(beta);
    beta += 1;
    actions.push_back(ActionFactory::make_shift());
    return;
  } else {
    int top = sigma.back();
    if (instance.heads[beta] == top) {
      actions.push_back(ActionFactory::make_right_arc(instance.deprels[beta]));
      sigma.push_back(beta);
      heads[beta] = top;
      beta += 1;
    } else {
      actions.push_back(ActionFactory::make_reduce());
      sigma.pop_back();
    }
  }
}

} //  end for namespace arceager
} //  end for namespace depparser
} //  end for namespace zuopar
