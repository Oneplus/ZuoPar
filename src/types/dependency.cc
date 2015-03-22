#include "types/dependency.h"

namespace ZuoPar {

void Dependency::push_back(const form_t& form, const postag_t& tag,
    const int& head, const deprel_t& deprel) {
  forms.push_back(form);
  postags.push_back(tag);
  heads.push_back(head);
  deprels.push_back(deprel);
}

size_t Dependency::size() const { return forms.size(); }

CoNLLXDependency::CoNLLXDependency(bool dummy_root) {
  if (dummy_root) {
    forms.push_back(1);     lemmas.push_back(1);
    cpostags.push_back(1);  postags.push_back(1);
    heads.push_back(-1);    deprels.push_back(0);
    feats.push_back(feat_list_t());
  }
}

void CoNLLXDependency::push_back(const form_t& form, const lemma_t& lemma,
    const postag_t& cpostag, const postag_t& postag,
    const feat_list_t& feat_list,
    const int& head, const deprel_t& deprel) {
  Dependency::push_back(form, postag, head, deprel);
  lemmas.push_back(lemma);
  cpostags.push_back(cpostag);
  feats.push_back(feat_list);
}

} //  end for namespace zuopar
