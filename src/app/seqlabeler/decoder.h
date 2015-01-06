#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_DECODER_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_DECODER_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "app/seqlabeler/state.h"
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/score_context.h"
#include "app/seqlabeler/action.h"
#include "app/seqlabeler/weight.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;

class Decoder: public TransitionSystem<Action, State, ScoreContext, Weight> {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr        The number of dependency relations.
   *  @param[in]  trans     The transition.
   *  @param[in]  beam_size The size of beam.
   *  @param[in]  early_update
   *  @param[in]  weight    The pointer to the weight.
   */
  Decoder(int nr, const std::vector< std::vector<bool> >& trans_,
      int beam_size, UpdateStrategy strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<Action>& actions);

  //! Implement arc standard transition.
  void transit(const State& source, const Action& act, const floatval_t& score,
      State* target);

private:
  int nr_tags;
  const std::vector< std::vector<bool> >& trans;
};

} //  namespace sequencelabeler
} //  namespace zuopar


#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_DECODER_H__
