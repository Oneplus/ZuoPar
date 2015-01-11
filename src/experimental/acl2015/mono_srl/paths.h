#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_PATH_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_PATH_H__

#include <boost/lexical_cast.hpp>
#include "types/semchunks.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

class Paths {
public:
  Paths(const MonoSemanticChunks& r);

  std::vector<std::string> payload;
};

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_PATH_H__
