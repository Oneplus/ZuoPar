#include <boost/assert.hpp>
#include "experimental/hc_search/arceager/action.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.action_name == Action::kShift) {
    os << "SH";
  } else if (act.action_name == Action::kReduce) {
    os << "RD";
  } else if (act.action_name == Action::kLeftArc) {
    os << "LA~" << act.deprel;
  } else if (act.action_name == Action::kRightArc) {
    os << "RA~" << act.deprel;
  } else if (act.action_name == Action::kNone) {
    os << "NO";
  } else {
    BOOST_ASSERT_MSG(false, "unknown actions.");
  }
  return os;
}

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