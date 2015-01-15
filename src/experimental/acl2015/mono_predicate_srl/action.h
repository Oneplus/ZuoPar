#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/singleton.hpp>
#include "types/common.h"
#include "system/action/abstract_action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

class Action: public AbstractAction {
public:
  enum {
    kNone,
    kO, //! The index of O action.
    kB, //! The index of B action.
    kI, //! The index of I action.
  };

  Action() : AbstractAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  Action(int name, tag_t tag) : AbstractAction(name, tag) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act) {
    if (act.action_name == kNone) {
      os << "NONE";
    } else if (act.action_name == kO) {
      os << "O";
    } else if (act.action_name == kB) {
      os << "B~" << act.deprel;
    } else if (act.action_name == kI) {
      os << "I~" << act.deprel;
    } else {
      BOOST_ASSERT_MSG(false, "unknown actions.");
    }
    return os;
  }

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

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_ACTION_H__
