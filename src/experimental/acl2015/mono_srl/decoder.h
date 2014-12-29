#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "experimental/acl2015/mono_srl/state.h"
#include "experimental/acl2015/mono_srl/action.h"
#include "experimental/acl2015/mono_srl/score_context.h"
#include "experimental/acl2015/mono_srl/action.h"
#include "experimental/acl2015/mono_srl/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

namespace eg = ZuoPar::Engine;

class Decoder
: public TransitionSystem<
  Action,
  State,
  ScoreContext,
  Weight
> {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr        The number of dependency relations.
   *  @param[in]  beam_size The size of beam.
   *  @param[in]  weight    The pointer to the weight.
   */
  Decoder(int nr, int beam_size, bool early_update, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

private:
  int nr_tags;
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace lexicalanalyzer
} //  namespace zuopar


#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_H__
