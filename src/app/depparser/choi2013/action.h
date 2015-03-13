#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include "types/common.h"
#include "system/action/abstract_inexact_action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

class Action: public AbstractInexactAction {
public:
  enum {
    kNone = 0,  //! Placeholder for illegal action.
    kIdle,      //! The idle action for finished parsing.
    kShift,     //! The shift action;
    kReduce,    //! The reduce action;
    kLeftArc,   //! The arc left action;
    kRightArc,  //! The arc right action;
    kNoPass,    //! The no pass action;
    kLeftPass,  //! The left pass action;
    kRightPass  //! The right pass action;
  };

  Action(): AbstractInexactAction() {}
  Action(int name, deprel_t rel): AbstractInexactAction(name, rel) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act);

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
};

class ActionFactory {
public:
  //! Make a IDLE action.
  static Action make_idle();
  //! Make a shift action.
  static Action make_shift();
  //! Make a reduce action.
  static Action make_reduce();
  //! Make a left arc action.
  static Action make_left_arc(const deprel_t& rel);
  //! Make a right arc action.
  static Action make_right_arc(const deprel_t& rel);
  //! Make a no pass action.
  static Action make_no_pass();
  //! Make a left pass action.
  static Action make_left_pass(const deprel_t& rel);
  //! Make a right pass action.
  static Action make_right_pass(const deprel_t& rel);
};

} //  namespace choi2013
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_H__
