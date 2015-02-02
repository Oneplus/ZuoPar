#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;

struct LearnOneOption: public fe::LearnOption {
  std::string phase_one_model_path;
};

struct LearnTwoOption: public fe::LearnOption {
  std::string phase_one_model_path; //! The path to the model in phase ONE.
  std::string phase_two_model_path; //! The path to the model in phase TWO.
};

struct TestOption: public fe::TestOption {
  std::string phase_one_model_path; //! The path to the model in phase ONE.
  std::string phase_two_model_path; //! The path to the model in phase TWO.
};


} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_H__
