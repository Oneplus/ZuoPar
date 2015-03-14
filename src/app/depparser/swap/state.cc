#include <cstring>
#include "app/depparser/swap/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

State::State(): ref(0) { clear(); }

State::State(const Dependency* r): ref(r) {
  clear();
  for (int i = ref->size() - 1; i >= 0; -- i) { buffer.push_back(i); }
}

void State::copy(const State& source) {
  this->ref = source.ref;
  this->score = source.score;
  this->previous = source.previous;
  this->buffer = source.buffer;
  this->top0 = source.top0;
  this->top1 = source.top1;
  this->front0 = source.front0;
  this->front1 = source.front1;
  this->stack = source.stack;
  this->last_action = source.last_action;

  #define _COPY(name) memcpy((name), source.name, sizeof(name));
  _COPY(heads);
  _COPY(deprels);
  _COPY(left_label_set);
  _COPY(right_label_set);
  _COPY(left_most_child);
  _COPY(left_2nd_most_child);
  _COPY(right_most_child);
  _COPY(right_2nd_most_child);
  _COPY(nr_left_children);
  _COPY(nr_right_children);
  #undef _COPY
}

void State::clear() {
  this->score = 0;
  this->previous = 0;
  this->top0 = -1;
  this->top1 = -1;
  this->front0 = 0;
  this->front1 = 1;
  stack.clear();
  buffer.clear();

  memset(heads, -1, sizeof(heads));
  memset(deprels, 0, sizeof(deprels));
  memset(left_label_set, 0, sizeof(left_label_set));
  memset(right_label_set, 0, sizeof(right_label_set));
  memset(nr_left_children, 0, sizeof(nr_left_children));
  memset(nr_right_children, 0, sizeof(nr_right_children));
  memset(left_most_child, -1, sizeof(left_most_child));
  memset(right_most_child, -1, sizeof(right_most_child));
  memset(left_2nd_most_child, -1, sizeof(left_2nd_most_child));
  memset(right_2nd_most_child, -1, sizeof(right_2nd_most_child));
}

void State::refresh_stack_information() {
  size_t sz = stack.size();
  if (0 == sz) {
    top0 = -1; top1 = -1;
  } else if (1 == sz) {
    top0 = stack.at(sz - 1); top1 = -1;
  } else {
    top0 = stack.at(sz - 1); top1 = stack.at(sz - 2);
  }

  front0 = buffer_empty() ? -1 : buffer.back();
  front1 = buffer.size() < 2 ? -1 : buffer[buffer.size() - 2];
}

bool State::shift(const State& source) {
  if (source.buffer_empty()) { return false; }
  copy(source);
  stack.push_back(buffer.back()); buffer.pop_back();
  refresh_stack_information();
  last_action = ActionFactory::make_shift();
  previous = &source;
  return true;
}

bool State::swap(const State& source) {
  if (source.stack.size() < 2) { return false; }
  copy(source);
  stack.pop_back(); stack.back() = top0; buffer.push_back(top1);
  refresh_stack_information();
  last_action = ActionFactory::make_swap();
  previous = &source;
  return true;
}

bool State::idle(const State& source) {
  if (!source.is_complete()) { return false; }
  copy(source);
  last_action = ActionFactory::make_idle();
  previous = &source;
  return true;
}


bool State::left_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() < 2) { return false; }

  this->copy(source);
  int h = top0, m = top1;

  stack.pop_back(); stack.back() = top0;
  refresh_stack_information();

  heads[m] = h; deprels[m] = deprel;
  left_label_set[h] |= (1<< deprel);
  if (-1 == left_most_child[h]) {
    // TP0 is left-isolate node.
    left_most_child[h] = m;
  } else if (m < left_most_child[h]) {
    // (TP1, LM0, TP0)
    left_2nd_most_child[h] = left_most_child[h];
    left_most_child[h] = m;
  } else if (m < left_2nd_most_child[h]) {
    // (LM0, TP1, TP0)
    left_2nd_most_child[h] = m;
  }
  ++ nr_left_children[h];

  this->last_action = ActionFactory::make_left_arc(deprel);
  this->previous = &source;
  return true;
}

bool State::right_arc(const State& source, deprel_t deprel) {
  if (source.stack.size() < 2) { return false; }

  this->copy(source);
  int h = top1, m = top0;

  stack.pop_back();
  refresh_stack_information();

  heads[m] = h; deprels[m] = deprel;
  right_label_set[h] |= (1<< deprel);
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

  this->last_action = ActionFactory::make_right_arc(deprel);
  this->previous = &source;
  return true;
}

bool State::buffer_empty() const { return buffer.empty(); }

bool State::is_complete() const { return stack.size() == 1 && buffer.empty(); }

size_t State::stack_size() const { return (this->stack.size()); }

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar
