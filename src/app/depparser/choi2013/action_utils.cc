#include <boost/assert.hpp>
#include "utils/logging.h"
#include "utils/print/vector.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

bool ActionUtils::is_idle(const Action& act) {
  return (act.name() == Action::kIdle);
}

bool ActionUtils::is_shift(const Action& act) {
  return (act.name() == Action::kShift);
}

bool ActionUtils::is_reduce(const Action& act) {
  return (act.name() == Action::kReduce);
}

bool ActionUtils::is_left_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kLeftArc) { deprel = act.rel(); return true; }
  deprel = 0;
  return false;
}

bool ActionUtils::is_right_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kRightArc) { deprel = act.rel(); return true; }
  deprel = 0;
  return false;
}

bool ActionUtils::is_no_pass(const Action& act) {
  return (act.name() == Action::kNoPass);
}

bool ActionUtils::is_left_pass(const Action& act, int& deprel) {
  if (act.name() == Action::kLeftPass) { deprel = act.rel(); return true; }
  deprel = 0;
  return false;
}

bool ActionUtils::is_right_pass(const Action& act, int& deprel) {
  if (act.name() == Action::kRightPass) { deprel = act.rel(); return true; }
  deprel = 0;
  return false;
}

void ActionUtils::get_oracle_actions(const Dependency& instance,
    std::vector<Action>& actions) {
  const int len = instance.size();
  std::vector<int> sigma;
  std::vector<int> delta;
  int beta = 0;
  std::vector<int> heads(len, -1);
  while (!(sigma.size() == 1 && beta == len)) {
    //std::cout << sigma << ", " << delta << ", " << beta << " *";
    get_oracle_actions_onestep(instance, len, sigma, delta, beta, heads, actions);
    //std::cout << actions.back() << "* " << sigma << ", " << delta << ", "  << beta << std::endl;
  }
}

void ActionUtils::get_oracle_actions_onestep(const Dependency& instance,
    const int len,
    std::vector<int>& sigma,
    std::vector<int>& delta,
    int& beta,
    std::vector<int>& heads,
    std::vector<Action>& actions) {

  if (sigma.empty()) {
    actions.push_back(ActionFactory::make_shift());
    while (!delta.empty()) { sigma.push_back(delta.back()); delta.pop_back(); }
    sigma.push_back(beta);
    beta += 1;
    return;
  }

  int dir = -1;
  int top = sigma.back();

  if (instance.heads[top] == beta) { dir = 0; }
  if (beta < len && instance.heads[beta] == top) { dir = 1; }

  bool is_gold_reduce = true;
  bool is_gold_shift = (beta < len &&
                       (instance.heads[beta] == -1 || instance.heads[beta] >= top));

  for (auto c = beta+ 1; c < len; ++ c) {
    if (instance.heads[c] == top) { is_gold_reduce = false; break; }
  }

  for (auto c: sigma) {
    if (instance.heads[c] == beta) { is_gold_shift = false; break; }
  }

  if (0 == dir) {
    heads[top] = beta;
    if (is_gold_reduce) {
      actions.push_back(ActionFactory::make_left_arc(instance.deprels[top]));
      sigma.pop_back();
    } else {
      actions.push_back(ActionFactory::make_left_pass(instance.deprels[top]));
      delta.push_back(sigma.back()); sigma.pop_back();
    }
  } else if (1 == dir) {
    heads[beta] = top;
    if (is_gold_shift) {
      actions.push_back(ActionFactory::make_right_arc(instance.deprels[beta]));
      while (!delta.empty()) { sigma.push_back(delta.back()); delta.pop_back(); }
      sigma.push_back(beta);
      beta += 1;
    } else {
      actions.push_back(ActionFactory::make_right_pass(instance.deprels[beta]));
      delta.push_back(sigma.back()); sigma.pop_back();
    }
  } else {
    if (heads[top] != -1 && is_gold_reduce) {
      actions.push_back(ActionFactory::make_reduce());
      sigma.pop_back();
      return;
    }
    if (is_gold_shift) {
      actions.push_back(ActionFactory::make_shift());
      while (!delta.empty()) { sigma.push_back(delta.back()); delta.pop_back(); }
      sigma.push_back(beta);
      beta += 1;
    } else {
      actions.push_back(ActionFactory::make_no_pass());
      delta.push_back(sigma.back()); sigma.pop_back();
    }
  }
}

} //  end for namespace choi2013
} //  end for namespace depparser
} //  end for namespace zuopar
