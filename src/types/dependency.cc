#include "types/dependency.h"

namespace ZuoPar {

void
Dependency::push_back(form_t form,
    postag_t tag,
    int head,
    deprel_t deprel) {
  forms.push_back(form);
  postags.push_back(tag);
  heads.push_back(head);
  deprels.push_back(deprel);
}

size_t
Dependency::size() const {
  return forms.size();
}

} //  end for namespace zuopar
