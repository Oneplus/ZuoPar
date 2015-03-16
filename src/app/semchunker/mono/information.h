#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_INFORMATION_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_INFORMATION_H__

#include <vector>
#include <unordered_map>
#include "types/common.h"
#include "types/semchunks.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

struct Information {
  Information(const MonoSemanticChunks& instance,
      const std::unordered_map<form_t, int>& verb_classes);

  int predicate_id;             //! The index of predicates.
  form_t predicate_form0;       //! The forms of the predicates.
  postag_t predicate_postag0;   //! The postags of the predicates.
  postag_t predicate_postag_1;  //! The postags of the predicates context.
  postag_t predicate_postag1;   //! The postags of the predicates context.
  tag_t predicate_verbclass;    //! The verb class of the predicate.
  std::vector<std::string> postag_paths;  //! The postags paths.
};

} //  namespace multipredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_INFORMATION_H__
