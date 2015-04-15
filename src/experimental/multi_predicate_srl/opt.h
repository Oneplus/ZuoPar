#ifndef __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__
#define __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

namespace fe = ZuoPar::FrontEnd;

struct ExtraOption {
  int cube_height;
  std::string verb_class_path;
  std::string predicate_tag;
  std::string argument_prefix;
};

struct LearnOption: public fe::LearnOption, public ExtraOption {};

struct TestOption: public fe::TestOption, public ExtraOption {
  std::string output_format;
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__
