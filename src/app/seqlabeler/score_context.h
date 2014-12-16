#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "engine/token_alphabet.h"
#include "app/depparser/arcstandard/state.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state)
    : sequence_instance(state.ref) {
  };

  const SequenceInstance* sequence_instance;
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_SCORE_CONTEXT_H__
