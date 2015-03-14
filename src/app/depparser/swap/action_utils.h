#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_ACTION_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_ACTION_UTILS_H__

#include <tuple>
#include <vector>
#include "types/dependency.h"
#include "app/depparser/swap/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

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

  //! Is the IDLE action.
  static bool is_idle(const Action& act);

  /**
   * Judge if the input action is a shift action.
   *
   *  @param[in]  act   The action.
   *  @return     bool  Return true on the action being a shift action, otherwise
   *                    false.
   */
  static bool is_shift(const Action& act);

  //! Is the SWAP action.
  static bool is_swap(const Action& act);

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
  //! The MPC calculate result type.
  typedef std::tuple<bool, int, int> mpc_result_t;

  // Calculate the depth of each node in the tree.
  static void get_oracle_actions_calculate_orders(int root,
      const tree_t& tree,
      std::vector<int>& orders,
      int& timestamp);

  static mpc_result_t get_oracle_actions_calculate_mpc(int root,
      const tree_t& tree,
      std::vector<int>& MPC);

  static void get_oracle_actions_onestep(const Dependency& dependency,
      const tree_t& tree,
      const std::vector<int>& orders,
      const std::vector<int>& MPC,
      std::vector<int>& sigma,
      std::vector<int>& beta,
      std::vector<int>& heads,
      std::vector<Action>& actions);
};

} //  end for swap
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_ACTION_UTILS_H__
