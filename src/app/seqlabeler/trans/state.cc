#include <cstring>
#include "app/seqlabeler/trans/state.h"

namespace ZuoPar {
namespace SequenceLabeler {

State::State(): ref(0) { clear(); }

State::State(const SequenceInstance* r): ref(r) { clear(); }

void State::copy(const State& source) {
  this->ref = source.ref;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  this->last_action = source.last_action;

  #define _COPY(name) memcpy((name), source.name, sizeof(name));
  _COPY(tags)
  #undef _COPY
}

void State::clear() {
  this->score = 0;
  this->previous = 0;
  this->buffer = 0;
  memset(tags, 0, sizeof(tags));
}

bool State::tag(const State& source, tag_t tag) {
  if (source.terminated()) { return false; }

  this->copy(source);
  this->tags[this->buffer] = tag;
  ++ this->buffer;

  this->last_action = ActionFactory::make(tag);
  this->previous = &source;
  return true;
}

bool State::terminated() const { return (this->buffer == this->ref->size()); }

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
