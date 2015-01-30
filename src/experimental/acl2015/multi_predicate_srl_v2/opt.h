#ifndef __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__
#define __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace fe = ZuoPar::FrontEnd;

struct LearnOption: public fe::LearnOption {
  std::string verb_class_path;
  std::string predicate_tag;
  std::string argument_prefix;
};

struct TestOption: public fe::TestOption {
  std::string verb_class_path;
  std::string output_format;
  std::string predicate_tag;
  std::string argument_prefix;
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_H__
