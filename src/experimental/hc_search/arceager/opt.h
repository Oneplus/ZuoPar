#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;

struct RootOption { std::string root; };
struct PhaseOneModelOption { std::string phase_one_model_path; };
struct PhaseTwoModelOption { std::string phase_two_model_path; };

struct LearnOneOption:
  public fe::LearnOption,
  public RootOption,
  public PhaseOneModelOption {};

struct LearnTwoOption:
  public fe::LearnOption,
  public RootOption,
  public PhaseOneModelOption,
  public PhaseTwoModelOption {};

struct EvaluateOption:
  public fe::TestOption,
  public RootOption,
  public PhaseOneModelOption {};

struct TestOption:
  public fe::TestOption,
  public RootOption,
  public PhaseOneModelOption,
  public PhaseTwoModelOption {
  bool rerank;
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__
