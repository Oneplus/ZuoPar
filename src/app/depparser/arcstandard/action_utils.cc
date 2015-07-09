#include <boost/assert.hpp>
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/action_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

bool ActionUtils::is_shift(const Action& act) { return (act.name() == Action::kShift); }

bool ActionUtils::is_left_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kLeftArc) { deprel = act.rel(); return true;  }
  deprel = 0;
  return false;
}

bool ActionUtils::is_right_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kRightArc) { deprel = act.rel(); return true; }
  deprel = 0;
  return false;
}

void ActionUtils::get_oracle_actions(const std::vector<int>& heads,
    const std::vector<deprel_t>& deprels,
    std::vector<Action>& actions) {
  // The oracle finding algorithm for arcstandard is using a in-order tree
  // searching.
  int N = heads.size();
  int root = -1;
  tree_t tree(N);

  actions.clear();
  for (int i = 0; i < N; ++ i) {
    int head = heads[i];
    if (head == -1) {
      BOOST_ASSERT_MSG(root== -1, "error: there should be only one root.");
      root = i;
    } else {
      tree[head].push_back(i);
    }
  }

  get_oracle_actions_travel(root, heads, deprels, tree, actions);
}

void ActionUtils::get_oracle_actions(const Dependency& instance,
    std::vector<Action>& actions) {
  get_oracle_actions(instance.heads, instance.deprels, actions);
}

void ActionUtils::get_oracle_actions_travel(int root,
    const std::vector<int>& heads,
    const std::vector<int>& deprels,
    const tree_t& tree,
    std::vector<Action>& actions) {
  const std::vector<int>& children = tree[root];

  int i;
  for (i = 0; i < children.size() && children[i] < root; ++ i) {
    get_oracle_actions_travel(children[i], heads, deprels, tree, actions);
  }

  actions.push_back(ActionFactory::make_shift());

  for (int j = i; j < children.size(); ++ j) {
    int child = children[j];
    get_oracle_actions_travel(child, heads, deprels, tree, actions);
    actions.push_back(ActionFactory::make_right_arc(deprels[child]));
  }

  for (int j = i - 1; j >= 0; -- j) {
    int child = children[j];
    actions.push_back(ActionFactory::make_left_arc(deprels[child]));
  }
}

void ActionUtils::get_oracle_actions2(const Dependency& instance,
    std::vector<Action>& actions) {
  get_oracle_actions2(instance.heads, instance.deprels, actions);
}

void ActionUtils::get_oracle_actions2(const std::vector<int>& heads,
    const std::vector<int>& deprels,
    std::vector<Action>& actions) {
  actions.clear();
  auto len = heads.size();
  std::vector<int> sigma;
  int beta = 0;
  std::vector<int> output(len, -1);

  int step = 0;
  while (!(sigma.size() ==1 && beta == len)) {
    get_oracle_actions_onestep(heads, deprels, sigma, beta, output, actions);
    // if (step++ < 2* len-1) { _INFO << actions.back(); }
  }
  // exit(1);
}

void ActionUtils::get_oracle_actions_onestep(const std::vector<int>& heads,
    const std::vector<int>& deprels,
    std::vector<int>& sigma,
    int& beta,
    std::vector<int>& output,
    std::vector<Action>& actions) {
  int top0 = (sigma.size() > 0 ? sigma.back(): -1);
  int top1 = (sigma.size() > 1 ? sigma[sigma.size()- 2]: -1);

  bool all_descendents_reduced = true;
  if (top0 >= 0) {
    for (auto i = 0; i < heads.size(); ++ i) {
      if (heads[i] == top0 && output[i] != top0) {
        // _INFO << i << " " << output[i];
        all_descendents_reduced = false; break; }
    }
  }

  if (top1 >= 0 && heads[top1] == top0) {
    actions.push_back(ActionFactory::make_left_arc(deprels[top1]));
    output[top1] = top0;
    sigma.pop_back();
    sigma.back() = top0;
  } else if (top1 >= 0 && heads[top0] == top1 && all_descendents_reduced) {
    actions.push_back(ActionFactory::make_right_arc(deprels[top0]));
    output[top0] = top1;
    sigma.pop_back();
  } else if (beta < heads.size()) {
    actions.push_back(ActionFactory::make_shift());
    sigma.push_back(beta);
    ++ beta;
  }
}

} //  namespace arcstandard
} //  namespace dependencyparser
} //  namespace zuopar
