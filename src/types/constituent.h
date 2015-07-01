#ifndef __ZUOPAR_TYPES_CONSTITUENT_H__
#define __ZUOPAR_TYPES_CONSTITUENT_H__

#include <vector>
#include "common.h"

namespace ZuoPar {

struct BinarizedCFGTreeNode {
  bool constituent; //! true if constituent else false
  bool unary;       //! true if single child else false
  bool left_head;   //! true if left child is head else false
  int label;        //! A constituent label if constituent else postag
  int form;         //! None if constituent else form
  int left;         //! The index to the left child.
  int right;        //! The index to the right child.

  BinarizedCFGTreeNode()
    : constituent(false), unary(false), left_head(false),
    label(0), form(-1), left(-1), right(-1) {}

  BinarizedCFGTreeNode(form_t f, postag_t t)
    : constituent(false), unary(false), left_head(false),
    label(t), form(f), left(-1), right(-1) {}

  BinarizedCFGTreeNode(tag_t l)
    : constituent(true), unary(false), left_head(false),
    label(l), form(-1), left(-1), right(-1) {}
};

struct BinarizedCFGTree {
  typedef BinarizedCFGTreeNode node_t;
  std::vector<node_t> nodes;  //! The tree nodes.
  int root_index;
};

} //  namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_CONSTITUENT_H__
