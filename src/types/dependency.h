#ifndef __ZUOPAR_TYPES_DEPENDENCY_H__
#define __ZUOPAR_TYPES_DEPENDENCY_H__

#include <iostream>
#include <vector>
#include "common.h"

namespace ZuoPar {

struct Dependency {
  std::vector<form_t> forms;      //! the forms of the dependency tree
  std::vector<postag_t> postags;  //! the part-of-speech tag of dependency tree.
  std::vector<int>  heads;        //! the heads
  std::vector<deprel_t> deprels;  //! the dependency relation.

  Dependency() {}

  void build(const std::vector<form_t>& _forms,
      const std::vector<postag_t>& _postags,
      const std::vector<int>& _heads,
      const std::vector<deprel_t>& _deprels) {
    forms = _forms;
    postags = _postags;
    heads = _heads;
    deprels = _deprels;
  }

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
      const deprel_t& deprel) {
    forms.push_back(form);
    postags.push_back(postag);
    heads.push_back(head);
    deprels.push_back(deprel);
  }

  //! shorthand for forms.size();
  size_t size() const { return forms.size(); }
};

typedef int feat_t;
typedef std::vector<int> feat_list_t;

struct CoNLLXDependency: public Dependency {
  std::vector<lemma_t>      lemmas;   //! The lemma column.
  std::vector<postag_t>     cpostags; //! The corase postag.
  std::vector<feat_list_t>  feats;    //! The CoNLL feat column.

  CoNLLXDependency(bool dummy_root = true) {
    if (dummy_root) {
      forms.push_back(1);     lemmas.push_back(1);
      cpostags.push_back(1);  postags.push_back(1);
      heads.push_back(-1);    deprels.push_back(0);
      feats.push_back(feat_list_t());
    }
  }

  void push_back(const form_t& form,
      const lemma_t& lemma,
      const postag_t& cpostag,
      const postag_t& postag,
      const feat_list_t& feat_list,
      const int& head,
      const deprel_t& deprel) {
    Dependency::push_back(form, postag, head, deprel);
    lemmas.push_back(lemma);
    cpostags.push_back(cpostag);
    feats.push_back(feat_list);
  }
};

struct RawDependency {
  std::vector<std::string> forms;
  std::vector<std::string> postags;
  std::vector<int> heads;
  std::vector<std::string> deprels;

  RawDependency() {}

  void push_back(const std::string& form,
      const std::string& postag,
      const int& head,
      const std::string& deprel) {
    forms.push_back(form);
    postags.push_back(postag);
    heads.push_back(head);
    deprels.push_back(deprel);
  }

  size_t size() const { return forms.size(); }
};

struct RawCoNLLXDependency: public RawDependency {
  std::vector<std::string>  lemmas;   //! The lemma column.
  std::vector<std::string>  cpostags; //! The corase postag.
  std::vector<
    std::vector<std::string>>  feats;    //! The CoNLL feat column.

  RawCoNLLXDependency(bool dummy_root = true) {
    if (dummy_root) {
      forms.push_back("$ROOT$");    lemmas.push_back("$ROOT$");
      cpostags.push_back("$ROOT$"); postags.push_back("$ROOT$");
      heads.push_back(-1);          deprels.push_back("$ROOT$");
      feats.push_back(std::vector<std::string>());
    }
  }

  void push_back(const std::string& form,
      const std::string& lemma,
      const std::string& cpostag,
      const std::string& postag,
      const std::vector<std::string>& feat_list,
      const int& head,
      const std::string& deprel) {
    RawDependency::push_back(form, postag, head, deprel);
    lemmas.push_back(lemma);
    cpostags.push_back(cpostag);
    feats.push_back(feat_list);
  }
};

class DependencyUtils {
private:
  typedef std::vector<int> node_t;
  typedef std::vector<node_t> tree_t;
public:
  static bool is_tree(const std::vector<int>& heads) {
    tree_t tree(heads.size());
    int root = -1;
    for (int modifier = 0; modifier < heads.size(); ++ modifier) {
      int head = heads[modifier];
      if (head == -1) { root = modifier; } else { tree[head].push_back(modifier); }
    }
    std::vector<bool> visited(heads.size(), false);
    if (!is_tree_travel(root, tree, visited)) { return false; }
    for (bool visit: visited) { if (!visit) { return false; } }
    return true;
  }

  static bool is_tree_travel(int now, const tree_t& tree, std::vector<bool>& visited) {
    if (visited[now]) { return false; }

    visited[now] = true;
    for (int next: tree[now]) {
      if (!is_tree_travel(next, tree, visited)) { return false; } }
    return true;
  }

  static bool is_non_projective(const std::vector<int>& heads) {
    for (int modifier = 0; modifier < heads.size(); ++ modifier) {
      int head = heads[modifier];
      if (head == -1) { continue; }
      if (head < modifier) {
        for (int from = head+ 1; from < modifier; ++ from) {
          int to = heads[from];
          if (to < head || to > from) { return false; }
        }
      } else {
        for (int from = modifier+ 1; from < head; ++ from) {
          int to = heads[to];
          if (to < modifier || to > head) { return false; }
        }
      }
    }
    return false;
  }

  static bool is_projective(const std::vector<int>& heads) {
    return !is_non_projective(heads);
  }
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_DEPENDENCY_H__
