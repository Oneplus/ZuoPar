#ifndef __ZUOPAR_TYPES_DEPENDENCY_H__
#define __ZUOPAR_TYPES_DEPENDENCY_H__

#include <iostream>
#include <vector>
#include "common.h"

namespace ZuoPar {

class Dependency {
public:
  //! the forms of the dependency tree
  std::vector<form_t> forms;

  //! the part-of-speech tag of dependency tree.
  std::vector<postag_t> postags;

  //!
  std::vector<int>  heads;

  //!
  std::vector<deprel_t> deprels;

  void push_back(form_t form, postag_t postag, int head, deprel_t deprel);
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_DEPENDENCY_H__
