#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_DECODER_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "experimental/acl2015/mono_predicate_srl/state.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"
#include "experimental/acl2015/mono_predicate_srl/score_context.h"
#include "experimental/acl2015/mono_predicate_srl/action.h"
#include "experimental/acl2015/mono_predicate_srl/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

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
  Decoder(int nr, int beam_size, bool avg, UpdateStrategy update_strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

private:
  //! number of tags
  int nr_tags;
};

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace lexicalanalyzer
} //  namespace zuopar


#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_DECODER_H__
