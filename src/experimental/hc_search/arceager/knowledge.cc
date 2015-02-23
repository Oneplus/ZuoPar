#include "experimental/hc_search/arceager/pipe.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

  //std::set<form_t> punctuation_forms;
  std::set<postag_t> PUNC_POS;
  std::set<postag_t> CONJ_POS;
  std::set<postag_t> ADP_POS;
  std::set<postag_t> VERB_POS;

  bool extract_punctuation = false;

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
