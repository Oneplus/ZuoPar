#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_H__

#include <iostream>
#include "types/common.h"
#include "app/depparser/opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

struct LanguageOption { std::string language; };

struct LearnOption: public DependencyParser::LearnOption,
  public LanguageOption {
  std::string evaluation;
  std::string ranker;
};

struct TestOption: public DependencyParser::TestOption,
  public LanguageOption {
  floatval_t alpha;
};

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_H__
