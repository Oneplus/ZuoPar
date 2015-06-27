#include <boost/assert.hpp>
#include "app/depparser/arcstandard/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

Action ActionFactory::make_shift() { return Action(Action::kShift, 0); }

Action ActionFactory::make_left_arc(const deprel_t& rel) {
  return Action(Action::kLeftArc, rel);
}

Action ActionFactory::make_right_arc(const deprel_t& rel) {
  return Action(Action::kRightArc, rel);
}

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.name() == Action::kShift) {
    os << "SH";
  } else if (act.name() == Action::kLeftArc) {
    os << "LA~" << act.rel();
  } else if (act.name() == Action::kRightArc) {
    os << "RA~" << act.rel();
  } else if (act.name() == Action::kNone) {
    os << "NO";
  } else {
    BOOST_ASSERT_MSG(false, "unknown actions.");
  }
  return os;
}

} //  end for arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
