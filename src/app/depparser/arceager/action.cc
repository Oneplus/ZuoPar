#include <boost/assert.hpp>
#include "app/depparser/arceager/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

Action
ActionFactory::make_shift() {
  return Action(Action::kShift, 0);
}

Action
ActionFactory::make_reduce() {
  return Action(Action::kReduce, 0);
}

Action
ActionFactory::make_left_arc(const deprel_t& rel) {
  return Action(Action::kLeftArc, rel);
}

Action
ActionFactory::make_right_arc(const deprel_t& rel) {
  return Action(Action::kRightArc, rel);
}

} //  end for arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
