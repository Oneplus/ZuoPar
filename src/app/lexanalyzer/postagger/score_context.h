#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "engine/token_alphabet.h"
#include "app/lexanalyzer/postagger/state.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state) {
    std::size_t len = state.ref->size();
    W0 = state.ref->forms[state.buffer];
    W1 = (state.buffer+ 1 < len ? state.ref->forms[state.buffer+ 1]: "__eos__");
    W2 = (state.buffer+ 2 < len ? state.ref->forms[state.buffer+ 2]: "__eos__");
    W_1= (state.buffer- 1 >= 0 ? state.ref->forms[state.buffer- 1]: "__bos__");
    W_2= (state.buffer- 2 >= 0 ? state.ref->forms[state.buffer- 2]: "__bos__");
    T_1= (state.buffer- 1 >= 0 ? state.postags[state.buffer- 1]: eg::TokenAlphabet::BEGIN);
    T_2= (state.buffer- 2 >= 0 ? state.postags[state.buffer- 2]: eg::TokenAlphabet::BEGIN);
    std::size_t sz = W0.size();
    PRE1 = (sz >= 1 ? W0.substr(0, 1): "");
    PRE2 = (sz >= 2 ? W0.substr(0, 2): "");
    PRE3 = (sz >= 3 ? W0.substr(0, 3): "");
    PRE4 = (sz >= 4 ? W0.substr(0, 4): "");
    SUF1 = (sz >= 1 ? W0.substr(sz- 1, 1): "");
    SUF2 = (sz >= 2 ? W0.substr(sz- 2, 2): "");
    SUF3 = (sz >= 3 ? W0.substr(sz- 3, 3): "");
    SUF4 = (sz >= 4 ? W0.substr(sz- 4, 4): "");
    NUM = (W0.find_first_of("0123456789") != std::string::npos) ? 1: 0;
    UPPERCASE = (W0.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ") != std::string::npos)? 1: 0;
    HYPHEN = (W0.find('-') != std::string::npos) ? 1: 0;
  };

  std::string W0, W1, W2, W_1, W_2;
  std::string PRE1, PRE2, PRE3, PRE4;
  std::string SUF1, SUF2, SUF3, SUF4;
  postag_t T_1, T_2;
  int NUM, UPPERCASE, HYPHEN;
};

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_SCORE_CONTEXT_H__
