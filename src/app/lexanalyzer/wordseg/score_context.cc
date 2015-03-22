#include "app/lexanalyzer/wordseg/score_context.h"
#include <boost/lexical_cast.hpp>

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

ScoreContext::ScoreContext(const State& state) {
  const std::vector<std::string>& chars = state.ref->chars;
  int i = state.index;
  int len = chars.size();
  prev_ch = i - 1 >= 0 ? chars[i- 1]: "__boc__";
  curr_ch = i < len ? chars[i]: "__eoc__";
  next_ch = i + 1 < len? chars[i+ 1]: "__eoc__";
  for (auto j = state.current_word_index; j <= i && j < len; ++ j) {
    if (j == state.current_word_index) { curr_w_first = chars[j]; }
    curr_w += chars[j];
    if (j == i || j + 1 == len) { curr_w_last = chars[j]; }
  }
  curr_w_len = i - state.current_word_index + 1;
  curr_w_len_s = boost::lexical_cast<std::string>(curr_w_len);
  if (state.previous_word_index != -1) {
    for (auto j = state.previous_word_index; j < state.current_word_index; ++ j) {
      if (j == state.previous_word_index) { prev_w_first = chars[j]; }
      prev_w += chars[j];
      if (j + 1 == state.current_word_index) { prev_w_last = chars[j]; }
    }
    prev_w_len = state.current_word_index- state.previous_word_index;
    prev_w_len_s = boost::lexical_cast<std::string>(prev_w_len);
  } else {
    prev_w_len = -1;
  }
}

} //  namespace chinesewordsegmentor
} //  namespace lexicalanalyzer
} //  namespace zuopar
