#ifndef __ZUOPAR_APP_DEPPARSER_OPT_H__
#define __ZUOPAR_APP_DEPPARSER_OPT_H__

#include "frontend/common_opt.h"

namespace ZuoPar {
namespace DependencyParser {

namespace fe = ZuoPar::FrontEnd;

struct RootOption {
  std::string root;
};

struct LearnOption: public fe::LearnOption, public RootOption {
};

struct MultiLearnOption: public fe::MultiLearnOption, public RootOption {
};

struct TestOption: public fe::TestOption, public RootOption {
};

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_OPT_H__
