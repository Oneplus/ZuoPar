#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__

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
namespace SequenceLabeler {

class Action: public AbstractAction {
public:
  enum {
    kShift,     //! The index of shift action.
  };

  Action() : AbstractAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  Action(int name, postag_t postag) : AbstractAction(name, postag) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act) {
    if (act.action_name == kShift) {
      os << "SH~" << act.deprel;
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
  /**
   * Make a shift action.
   *
   *  @return Action  A shift action.
   */
  static Action make_shift(const postag_t& postag);
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_ACTION_H__
