#ifndef __ZUOPAR_APP_CONPARSER_OPT_H__
#define __ZUOPAR_APP_CONPARSER_OPT_H__

#include "frontend/common_opt.h"

namespace ZuoPar {
namespace ConstituentParser {

namespace fe = ZuoPar::FrontEnd;

struct LearnOption: public fe::LearnOption {
};

struct MultiLearnOption: public fe::MultiLearnOption {
};

struct TestOption: public fe::TestOption {
};

} //  namespace constituentparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_CONPARSER_OPT_H__
