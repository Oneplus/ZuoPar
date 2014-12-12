#ifndef __ZUOPAR_TYPES_POSTAG_H__
#define __ZUOPAR_TYPES_POSTAG_H__

#include "common.h"
#include <vector>
#include <string>
namespace ZuoPar {

class Postag {
public:
  std::vector<form_t> forms;
  std::vector<postag_t> postags;

  std::size_t size() const { return forms.size(); }

  void push_back(form_t form, postag_t postag) {
    forms.push_back(form);
    postags.push_back(postag);
  }
};

class PostagWithLiteralCache: public Postag {
public:
  std::vector<std::string> cache;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_POSTAG_H__
