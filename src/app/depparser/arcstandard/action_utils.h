#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_UTILS_H__

#include <vector>
#include "types/dependency.h"
#include "app/depparser/arcstandard/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class ActionUtils {
public:
  /**
   * Calculate the orcale action sequence in the arcstandard transition system.
   *
   *  @param[in]  instance  The input reference instance.
   *  @param[out] actions   The oracle transition sequence.
   */
  static void get_oracle_actions(const Dependency& instance,
      std::vector<Action>& actions);

  static void get_oracle_actions(const std::vector<int>& heads,
      const std::vector<deprel_t>& deprels,
      std::vector<Action>& actions);

  static void get_oracle_actions2(const Dependency& instance,
      std::vector<Action>& actions);

  static void get_oracle_actions2(const std::vector<int>& heads,
      const std::vector<deprel_t>& deprels,
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
   * Perform the mid-order tree travel to get the correct actions sequence.
   *
   *  @param[in]  root      The current root to visit.
   *  @param[in]  instance  The reference instance.
   *  @param[in]  tree      The converted tree.
   *  @param[out] actions   The actions.
   */
  static void get_oracle_actions_travel(int root,
      const std::vector<int>& heads,
      const std::vector<int>& deprels,
      const tree_t& tree,
      std::vector<Action>& actions);

  static void get_oracle_actions_onestep(
      const std::vector<int>& heads,
      const std::vector<int>& deprels,
      std::vector<int>& sigma,
      int& beta,
      std::vector<int>& output,
      std::vector<Action>& actions);
};

} //  end for arcstandard
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_UTILS_H__
