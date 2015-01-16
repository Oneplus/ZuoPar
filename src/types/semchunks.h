#ifndef __ZUOPAR_TYPES_SEMCHUNKS_H__
#define __ZUOPAR_TYPES_SEMCHUNKS_H__

#include <iostream>
#include <vector>
#include "types/common.h"

namespace ZuoPar {

const int kChunkOuterPrefix = 1;
const int kChunkBeginPrefix = 10000000;
const int kChunkInterPrefix = 20000000;
const int kChunkEndPrefix = 30000000;
const int kChunkSinglePrefix = 40000000;

const int kSemanticChunkOuterTag = 1;
const int kSemanticChunkBeginTag = 10000000;
const int kSemanticChunkInterTag = 20000000;

class SemanticChunks {
public:
  typedef std::pair< std::size_t, std::vector<tag_t> > predicate_t;

  std::vector<form_t> forms;          //! the forms of the semantic chunks
  std::vector<postag_t> postags;      //! the postags of the semantic chunks
  std::vector<tag_t> senses;          //! the senses of the semantic chunks
  std::vector<predicate_t> semchunks; //! the semantic chunks

  std::size_t size() const { return forms.size(); }
  std::size_t nr_predicates() const { return semchunks.size(); }
};

class MonoSemanticChunks {
public:
  MonoSemanticChunks(const std::vector<form_t>& forms_,
      const std::vector<postag_t>& postags_,
      const std::vector<tag_t>& senses_,
      const SemanticChunks::predicate_t& predicate_)
    : forms(forms_), postags(postags_), senses(senses_),
    predicate(predicate_) {
  }

  const std::vector<form_t>& forms;               //! The forms
  const std::vector<postag_t>& postags;           //! The postags
  const std::vector<tag_t>& senses;               //! The senses
  const SemanticChunks::predicate_t&  predicate;  //! The mono predicate

  std::size_t size() const { return forms.size(); }
};

class SemanticChunksWithChunks {
public:
  typedef std::pair< std::size_t, std::vector<tag_t> > predicate_t;
  std::vector<form_t> forms;
  std::vector<postag_t> postags;
  std::vector<tag_t> chunks;
  std::vector<tag_t> senses;
  std::vector<predicate_t> predicates;

  std::size_t size() const { return forms.size(); }
  std::size_t nr_predicates() const { return predicates.size(); }
};

} // namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_SEMCHUNKS_H__
