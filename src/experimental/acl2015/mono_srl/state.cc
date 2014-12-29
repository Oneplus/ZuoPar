#include <cstring>
#include "experimental/acl2015/mono_srl/state.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

State::State()
  : ref(0) {
  clear();
}

State::State(const MonoSemanticChunks* r)
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
}

void
State::clear() {
  this->score = 0;
  this->previous = 0;
  this->buffer = 0;
}

bool
State::O(const State& source) {
  if (source.terminated()) { return false; }
  this->copy(source);
  ++ this->buffer;
  this->last_action = ActionFactory::make_O();
  this->previous = &source;
  return true;
}

bool
State::B(const State& source, const tag_t& tag) {
  if (source.terminated()) { return false; }
  this->copy(source);
  ++ this->buffer;
  this->last_action = ActionFactory::make_B(tag);
  this->previous = &source;
  return true;
}

bool
State::I(const State& source, const tag_t& tag) {
  if (source.terminated()) { return false; }
  this->copy(source);
  ++ this->buffer;
  this->last_action = ActionFactory::make_I(tag);
  this->previous = &source;
  return true;
}

bool
State::terminated() const {
  return (this->buffer == this->ref->size());
}

} //  namespace monosrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar
