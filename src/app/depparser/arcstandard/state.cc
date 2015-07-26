#include <cstring>
#include <climits>
#include <bitset>
#include <boost/multi_array.hpp>
#include "utils/logging.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/action_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

State::State(): ref(0) { clear(); }
State::State(const CoNLLXDependency* r): ref(r) { clear(); }

bool State::can_shift() const     { return !buffer_empty(); }
bool State::can_left_arc() const  { return stack_size() >= 2; }
bool State::can_right_arc() const { return stack_size() >= 2; }

void State::copy(const State& source) {
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
  this->buffer = 0;
  stack.clear();

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

bool State::shift(const State& source) {
  if (!source.can_shift()) { return false; }

  this->copy(source);
  stack.push_back(this->buffer);
  refresh_stack_information();
  ++ this->buffer;

  this->last_action = ActionFactory::make_shift();
  this->previous = &source;
  return true;
}

bool State::left_arc(const State& source, deprel_t deprel) {
  if (!source.can_left_arc()) { return false; }

  this->copy(source);
  stack.pop_back();
  stack.back() = top0;

  heads[top1] = top0;
  deprels[top1] = deprel;
  left_label_set[top0] |= (1<< deprel);

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

bool State::right_arc(const State& source, deprel_t deprel) {
  if (!source.can_right_arc()) { return false; }

  this->copy(source);
  stack.pop_back();
  heads[top0] = top1;
  deprels[top0] = deprel;
  right_label_set[top1] |= (1<< deprel);

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

int State::cost(const std::vector<int>& gold_heads,
    const std::vector<int>& gold_deprels) {
  std::vector< std::vector<int> > tree(gold_heads.size());
  for (auto i = 0; i < gold_heads.size(); ++ i) {
    auto h = gold_heads[i]; if (h >= 0) { tree[h].push_back(i); }
  }

  const std::vector<int>& sigma_l = stack;
  std::vector<int> sigma_r; sigma_r.push_back(stack.back());

  std::bitset<kMaxNumberOfWords> sigma_l_mask;
  std::bitset<kMaxNumberOfWords> sigma_r_mask;
  for (auto s: sigma_l) { sigma_l_mask.set(s); }

  for (int i = buffer; i < ref->size(); ++ i) {
    if (gold_heads[i] < buffer) {
      sigma_r.push_back(i);
      sigma_r_mask.set(i);
      continue;
    }

    for (auto d: tree[i]) {
      if (sigma_l_mask.test(d) || sigma_r_mask.test(d)) {
        sigma_r.push_back(i);
        sigma_r_mask.set(i);
        break;
      }
    }
  }

  int len_l = sigma_l.size();
  int len_r = sigma_r.size();

  typedef boost::multi_array<int, 3> array_t;
  array_t T(boost::extents[len_l][len_r][len_l+len_r-1]);
  std::fill( T.origin(), T.origin()+ T.num_elements(), 1024);

  T[0][0][len_l-1]= 0;
  for (int d = 0; d < len_l+len_r- 1; ++ d) {
    for (int j = std::max(0, d-len_l+1); j < std::min(d+1, len_r); ++ j) {
      int i = d-j;
      if (i < len_l-1) {
        int i_1 = sigma_l[len_l-i-2];
        int i_1_rank = len_l-i-2;
        for (auto rank = len_l-i-1; rank < len_l; ++ rank) {
          auto h = sigma_l[rank];
          auto h_rank = rank;
          T[i+1][j][h_rank] = std::min(T[i+1][j][h_rank],
              T[i][j][h_rank] + (gold_heads[i_1] == h ? 0: 2));
          T[i+1][j][i_1_rank] = std::min(T[i+1][j][i_1_rank],
              T[i][j][h_rank] + (gold_heads[h] == i_1 ? 0: 2));
        }
        for (auto rank = 1; rank < j+1; ++ rank) {
          auto h =sigma_r[rank];
          auto h_rank = len_l+rank-1;
          T[i+1][j][h_rank] = std::min(T[i+1][j][h_rank],
              T[i][j][h_rank] + (gold_heads[i_1] == h ? 0: 2));
          T[i+1][j][i_1_rank] = std::min(T[i+1][j][i_1_rank],
              T[i][j][h_rank] + (gold_heads[h] == i_1 ? 0: 2));
        }
      }
      if (j < len_r-1) {
        int j_1 = sigma_r[j+1];
        int j_1_rank = len_l+j;
        for (auto rank = len_l-i-1; rank < len_l; ++ rank) {
          auto h = sigma_l[rank];
          auto h_rank = rank;
          T[i][j+1][h_rank] = std::min(T[i][j+1][h_rank],
              T[i][j][h_rank] + (gold_heads[j_1] == h ? 0: 2));
          T[i][j+1][j_1_rank] = std::min(T[i][j+1][j_1_rank],
              T[i][j][h_rank] + (gold_heads[h] == j_1 ? 0: 2));
        }
        for (auto rank = 1; rank < j+1; ++ rank) {
          auto h =sigma_r[rank];
          auto h_rank = len_l+rank-1;
          T[i][j+1][h_rank] = std::min(T[i][j+1][h_rank],
              T[i][j][h_rank] + (gold_heads[j_1] == h ? 0: 2));
          T[i][j+1][j_1_rank] = std::min(T[i][j+1][j_1_rank],
              T[i][j][h_rank] + (gold_heads[h] == j_1 ? 0: 2));
        }
      }
    }
  }
  auto penalty = 0;
  for (int i = 0; i < buffer; ++ i) {
    if (heads[i] != -1) {
      if (heads[i] != gold_heads[i]) { penalty += 2; }
      else if (deprels[i] != gold_deprels[i]) { penalty += 1; }
    }
  }
  return T[len_l-1][len_r-1][0]+ penalty;
}

bool State::buffer_empty() const { return (this->buffer == this->ref->size()); }
size_t State::stack_size() const { return (this->stack.size()); }
bool State::is_complete() const           { return stack.size() == 1 && buffer == ref->size(); }

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
