#include "experimental/acl2015/mono_srl/paths.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

Paths::Paths(const MonoSemanticChunks& r) {
  payload.resize(r.size());
  const std::vector<postag_t>& postags = r.postags;
  size_t predicate_index = r.predicate.first;
  payload[predicate_index] = boost::lexical_cast<std::string>(postags[predicate_index]);
  for (int i = predicate_index- 1; i >= 0; -- i) {
    payload[i] = boost::lexical_cast<std::string>(postags[i]) + "|" + payload[i+ 1];
  }
  for (size_t i = predicate_index+ 1; i < r.size(); ++ i) {
    payload[i] = payload[i- 1] + "|" + boost::lexical_cast<std::string>(postags[i]);
  }
}

} //  namespace monosrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar


