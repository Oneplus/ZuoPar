#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "utils/math/fast_binned.h"
#include "app/depparser/arcstandard/state.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state) {
    W0 = state.ref->cache[state.buffer];
  };

  std::string W0;
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__
