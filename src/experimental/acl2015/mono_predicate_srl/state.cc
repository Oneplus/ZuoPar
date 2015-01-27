#include <cstring>
#include "experimental/acl2015/mono_predicate_srl/state.h"
#include "experimental/acl2015/mono_predicate_srl/argument_relation_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

State::State()
  : ref(0), paths(0), verb_class(0) {
  clear();
}

State::State(const MonoSemanticChunks* r, const Paths* p, int vc)
  : ref(r), paths(p), verb_class(vc) {
  clear();
}

void
State::copy(const State& source) {
  this->ref = source.ref;
  this->paths = source.paths;
  this->verb_class = source.verb_class;
  this->label_set_low = source.label_set_low;
  this->label_set_high = source.label_set_high;
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
  this->label_set_low = 0;
  this->label_set_high = 0;
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
  if      (tag == ArgumentRelationUtils::arg0) { label_set_low |= (1<<0); }
  else if (tag == ArgumentRelationUtils::arg1) { label_set_low |= (1<<1); }
  else if (tag == ArgumentRelationUtils::arg2) { label_set_low |= (1<<2); }
  else if (tag == ArgumentRelationUtils::arg3) { label_set_low |= (1<<3); }
  else if (tag == ArgumentRelationUtils::arg4) { label_set_low |= (1<<4); }
  ++ this->buffer;
  this->last_action = ActionFactory::make_B(tag);
  this->previous = &source;
  return true;
}

bool
State::I(const State& source, const tag_t& tag) {
  if (source.terminated()) { return false; }
  this->copy(source);
  if      (tag == ArgumentRelationUtils::arg0) { label_set_low |= (1<<0); }
  else if (tag == ArgumentRelationUtils::arg1) { label_set_low |= (1<<1); }
  else if (tag == ArgumentRelationUtils::arg2) { label_set_low |= (1<<2); }
  else if (tag == ArgumentRelationUtils::arg3) { label_set_low |= (1<<3); }
  else if (tag == ArgumentRelationUtils::arg4) { label_set_low |= (1<<4); }
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
