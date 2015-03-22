#ifndef __ZUOPAR_APP_LEXANALYZER_WORDSEG_SCORE_CONTEXT_H__
#define __ZUOPAR_APP_LEXANALYZER_WORDSEG_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "engine/token_alphabet.h"
#include "app/lexanalyzer/wordseg/state.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state);
  std::string prev_ch, curr_ch, next_ch;
  std::string curr_w, prev_w;
  std::string curr_w_first, curr_w_last, prev_w_first, prev_w_last;
  int curr_w_len, prev_w_len;
  std::string curr_w_len_s, prev_w_len_s;
};

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_WORDSEG_SCORE_CONTEXT_H__
