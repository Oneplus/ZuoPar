#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__

#include <set>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

//static std::set<form_t> PUNC;
static std::set<postag_t> PUNC_POS;
static std::set<postag_t> CONJ_POS;
static std::set<postag_t> ADP_POS;
static std::set<postag_t> VERB_POS;
//static std::set<postag_t> ;

} //  namespace hcsearchdependencyparser
} //  namespace Experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__
