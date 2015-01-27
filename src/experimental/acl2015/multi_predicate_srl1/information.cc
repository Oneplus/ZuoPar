#include <boost/lexical_cast.hpp>
#include "engine/token_alphabet.h"
#include "experimental/acl2015/multi_predicate_srl1/information.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace eg = ZuoPar::Engine;

Information::Information(const SemanticChunks& instance,
    const std::unordered_map<form_t, int>& verb_classes) {
  const std::vector<form_t>& forms(instance.forms);
  const std::vector<postag_t>& postags(instance.postags);

  int M = forms.size();
  int N = instance.nr_predicates();

  predicates_id.resize(N);
  predicates_form0.resize(N);
  predicates_postag0.resize(N);
  predicates_postag1.resize(N);
  predicates_postag_1.resize(N);
  predicates_verbclass.resize(N);
  postags_path_to_predicates.resize(N);

  for (int i = 0; i < instance.nr_predicates(); ++ i) {
    int predicate_id = instance.predicates[i].first;

    predicates_id[i] = predicate_id;
    predicates_form0[i] = forms[predicate_id];
    predicates_postag0[i] = postags[predicate_id];
    predicates_postag_1[i] = (predicate_id - 1 >= 0 ?
        postags[predicate_id- 1]:
        eg::TokenAlphabet::BEGIN);
    predicates_postag1[i] = (predicate_id + 1 < M ?
        postags[predicate_id+ 1]:
        eg::TokenAlphabet::END);
    std::unordered_map<form_t, int>::const_iterator entry =
      verb_classes.find(predicates_form0[i]);
    predicates_verbclass[i] = (entry != verb_classes.end() ? entry->second: 0);

    std::vector<std::string>& path = postags_path_to_predicates[i];
    path.resize(M);
    path[predicate_id] = boost::lexical_cast<std::string>(postags[predicate_id]);
    for (int j = predicate_id- 1; j >= 0; -- j) {
      path[j] = boost::lexical_cast<std::string>(postags[j])+ "|" + path[j+ 1];
    }
    for (int j = predicate_id+ 1; j < M; ++ j) {
      path[j] = path[j- 1]+ "|" + boost::lexical_cast<std::string>(postags[j]);
    }
  }
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
