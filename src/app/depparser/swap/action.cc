#include <boost/assert.hpp>
#include "app/depparser/swap/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

Action ActionFactory::make_idle() {
  return Action(Action::kIdle, 0);
}

Action ActionFactory::make_shift() {
  return Action(Action::kShift, 0);
}

Action ActionFactory::make_left_arc(const deprel_t& rel) {
  return Action(Action::kLeftArc, rel);
}

Action ActionFactory::make_right_arc(const deprel_t& rel) {
  return Action(Action::kRightArc, rel);
}

Action ActionFactory::make_swap() {
  return Action(Action::kSwap, 0);
}

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.name() == Action::kShift) { os << "SH"; }
  else if (act.name() == Action::kLeftArc)  { os << "LA#" << act.rel(); }
  else if (act.name() == Action::kRightArc) { os << "RA#" << act.rel(); }
  else if (act.name() == Action::kSwap)     { os << "SW"; }
  else if (act.name() == Action::kNone)     { os << "NO"; }
  else if (act.name() == Action::kIdle)     { os << "ID"; }
  else { os << "UNK:" << act.name() << " " << act.rel(); }
  return os;
}

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
