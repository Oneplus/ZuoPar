#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_DECODER_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "app/semchunker/mono/state.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/score_context.h"
#include "app/semchunker/mono/action.h"
#include "app/semchunker/mono/weight.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace eg = ZuoPar::Engine;

class Decoder
: public TransitionSystem<Action, State, ScoreContext, Weight> {
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

  bool terminated();
private:
  //! number of tags
  int nr_tags;
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_DECODER_H__
