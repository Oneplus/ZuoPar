#ifndef __ZUOPAR_APP_SEMLABELER_MONO_OPT_H__
#define __ZUOPAR_APP_SEMLABELER_MONO_OPT_H__

#include <iostream>
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace fe = ZuoPar::FrontEnd;

struct SemanticOption {
  std::string verb_class_path;
  std::string predicate_tag;
  std::string argument_prefix;
};

struct LearnOption: public fe::LearnOption, SemanticOption {};

struct TestOption: public fe::TestOption, SemanticOption {
  std::string output_format;
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMLABELER_MONO_OPT_H__
