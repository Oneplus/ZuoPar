#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "types/common.h"
//#include "system/action/abstract_action.h"
#include "system/action/abstract_simple_action.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

class Action: public AbstractSimpleAction {
public:
  Action(): AbstractSimpleAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  Action(tag_t tag): AbstractSimpleAction(tag) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act);

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
};

class ActionCollection: public std::vector<Action> {
public:
  std::size_t seed;
  ActionCollection(): seed(0) {}

  void push_back(const Action& val) {
    boost::hash_combine(seed, val);
    std::vector<Action>::push_back(val);
  }

  friend std::size_t hash_value(const ActionCollection& a) {
    return a.seed;
  }

  friend std::ostream& operator<<(std::ostream& os, const ActionCollection& act);
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

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_ACTION_H__
