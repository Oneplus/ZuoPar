#include <cstring>
#include "utils/print/vector.h"
#include "utils/logging.h"
#include "app/depparser/choi2013/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

State::State(): ref(0) { clear(); }
State::State(const CoNLLXDependency* r): ref(r) { clear(); }

void State::copy(const State& source) {
  ref = source.ref;
  score = source.score;
  previous = source.previous;
  buffer = source.buffer;
  stack = source.stack;
  deque = source.deque;
  nr_empty_heads= source.nr_empty_heads;
  last_action = source.last_action;
  top0 = source.top0;
  top1 = source.top1;
  #define _COPY(name) memcpy((name), source.name, sizeof(name));
  _COPY(heads);
  _COPY(deprels);
  _COPY(left_label_set_lowbit);
  _COPY(left_label_set_highbit);
  _COPY(right_label_set_lowbit);
  _COPY(right_label_set_highbit);
  _COPY(left_most_child);
  _COPY(left_2nd_most_child);
  _COPY(right_most_child);
  _COPY(right_2nd_most_child);
  _COPY(nr_left_children);
  _COPY(nr_right_children);
  #undef _COPY
}

void State::clear() {
  score = 0;
  previous = 0;
  buffer = 0;
  stack.clear();
  deque.clear();
  nr_empty_heads= 0;
  top0 = -1;
  top1 = -1;
  memset(heads, -1, sizeof(heads));
  memset(deprels, 0, sizeof(deprels));
  memset(left_label_set_lowbit,   0, sizeof(left_label_set_lowbit));
  memset(left_label_set_highbit,  0, sizeof(left_label_set_highbit));
  memset(right_label_set_lowbit,  0, sizeof(right_label_set_lowbit));
  memset(right_label_set_highbit, 0, sizeof(right_label_set_highbit));
  memset(nr_left_children,  0, sizeof(nr_left_children));
  memset(nr_right_children, 0, sizeof(nr_right_children));
  memset(left_most_child,  -1, sizeof(left_most_child));
  memset(right_most_child, -1, sizeof(right_most_child));
  memset(left_2nd_most_child,  -1, sizeof(left_2nd_most_child));
  memset(right_2nd_most_child, -1, sizeof(right_2nd_most_child));
}

void State::_update_stack_information() {
  size_t sz = stack.size();
  if (0 == sz)      { top0 = -1; top1 = -1; }
  else if (1 == sz) { top0 = stack.back(); top1 = -1; }
  else              { top0 = stack.back(); top1 = stack[stack.size() - 2]; }
}

void State::_update_left_children_information(int h, int m) {
  if (-1 == left_most_child[h]) {             // m is left-isolate node.
    left_most_child[h] = m;
  } else if (m < left_most_child[h]) {        // (m, lm0, h)
    left_2nd_most_child[h] = left_most_child[h];
    left_most_child[h] = m;
  } else if (m < left_2nd_most_child[top0]) { // (lm0, m, h)
    left_2nd_most_child[h] = m;
  }
  ++ nr_left_children[h];
}

void State::_update_right_children_information(int h, int m) {
  if (-1 == right_most_child[h]) {            // TP1 is right-isolate node.
    right_most_child[h] = m;
  } else if (right_most_child[h] < m) {       // (h, rm0, m)
    right_2nd_most_child[h] = right_most_child[h];
    right_most_child[h] = m;
  } else if (right_2nd_most_child[h] < m) {   // (h, m, rm0)
    right_2nd_most_child[h] = m;
  }
  ++ nr_right_children[h];
}

void State::_update_left_label_set(int h, int deprel) {
  if (deprel >= sizeof(unsigned)) {
    left_label_set_highbit[h] |= (1<< (deprel - sizeof(unsigned)));
  } else {
    left_label_set_lowbit[h]  |= (1<< deprel);
  }
}

void State::_update_right_label_set(int h, int deprel) {
  if (deprel >= sizeof(unsigned)) {
    right_label_set_highbit[h] |= (1<< (deprel - sizeof(unsigned)));
  } else {
    right_label_set_lowbit[h]  |= (1<< deprel);
  }
}

void State::_shift() {
  while (!deque.empty()) { stack.push_back(deque.back()); deque.pop_back(); }
  stack.push_back(buffer);  ++ buffer;
  _update_stack_information();
}

void State::_reduce() {
  stack.pop_back();
  _update_stack_information();
}

void State::_pass() {
  deque.push_back(stack.back()); stack.pop_back();
  _update_stack_information();
}

bool State::idle(const State& source) {
  if (!source.is_complete()) { return false; }
  copy(source);
  last_action = ActionFactory::make_idle();
  previous = &source;
  return true;
}

bool State::shift(const State& source) {
  if (source.buffer_empty()) { return false; }
  copy(source);
  if (heads[buffer] == -1) { ++ nr_empty_heads; }
  _shift();
  last_action = ActionFactory::make_shift();
  previous = &source;
  return true;
}

bool State::reduce(const State& source) {
  if (source.stack_empty()) { return false; }
  copy(source);
  _reduce();
  last_action = ActionFactory::make_reduce();
  previous = &source;
  return true;
}

bool State::no_pass(const State& source) {
  if (source.stack_empty()) { return false; }
  copy(source);
  _pass();
  last_action = ActionFactory::make_no_pass();
  previous = &source;
  return true;
}

bool State::left_arc(const State& source, deprel_t deprel) {
  if (source.stack_empty() || source.stack_top_has_head()) { return false; }

  copy(source);
  int h = buffer; int m = stack.back();
  _reduce();
  -- nr_empty_heads;

  heads[m] = h; deprels[m] = deprel;
  _update_left_label_set(h, deprel);
  _update_left_children_information(h, m);
  last_action = ActionFactory::make_left_arc(deprel);
  previous = &source;
  return true;
}

bool State::right_arc(const State& source, deprel_t deprel) {
  if (source.stack_empty()) { return false; }

  copy(source);
  int h = top0; int m = buffer;
  _shift();

  heads[m] = h; deprels[m] = deprel;
  _update_right_label_set(h, deprel);
  _update_right_children_information(h, m);
  last_action = ActionFactory::make_right_arc(deprel);
  previous = &source;
  return true;
}

bool State::left_pass(const State& source, deprel_t deprel) {
  if (source.stack_empty() || source.heads[source.top0] != -1) { return false; }

  copy(source);
  int h = buffer; int m = top0;
  _pass();
  -- nr_empty_heads;

  heads[m] = h; deprels[m] = deprel;
  _update_left_label_set(h, deprel);
  _update_left_children_information(h, m);
  last_action = ActionFactory::make_left_pass(deprel);
  previous = &source;
  return true;
}

bool State::right_pass(const State& source, deprel_t deprel) {
  if (source.stack_empty()) { return false; }

  copy(source);
  int h = top0; int m = buffer;
  _pass();

  heads[m] = h; deprels[m] = deprel;
  _update_right_label_set(h, deprel);
  _update_right_children_information(h, m);
  last_action = ActionFactory::make_right_pass(deprel);
  previous = &source;
  return true;
}

bool State::buffer_empty() const          { return (buffer == ref->size()); }
bool State::stack_empty() const           { return stack.empty(); }
bool State::reach_last_token() const      { return (buffer == ref->size() - 1);}
bool State::stack_top_has_head() const    { return heads[top0] != -1; }
bool State::buffer_front_has_head() const { return heads[buffer] != -1; }
bool State::is_complete() const           { return stack.size() == 1 && buffer == ref->size(); }

bool State::is_descendant(int parent, int child) const {
  while (child != -1) {
    child = heads[child];
    if (child == parent) { return true; }
  }
  return false;
}

size_t State::stack_size() const { return (stack.size()); }

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar
