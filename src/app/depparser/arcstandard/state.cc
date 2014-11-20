#include <cstring>
#include "app/depparser/arcstandard/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

State::State()
  : ref(0) {
  clear();
}

State::State(const Dependency* r)
  : ref(r) {
  clear();
}

void
State::copy(const State& source) {
  this->ref = source.ref;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  this->top0 = source.top0;
  this->top1 = source.top1;
  this->stack = source.stack;
  this->last_action = source.last_action;

  #define _COPY(name) memcpy((name), source.name, sizeof(name));
  _COPY(heads);
  _COPY(deprels);
  _COPY(left_most_child);
  _COPY(left_2nd_most_child);
  _COPY(right_most_child);
  _COPY(right_2nd_most_child);
  _COPY(nr_left_children);
  _COPY(nr_right_children);
  #undef _COPY
}

void
State::clear() {
  this->score = 0;
  this->previous = 0;
  this->top0 = 0;
  this->top1 = 0;
  this->buffer = 0;
  stack.clear();

  memset(heads, -1, sizeof(heads));
  memset(deprels, 0, sizeof(deprels));
  memset(nr_left_children, 0, sizeof(nr_left_children));
  memset(nr_right_children, 0, sizeof(nr_right_children));
  memset(left_most_child, -1, sizeof(left_most_child));
  memset(right_most_child, -1, sizeof(right_most_child));
  memset(left_2nd_most_child, -1, sizeof(left_2nd_most_child));
  memset(right_2nd_most_child, -1, sizeof(right_2nd_most_child));
}

void
State::refresh_stack_information() {
  size_t sz = stack.size();
  if (0 == sz) {
    top0 = -1;
    top1 = -1;
  } else if (1 == sz) {
    top0 = stack.at(sz - 1);
    top1 = -1;
  } else {
    top0 = stack.at(sz - 1);
    top1 = stack.at(sz - 2);
  }
}

bool
State::shift(const State& source) {
  if (source.buffer_empty()) {
    return false;
  }

  this->copy(source);
  stack.push_back(this->buffer);
  refresh_stack_information();
  ++ this->buffer;

  this->last_action = ActionFactory::make_shift();
  this->previous = &source;
  return true;
}

bool
State::left_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() < 2) {
    return false;
  }

  this->copy(source);
  stack.pop_back();
  stack.back() = top0;

  heads[top1] = top0;
  deprels[top1] = deprel;

  if (-1 == left_most_child[top0]) {
    // TP0 is left-isolate node.
    left_most_child[top0] = top1;
  } else if (top1 < left_most_child[top0]) {
    // (TP1, LM0, TP0)
    left_2nd_most_child[top0] = left_most_child[top0];
    left_most_child[top0] = top1;
  } else if (top1 < left_2nd_most_child[top0]) {
    // (LM0, TP1, TP0)
    left_2nd_most_child[top0] = top1;
  }

  ++ nr_left_children[top0];
  refresh_stack_information();
  this->last_action = ActionFactory::make_left_arc(deprel);
  this->previous = &source;
  return true;
}

bool
State::right_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() < 2) {
    return false;
  }

  this->copy(source);
  stack.pop_back();
  heads[top0] = top1;
  deprels[top0] = deprel;

  if (-1 == right_most_child[top1]) {
    // TP1 is right-isolate node.
    right_most_child[top1] = top0;
  } else if (right_most_child[top1] < top0) {
    right_2nd_most_child[top1] = right_most_child[top1];
    right_most_child[top1] = top0;
  } else if (right_2nd_most_child[top1] < top0) {
    right_2nd_most_child[top1] = top0;
  }
  ++ nr_right_children[top1];
  refresh_stack_information();
  this->last_action = ActionFactory::make_right_arc(deprel);
  this->previous = &source;
  return true;
}

bool
State::buffer_empty() const {
  return (this->buffer == this->ref->size());
}

size_t
State::stack_size() const {
  return (this->stack.size());
}

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
