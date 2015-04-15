#include <cstring>
#include "experimental/multi_predicate_srl/state.h"
#include "experimental/multi_predicate_srl/action_utils.h"
#include "experimental/multi_predicate_srl/argument_relation_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

State::State()
  : ref(0), info(0) {
  clear();
}

State::State(const SemanticChunks& r, const Information& i)
  : ref(&r), info(&i) {
  clear();
  this->label_set.resize(r.nr_predicates());
}

void
State::copy(const State& source) {
  this->ref = source.ref;
  this->info = source.info;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  this->label_set = source.label_set;
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
  for (int rank = 0; rank < collection.size(); ++ rank) {
    tag_t tag = ActionUtils::tag(collection[rank]);
    if      (tag == ArgumentRelationUtils::arg0) { label_set[rank] |= (1<<0); }
    else if (tag == ArgumentRelationUtils::arg1) { label_set[rank] |= (1<<1); }
    else if (tag == ArgumentRelationUtils::arg2) { label_set[rank] |= (1<<2); }
    else if (tag == ArgumentRelationUtils::arg3) { label_set[rank] |= (1<<3); }
    else if (tag == ArgumentRelationUtils::arg4) { label_set[rank] |= (1<<4); }
  }
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
} //  namespace sequencelabeler
} //  namespace zuopar
