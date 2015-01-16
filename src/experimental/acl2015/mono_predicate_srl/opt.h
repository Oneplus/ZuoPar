#ifndef __ZUOPAR_APP_EXPERIMENTAL_MONO_PREDICATE_SRL_OPT_H__
#define __ZUOPAR_APP_EXPERIMENTAL_MONO_PREDICATE_SRL_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

namespace fe = ZuoPar::FrontEnd;

struct LearnOption: public fe::LearnOption {
  std::string verb_class_path;
  std::string predicate_tag;
};

struct TestOption: public fe::TestOption {
  std::string verb_class_path;
  std::string predicate_tag;
  std::string output_format;
};

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_MONO_PREDICATE_SRL_OPT_H__
