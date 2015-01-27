#include <cstring>
#include "experimental/acl2015/multi_predicate_srl1/state.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

State::State()
  : ref(0), info(0) {
  clear();
}

State::State(const SemanticChunks& r, const Information& i)
  : ref(&r), info(&i) {
  clear();
}

void
State::copy(const State& source) {
  this->ref = source.ref;
  this->info = source.info;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  //this->label_set = source.label_set;
  this->last_action = source.last_action;
}

void
State::clear() {
  this->score = 0;
  this->previous = 0;
  this->buffer = 0;
}

bool
State::take(const State& source, const ActionCollection& collection) {
  if (source.terminated()) { return false; }
  this->copy(source);
  ++ this->buffer;
  this->last_action = collection;
  this->previous = &source;
  return true;
}

bool
State::terminated() const {
  return (this->buffer == this->ref->size());
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar
