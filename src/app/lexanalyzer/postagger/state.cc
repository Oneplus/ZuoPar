#include <cstring>
#include "app/lexanalyzer/postagger/state.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

State::State(): ref(0) { clear(); }

State::State(const Postag* r): ref(r) { clear(); }

void State::copy(const State& source) {
  ref = source.ref;
  score = source.score;
  previous = source.previous;
  buffer = source.buffer;
  last_action = source.last_action;
  memcpy(postags, source.postags, sizeof(postags));
}

void State::clear() {
  score = 0;
  previous = 0;
  buffer = 0;
  memset(postags, 0, sizeof(postags));
}

bool State::tag(const State& source, postag_t postag) {
  if (source.terminated()) {
    return false;
  }

  copy(source);
  postags[buffer] = postag;
  ++ buffer;

  last_action = ActionFactory::make(postag);
  previous = &source;
  return true;
}

bool State::terminated() const { return (buffer == ref->size()); }

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
