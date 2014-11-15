#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_STATE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_STATE_H__

#include "types/common.h"
#include "types/dependency.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class State {
public:
  //! The pointer to the previous state.
  std::vector<int> stack;
  int buffer;
  const State* previous;
  const Dependency* ref;
  floatval_t score;
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar
#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_STATE_H__
