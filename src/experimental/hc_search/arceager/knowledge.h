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

/*bool non_punctuation;
bool extract_singleton;
bool extract_1st_bare;
bool extract_1st_direction;
bool extract_1st_distacne;
bool extract_1st_labeled;
bool extract_2nd_sibling_bare;
bool extract_2nd_sibling_direction;
bool extract_2nd_sibling_labeled;
bool extract_2nd_sibling_context;
bool extract_2nd_grandparent_bare;
bool extract_2nd_grandparent_direction;
bool extract_2nd_grandparent_labeled;
bool extract_2nd_bigram_bare;
bool extract_3rd_grand_grandparent_bare;
bool extract_3rd_grand_grandparent_direction;
bool extract_3rd_grand_grandparent_label;*/

} //  namespace hcsearchdependencyparser
} //  namespace Experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_KNOWLEDGE_H__
