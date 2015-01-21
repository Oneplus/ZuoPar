#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "types/common.h"
#include "system/action/abstract_simple_action.h"

namespace ZuoPar {
namespace SequenceLabeler {

class Action: public AbstractSimpleAction {
public:
  Action(): AbstractSimpleAction() {}

  Action(tag_t tag): AbstractSimpleAction(tag) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act) {
    os << "TAG~" << act.action_name;
    return os;
  }

  friend class ActionUtils;
};

class ActionFactory {
public:
  /**
   * Make a shift action.
   *
   *  @return Action  A shift action.
   */
  static Action make(const tag_t& tag);
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__
