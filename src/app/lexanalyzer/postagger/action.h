#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include "types/common.h"
#include "system/action/abstract_simple_action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

class Action: public AbstractSimpleAction {
public:
  Action() : AbstractSimpleAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name    The name for the action.
   *  @param[in]  postag  The postag of this action.
   */
  Action(postag_t postag) : AbstractSimpleAction(postag) {}

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
  static Action make(const postag_t& postag);
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_H__
