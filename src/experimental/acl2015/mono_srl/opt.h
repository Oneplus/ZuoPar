#ifndef __ZUOPAR_APP_EXPERIMENTAL_MONO_SRL_OPT_H__
#define __ZUOPAR_APP_EXPERIMENTAL_MONO_SRL_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

namespace fe = ZuoPar::FrontEnd;

struct TestOption: public fe::TestOption {
  std::string output_format;
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_MONO_SRL_OPT_H__
