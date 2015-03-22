#include "app/lexanalyzer/wordseg/state.h"
#include "app/lexanalyzer/wordseg/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

State::State(): ref(0) { clear(); }
State::State(const Segmentation* r): ref(r) {
  index = 1;
  previous_word_index = -1;
  current_word_index = 0;
  previous = 0;
  score = 0;
}

void State::copy(const State& source) {
  ref = source.ref;
  score = source.score;
  previous = source.previous;
  index = source.index;
  previous_word_index = source.previous_word_index;
  current_word_index = source.current_word_index;
  last_action = source.last_action;
}

void State::clear() {
  score = 0;
  previous = 0;
  index = 0;
  previous_word_index = -1;
  current_word_index = -1;
}

bool State::split(const State& source) {
  if (source.terminated()) { return false; }
  copy(source);
  previous_word_index = source.current_word_index;
  current_word_index = source.index;
  ++ index;
  last_action = ActionFactory::make_split();
  previous = &source;
  return true;
}

bool State::append(const State& source) {
  if (source.terminated()) { return false; }
  copy(source);
  previous_word_index = source.previous_word_index;
  current_word_index = source.current_word_index;
  ++ index;
  last_action = ActionFactory::make_append();
  previous = &source;
  return true;
}

bool State::terminated() const { return (index == ref->nr_chars()+ 1); }

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
