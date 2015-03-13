#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_UTILS_H__

#include <vector>
#include "types/dependency.h"
#include "app/depparser/choi2013/action.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

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

  //! Is idle action.
  static bool is_idle(const Action& act);

  //! Is shift action.
  static bool is_shift(const Action& act);

  //! Is reduce action.
  static bool is_reduce(const Action& act);

  //! Is right arc action.
  static bool is_right_arc(const Action& act, int& deprel);

  //! Is left arc action.
  static bool is_left_arc(const Action& act, int& deprel);

  //! Is no pass action.
  static bool is_no_pass(const Action& act);

  //! Is left pass action.
  static bool is_left_pass(const Action& act, int& deprel);

  //! Is right pass action.
  static bool is_right_pass(const Action& act, int& deprel);
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
      const int len,
      std::vector<int>& sigma,
      std::vector<int>& delta,
      int& beta,
      std::vector<int>& heads,
      std::vector<Action>& actions);
};

} //  end for choi2013
} //  end for dependencyparser
} //  end for zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_CHOI2013_ACTION_UTILS_H__
