#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include "types/common.h"
#include "system/action/abstract_inexact_action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

class Action: public AbstractInexactAction {
public:
  enum {
    kNone = 0,  //! Placeholder for illegal action.
    kIdle,      //! The index of idle action.
    kShift,     //! The index of shift action.
    kLeftArc,   //! The index of arc left action.
    kRightArc,  //! The index of arc right action.
    kSwap       //! The index of swap action.
  };

  Action(): AbstractInexactAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  Action(int name, deprel_t rel): AbstractInexactAction(name, rel) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act);

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
};

class ActionFactory {
public:
  /**
   * Make a shift action.
   *
   *  @return Action  A shift action.
   */
  static Action make_shift();
  static Action make_idle();

  /**
   * Make a arc left action.
   *
   *  @param[in]  rel     The dependency relation.
   *  @return     Action  The arc left action.
   */
  static Action make_left_arc(const deprel_t& rel);

  /**
   * Make a arc right action.
   *
   *  @param[in]  rel     The dependency relation.
   *  @return     Action  The arc right action.
   */
  static Action make_right_arc(const deprel_t& rel);

  static Action make_swap();
};

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__
