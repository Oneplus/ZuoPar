#ifndef __ZUOPAR_TYPES_DEPENDENCY_H__
#define __ZUOPAR_TYPES_DEPENDENCY_H__

#include <iostream>
#include <vector>
#include "common.h"

namespace ZuoPar {

struct Dependency {
  Dependency() {}

  std::vector<form_t> forms;      //! the forms of the dependency tree
  std::vector<postag_t> postags;  //! the part-of-speech tag of dependency tree.
  std::vector<int>  heads;        //! the heads
  std::vector<deprel_t> deprels;  //! the dependency relation.

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

  size_t size() const;  // shorthand for forms.size();
};

typedef int feat_t;
typedef std::vector<int> feat_list_t;

struct CoNLLXDependency: Dependency {
  CoNLLXDependency(bool dummy_root = true);

  std::vector<lemma_t>      lemmas;   //! The lemma column.
  std::vector<postag_t>     cpostags; //! The corase postag.
  std::vector<feat_list_t>  feats;    //! The CoNLL feat column.

  void push_back(const form_t& form,
      const lemma_t& lemma,
      const postag_t& cpostag,
      const postag_t& postag,
      const feat_list_t& feat_list,
      const int& head,
      const deprel_t& deprel);
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_DEPENDENCY_H__
