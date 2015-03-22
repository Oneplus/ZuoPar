#ifndef __ZUOPAR_APP_LEXANALYZER_SEGMENTOR_ACTION_H__
#define __ZUOPAR_APP_LEXANALYZER_SEGMENTOR_ACTION_H__

#include <iostream>
#include "types/common.h"
#include "system/action/abstract_simple_action.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

class Action: public AbstractSimpleAction {
public:
  enum {
    kNone = 0,  //! The place holder
    kSplit,     //! The split action.
    kAppend     //! The append action.
  };

  Action() : AbstractSimpleAction() {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name    The name for the action.
   *  @param[in]  postag  The postag of this action.
   */
  Action(int name) : AbstractSimpleAction(name) {}

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act);

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
};

class ActionFactory {
public:
  static Action make_split();   //! Shorthand for Action(Action::kSplit);
  static Action make_append();  //! Shorthand for Action(Action::kAppend);
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_ACTION_H__
