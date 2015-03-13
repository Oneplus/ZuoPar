#include <boost/assert.hpp>
#include "app/depparser/choi2013/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

Action ActionFactory::make_idle() {
  return Action(Action::kIdle, 0);
}

Action ActionFactory::make_shift() {
  return Action(Action::kShift, 0);
}

Action ActionFactory::make_reduce() {
  return Action(Action::kReduce, 0);
}

Action ActionFactory::make_left_arc(const deprel_t& rel)  {
  return Action(Action::kLeftArc, rel);
}

Action ActionFactory::make_right_arc(const deprel_t& rel) {
  return Action(Action::kRightArc, rel);
}

Action ActionFactory::make_no_pass() {
  return Action(Action::kNoPass, 0);
}

Action ActionFactory::make_left_pass(const deprel_t& rel) {
  return Action(Action::kLeftPass, rel);
}

Action ActionFactory::make_right_pass(const deprel_t& rel) {
  return Action(Action::kRightPass, rel);
}

std::ostream& operator<<(std::ostream& os, const Action& act) {
  if (act.name() == Action::kShift)         { os << "SH"; }
  else if (act.name() == Action::kReduce)   { os << "RD"; }
  else if (act.name() == Action::kLeftArc)  { os << "LA#" << (act.rel()); }
  else if (act.name() == Action::kRightArc) { os << "RA#" << (act.rel()); }
  else if (act.name() == Action::kNoPass)   { os << "NP"; }
  else if (act.name() == Action::kLeftPass) { os << "LP#" << (act.rel()); }
  else if (act.name() == Action::kRightPass){ os << "RP#" << (act.rel()); }
  else if (act.name() == Action::kIdle)     { os << "ID"; }
  else if (act.name() == Action::kNone)     { os << "NO"; }
  else { os << "UNK:" << act.name() << " " << act.rel(); }
  return os;
}

} //  namespace choi2013
} //  namespace dependencyparser
} //  namespace zuopar
