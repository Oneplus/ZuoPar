#include <boost/assert.hpp>
#include "utils/logging.h"
#include "app/depparser/swap/action.h"
#include "app/depparser/swap/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

bool ActionUtils::is_idle(const Action& act) {
  return (act.name() == Action::kIdle);
}

bool ActionUtils::is_shift(const Action& act) {
  return (act.name() == Action::kShift);
}

bool ActionUtils::is_swap(const Action& act) {
  return (act.name() == Action::kSwap);
}

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
  int N = instance.size();
  tree_t tree(N);
  int root = -1;
  for (int i = 0; i < N; ++ i) {
    int head = instance.heads[i];
    if (head == -1) {
      BOOST_ASSERT_MSG(root== -1, "error: there should be only one root.");
      root = i;
    } else {
      tree[head].push_back(i);
    }
  }

  int timestamp = 0;
  std::vector<int> orders(N, -1);
  get_oracle_actions_calculate_orders(root, tree, orders, timestamp);

  std::vector<int> MPC(N, 0);
  get_oracle_actions_calculate_mpc(root, tree, MPC);

  std::vector<int> sigma;
  std::vector<int> beta;
  std::vector<int> heads(N, -1);
  for (auto i = N - 1; i >= 0; -- i) { beta.push_back(i); }
  while (!(sigma.size() == 1 && beta.empty())) {
    get_oracle_actions_onestep(instance, tree, orders, MPC, sigma, beta, heads, actions);
  }

  int max_len = N* N + N;
  while (actions.size() < max_len) { actions.push_back(ActionFactory::make_idle()); }
}

void ActionUtils::get_oracle_actions_onestep(const Dependency& instance,
    const tree_t& tree,
    const std::vector<int>& orders,
    const std::vector<int>& MPC,
    std::vector<int>& sigma,
    std::vector<int>& beta,
    std::vector<int>& heads,
    std::vector<Action>& actions) {
  if (sigma.size() < 2) {
    actions.push_back(ActionFactory::make_shift());
    sigma.push_back(beta.back()); beta.pop_back();
    return;
  }

  int top0 = sigma.back();
  int top1 = sigma[sigma.size() - 2];

  if (instance.heads[top1] == top0) {
    bool all_found = true;
    for (int c: tree[top1]) { if (heads[c] == -1) { all_found = false; } }
    if (all_found) {
      actions.push_back(ActionFactory::make_left_arc(instance.deprels[top1]));
      sigma.pop_back(); sigma.back() = top0; heads[top1] = top0;
      return;
    }
  }

  if (instance.heads[top0] == top1) {
    bool all_found = true;
    for (int c: tree[top0]) { if (heads[c] == -1) { all_found = false; } }
    if (all_found) {
      actions.push_back(ActionFactory::make_right_arc(instance.deprels[top0]));
      sigma.pop_back(); heads[top0] = top1;
      return;
    }
  }

  int k = beta.empty() ? -1 : beta.back();
  if ((orders[top0] < orders[top1]) &&
      (k == -1 || MPC[top0] != MPC[k])) {
    actions.push_back(ActionFactory::make_swap());
    sigma.pop_back(); sigma.back() = top0; beta.push_back(top1);
  } else {
    actions.push_back(ActionFactory::make_shift());
    sigma.push_back(beta.back()); beta.pop_back();
  }
}

void ActionUtils::get_oracle_actions_calculate_orders(int root,
    const tree_t& tree,
    std::vector<int>& orders,
    int& timestamp) {
  const std::vector<int>& children = tree[root];
  if (children.size() == 0) {
    orders[root] = timestamp;
    timestamp += 1;
    return;
  }

  int i;
  for (i = 0; i < children.size() && children[i] < root; ++ i) {
    int child = children[i];
    get_oracle_actions_calculate_orders(child, tree, orders, timestamp);
  }

  orders[root] = timestamp;
  timestamp += 1;

  for (; i < children.size(); ++ i) {
    int child = children[i];
    get_oracle_actions_calculate_orders(child, tree, orders, timestamp);
  }
}

ActionUtils::mpc_result_t ActionUtils::get_oracle_actions_calculate_mpc(int root,
    const tree_t& tree,
    std::vector<int>& MPC) {
  const std::vector<int>& children = tree[root];
  if (children.size() == 0) {
    MPC[root] = root;
    return std::make_tuple(true, root, root);
  }

  int left = root, right = root;
  bool overall = true;

  int pivot = -1;
  for (pivot = 0; pivot < children.size() && children[pivot] < root; ++ pivot);

  for (int i = pivot - 1; i >= 0; -- i) {
    int child = children[i];
    ActionUtils::mpc_result_t result =
      get_oracle_actions_calculate_mpc(child, tree, MPC);
    overall = overall && std::get<0>(result);
    if (std::get<0>(result) == true && std::get<2>(result) + 1 == left) {
      left = std::get<1>(result);
    } else {
      overall = false;
    }
  }

  for (int i = pivot; i < children.size(); ++ i) {
    int child = children[i];
    mpc_result_t result = get_oracle_actions_calculate_mpc(child, tree, MPC);
    overall = overall && std::get<0>(result);
    if (std::get<0>(result) == true && right + 1 == std::get<1>(result)) {
      right = std::get<2>(result);
    } else {
      overall = false;
    }
  }

  for (int i = left; i <= right; ++ i) { MPC[i] = root; }

  return std::make_tuple(overall, left, right);
}

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
