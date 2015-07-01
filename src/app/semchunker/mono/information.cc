#include <boost/lexical_cast.hpp>
#include "engine/token_alphabet.h"
#include "app/semchunker/mono/information.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace eg = ZuoPar::Engine;

Information::Information(const MonoSemanticChunks& instance,
    const std::unordered_map<form_t, int>& verb_classes) {
  const std::vector<form_t>& forms(instance.forms);
  const std::vector<postag_t>& postags(instance.postags);
  int M = forms.size();

  predicate_id = instance.predicate.first;

  predicate_form0 = forms[predicate_id];
  predicate_postag0 = postags[predicate_id];
  predicate_postag_1 = (predicate_id - 1 >= 0 ?
      postags[predicate_id- 1]:
      eg::TokenAlphabet::BEGIN);
  predicate_postag1 = (predicate_id + 1 < M ?
      postags[predicate_id+ 1]:
      eg::TokenAlphabet::END);

  std::unordered_map<form_t, int>::const_iterator entry =
    verb_classes.find(predicate_form0);

  predicate_verbclass = (entry != verb_classes.end() ? entry->second: -1);

  std::vector<std::string>& paths= postag_paths;
  paths.resize(M);
  paths[predicate_id] = boost::lexical_cast<std::string>(postags[predicate_id]);
  for (int j = predicate_id- 1; j >= 0; -- j) {
    paths[j] = boost::lexical_cast<std::string>(postags[j])+ "|" + paths[j+ 1];
  }
  for (int j = predicate_id+ 1; j < M; ++ j) {
    paths[j] = paths[j- 1]+ "|" + boost::lexical_cast<std::string>(postags[j]);
  }
}

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar
