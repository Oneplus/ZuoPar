#include <cstring>
#include "app/lexanalyzer/postagger/state.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

State::State()
  : ref(0) {
  clear();
}

State::State(const PostagWithLiteralCache* r)
  : ref(r) {
  clear();
}

void
State::copy(const State& source) {
  this->ref = source.ref;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  this->last_action = source.last_action;

  #define _COPY(name) memcpy((name), source.name, sizeof(name));
  _COPY(postags)
  #undef _COPY
}

void
State::clear() {
  this->score = 0;
  this->previous = 0;
  this->buffer = 0;
  memset(postags, 0, sizeof(postags));
}

bool
State::shift(const State& source, postag_t postag) {
  if (source.terminated()) {
    return false;
  }

  this->copy(source);
  this->postags[this->buffer] = postag;
  ++ this->buffer;

  this->last_action = ActionFactory::make_shift(postag);
  this->previous = &source;
  return true;
}

bool
State::terminated() const {
  return (this->buffer == this->ref->size());
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
