#ifndef __ZUOPAR_TYPES_DEPENDENCY_H__
#define __ZUOPAR_TYPES_DEPENDENCY_H__

#include <iostream>
#include <vector>
#include "common.h"

namespace ZuoPar {

class Dependency {
public:
  Dependency() {}

  Dependency(const std::vector<form_t>& _forms,
      const std::vector<postag_t>& _postags,
      const std::vector<int>& _heads,
      const std::vector<deprel_t>& _deprels)
    : forms(_forms), postags(_postags), heads(_heads), deprels(_deprels) {}

  //! the forms of the dependency tree
  std::vector<form_t> forms;

  //! the part-of-speech tag of dependency tree.
  std::vector<postag_t> postags;

  //!
  std::vector<int>  heads;

  //!
  std::vector<deprel_t> deprels;

  /**
   * Push a dependency node into the dependency parse.
   *
   *  @param[in]  form    The form.
   *  @param[in]  postag  The part-of-speech tag.
   *  @param[in]  head    The index for the head.
   *  @param[in]  deprel  The dependency relation.
   */
  void push_back(const form_t& form,
      const postag_t& postag,
      const int& head,
      const deprel_t& deprel);

  /**
   * Get number of forms in the dependency.
   *
   *  @return size_t  The number of forms in the dependency parse.
   */
  size_t size() const;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_DEPENDENCY_H__
