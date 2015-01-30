#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_EXTRA_INFORMATION_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_EXTRA_INFORMATION_H__

#include <vector>
#include <unordered_map>
#include "types/common.h"
#include "types/semchunks.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

struct Information {
  Information(const MonoSemanticChunks& instance,
      const std::unordered_map<form_t, int>& verb_classes);

  //! The index of predicates.
  int predicate_id;

  //! The forms of the predicates.
  form_t predicate_form0;

  //! The postags of the predicates.
  postag_t predicate_postag0;

  //! The postags of the predicates context.
  postag_t predicate_postag_1;

  //! The postags of the predicates context.
  postag_t predicate_postag1;

  //! The verb class of the predicate.
  tag_t predicate_verbclass;

  //! The postags paths.
  std::vector<std::string> postag_paths;
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_EXTRA_INFORMATION_H__
