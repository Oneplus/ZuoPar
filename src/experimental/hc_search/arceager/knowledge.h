#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__

#include <set>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

static std::set<form_t> punctuation_forms;
static std::set<postag_t> punctuation_postags;
static std::set<postag_t> conjunction_postags;
static std::set<postag_t> pp_postags;

} //  namespace hcsearchdependencyparser
} //  namespace Experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__
