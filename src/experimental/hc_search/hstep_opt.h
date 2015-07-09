#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"
#include "app/depparser/opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

enum NegtiveSampleStrategy {
  kNegativeSamplingBaseline,
  kNegativeSamplingBest,
  kNegativeSamplingRandom,
  kNegativeSamplingWorst
};

struct LearnOption: public DependencyParser::LearnOption {
  std::string neg_sample;
};

struct LanguageOption { std::string language; };

struct PrepareOption:
  public DependencyParser::TestOption,
  public LanguageOption {
};

struct EvaluateOption:
  public DependencyParser::TestOption,
  public LanguageOption {
};

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_H__
