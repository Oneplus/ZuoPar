#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include "types/common.h"
#include "system/action/abstract_inexact_action.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

class Action: public AbstractInexactAction {
public:
  enum {
    kNone = 0,
    kO,
    kB,
    kI
  };

  Action(): AbstractInexactAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  tag   The name for the action.
   */
  Action(int name, tag_t tag): AbstractInexactAction(name, tag) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act);

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
};

class ActionFactory {
public:
  //! Make a O action.
  static Action make_O();

  //! Make a B action.
  static Action make_B(const tag_t& tag);

  //! Make a I action.
  static Action make_I(const tag_t& tag);
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_ACTION_H__
