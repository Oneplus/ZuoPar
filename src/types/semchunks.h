#ifndef __ZUOPAR_TYPES_SEMCHUNKS_H__
#define __ZUOPAR_TYPES_SEMCHUNKS_H__

#include <iostream>
#include <vector>
#include "types/common.h"

namespace ZuoPar {

const int kSemanticChunkBeginTag = 10000000;
const int kSemanticChunkInterTag = 20000000;

class SemanticChunks {
public:
  typedef std::pair<
    std::size_t,
    std::vector<tag_t>
  > predicate_t;

  //! the forms of the semantic chunks
  std::vector<form_t> forms;

  //! the postags of the semantic chunks
  std::vector<postag_t> postags;

  //! the senses of the semantic chunks
  std::vector<tag_t> senses;

  //! the semantic chunks
  std::vector<predicate_t> semchunks;

  std::size_t size() const {
    return forms.size();
  }

  std::size_t nr_predicates() const {
    return semchunks.size();
  }
};

class MonoSemanticChunks {
public:
  MonoSemanticChunks(const std::vector<form_t>& forms_,
      const std::vector<postag_t>& postags_,
      const std::vector<tag_t>& senses_,
      const SemanticChunks::predicate_t& predicate_)
    : forms(forms_),
    postags(postags_),
    senses(senses_),
    predicate(predicate_) {
  }

  //! The forms
  const std::vector<form_t>& forms;

  //! The postags
  const std::vector<postag_t>& postags;

  //! The senses
  const std::vector<tag_t>& senses;

  //! The mono predicate
  const SemanticChunks::predicate_t&  predicate;

  std::size_t size() const {
    return forms.size();
  }
};

} // namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_SEMCHUNKS_H__
