#ifndef __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_EXTRA_INFORMATION_H__
#define __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_EXTRA_INFORMATION_H__

#include <vector>
#include <unordered_map>
#include "types/common.h"
#include "types/semchunks.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

struct Information {
  Information(const SemanticChunks& instance,
      const std::unordered_map<form_t, int>& verb_classes);

  //! The index of predicates.
  std::vector<int> predicates_id;

  //! The forms of the predicates.
  std::vector<form_t> predicates_form0;

  //! The postags of the predicates.
  std::vector<postag_t> predicates_postag0;

  //! The postags of the predicates context.
  std::vector<postag_t> predicates_postag_1;

  //! The postags of the predicates context.
  std::vector<postag_t> predicates_postag1;

  //! The verb class of the predicate.
  std::vector<tag_t> predicates_verbclass;

  //! The postags paths.
  std::vector< std::vector<std::string> > postags_path_to_predicates;
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_EXTRA_INFORMATION_H__
