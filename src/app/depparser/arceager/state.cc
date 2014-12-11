#include <cstring>
#include "app/depparser/arceager/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

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
  this->stack = source.stack;
  this->nr_empty_heads= source.nr_empty_heads;
  this->last_action = source.last_action;
  this->top0 = source.top0;
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
  this->buffer = 0;
  stack.clear();
  this->nr_empty_heads= 0;
  this->top0 = 0;
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
  } else {
    top0 = stack.at(sz - 1);
  }
}

bool
State::shift(const State& source) {
  if (source.buffer_empty()) {
    return false;
  }

  this->copy(source);
  stack.push_back(this->buffer);
  ++ this->nr_empty_heads;
  refresh_stack_information();
  ++ this->buffer;

  this->last_action = ActionFactory::make_shift();
  this->previous = &source;
  return true;
}

bool
State::reduce(const State& source) {
  if (source.stack.size() <= 0) {
    return false;
  }
  this->copy(source);
  stack.pop_back();
  refresh_stack_information();
  this->last_action = ActionFactory::make_reduce();
  this->previous = &source;
  return true;
}

bool
State::left_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() <= 0 || source.heads[source.top0] != -1) {
    return false;
  }

  this->copy(source);
  int h = buffer; int m = stack.back();
  stack.pop_back();
  -- this->nr_empty_heads;

  heads[m] = h;
  deprels[m] = deprel;

  if (-1 == left_most_child[h]) {
    // TP0 is left-isolate node.
    left_most_child[h] = m;
  } else if (m < left_most_child[h]) {
    // (TP1, LM0, TP0)
    left_2nd_most_child[h] = left_most_child[h];
    left_most_child[h] = m;
  } else if (m < left_2nd_most_child[top0]) {
    // (LM0, TP1, TP0)
    left_2nd_most_child[h] = m;
  }

  ++ nr_left_children[h];
  refresh_stack_information();
  this->last_action = ActionFactory::make_left_arc(deprel);
  this->previous = &source;
  return true;
}

bool
State::right_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() <= 0) {
    return false;
  }

  this->copy(source);
  int h = top0; int m = buffer;
  heads[m] = h;
  deprels[m] = deprel;

  stack.push_back(buffer);
  buffer ++;

  if (-1 == right_most_child[h]) {
    // TP1 is right-isolate node.
    right_most_child[h] = m;
  } else if (right_most_child[h] < m) {
    right_2nd_most_child[h] = right_most_child[h];
    right_most_child[h] = m;
  } else if (right_2nd_most_child[h] < m) {
    right_2nd_most_child[h] = m;
  }

  ++ nr_right_children[h];
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

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
