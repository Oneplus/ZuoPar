#ifndef __ZUOPAR_TYPES_CHUNKS_H__
#define __ZUOPAR_TYPES_CHUNKS_H__

#include <iostream>
#include <vector>
#include "types/common.h"

namespace ZuoPar {

const int kChunkOuterPrefix = 1;
const int kChunkBeginPrefix = 10000000;
const int kChunkInterPrefix = 20000000;
const int kChunkEndPrefix = 30000000;
const int kChunkSinglePrefix = 40000000;

class ChunksWithSemanticRoles {
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

class MonoChunksWithSemanticRoles {
public:
  MonoChunksWithSemanticRoles(const std::vector<form_t>& forms_,
      const std::vector<postag_t>& postags_,
      const std::vector<tag_t>& chunks_,
      const std::vector<tag_t>& senses_,
      const ChunksWithSemanticRoles::predicate_t& predicate_)
    : forms(forms_), postags(postags_), chunks(chunks_),
    senses(senses_), predicate(predicate_) {
  }

  const std::vector<form_t>& forms;
  const std::vector<postag_t>& postags;
  const std::vector<tag_t>& chunks;
  const std::vector<tag_t>& senses;
  const ChunksWithSemanticRoles::predicate_t& predicate;
  std::size_t size() const { return forms.size(); }
};

} //  namespace zuopar

#endif  //  end for __ZUOPAR_TYPES_CHUNKS_H__
