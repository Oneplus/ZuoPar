#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_ACTION_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_ACTION_UTILS_H__

#include <vector>
#include "types/dependency.h"
#include "experimental/hc_search/arceager/action.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the arceager transition system.
   *
   *  @param[in]  instance  The input reference instance.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_oracle_actions(const Dependency& instance,
      std::vector<Action>& actions);

  /**
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_shift(const Action& act);

  /**
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_reduce(const Action& act);

  /**
   * Judge if the input action is a RightArc action.
   *
   *  @param[in]  act     The action.
   *  @param[out] deprel  The dependency relation. If input is not ArcLeft, set it
   *                      to zero.
   *  @return     bool    Return true on the action being a shift action,
   *                      otherwise false.
   */
  static bool is_right_arc(const Action& act, int& deprel);

  /**
   * Judge if the input action is a LeftArc action.
   *
   *  @param[in]  act     The action.
   *  @param[out] deprel  The dependency relation. If input is not ArcLeft, set it
   *                      to zero.
   *  @return     bool    Return true on the action being a shift action,
   *                      otherwise false.
   */
  static bool is_left_arc(const Action& act, int& deprel);
private:
  //! The tree type.
  typedef std::vector<std::vector<int> > tree_t;

  /**
   * Perform the linear oracle finding algorithm on one step.
   *
   *  @param[in]  instance  The reference instance.
   *  @param[in]  sigma     The stack.
   *  @param[in]  heads     The linked heads.
   *  @param[in]  beta      The buffer.
   *  @param[in]  len       The length of the input sentence.
   *  @param[out] actions   The actions.
   */
  static void get_oracle_actions_onestep(const Dependency& instance,
      std::vector<int>& sigma,
      std::vector<int>& heads,
      int& beta,
      const int len,
      std::vector<Action>& actions);
};

} //  end for arceager
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_ACTION_UTILS_H__
