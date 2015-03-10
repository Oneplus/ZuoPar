#include <boost/assert.hpp>
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

bool
ActionUtils::is_shift(const Action& act) {
  return (act.name() == Action::kShift);
}

bool
ActionUtils::is_left_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kLeftArc) {
    deprel = act.rel();
    return true;
  }
  deprel = 0;
  return false;
}

bool
ActionUtils::is_right_arc(const Action& act, int& deprel) {
  if (act.name() == Action::kRightArc) {
    deprel = act.rel();
    return true;
  }
  deprel = 0;
  return false;
}

void
ActionUtils::get_oracle_actions(const Dependency& instance,
    std::vector<Action>& actions) {
  // The oracle finding algorithm for arcstandard is using a in-order tree
  // searching.
  int N = instance.size();
  int root = -1;
  tree_t tree(N);

  actions.clear();
  for (int i = 0; i < N; ++ i) {
    int head = instance.heads[i];
    if (head == -1) {
      BOOST_ASSERT_MSG(root== -1, "error: there should be only one root.");
      root = i;
    } else {
      tree[head].push_back(i);
    }
  }

  get_oracle_actions_travel(root, instance, tree, actions);
}

void
ActionUtils::get_oracle_actions_travel(int root,
    const Dependency& instance,
    const tree_t& tree,
    std::vector<Action>& actions) {
  const std::vector<int>& children = tree[root];

  int i;
  for (i = 0; i < children.size() && children[i] < root; ++ i) {
    get_oracle_actions_travel(children[i], instance, tree, actions);
  }

  actions.push_back(ActionFactory::make_shift());

  for (int j = i; j < children.size(); ++ j) {
    int child = children[j];
    get_oracle_actions_travel(child, instance, tree, actions);
    actions.push_back(ActionFactory::make_right_arc(instance.deprels[child]));
  }

  for (int j = i - 1; j >= 0; -- j) {
    int child = children[j];
    actions.push_back(ActionFactory::make_left_arc(instance.deprels[child]));
  }
}


}
}
} //  end for namespace zuopar
