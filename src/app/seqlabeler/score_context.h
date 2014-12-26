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
    : instance(state.ref),
    buffer(state.buffer) {
    T_1= (state.buffer- 1 >= 0 ? state.tags[state.buffer- 1]: eg::TokenAlphabet::BEGIN);
  };

  const int& buffer;
  tag_t T_1;
  const SequenceInstance* instance;
};

} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_SCORE_CONTEXT_H__
