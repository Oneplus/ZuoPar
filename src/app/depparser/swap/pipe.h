#ifndef __ZUOPAR_APP_DEPPARSER_SWAP_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_SWAP_PIPE_H__

#include "app/depparser/pipe.h"
#include "app/depparser/multi_pipe.h"
#include "app/depparser/swap/action.h"
#include "app/depparser/swap/action_utils.h"
#include "app/depparser/swap/state.h"
#include "app/depparser/swap/weight.h"
#include "app/depparser/swap/decoder.h"
#include "app/depparser/swap/learner.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Swap {

struct MaxNunmberActions {
  int operator ()(const CoNLLXDependency& instance) {
    // According to Nivre (2009), the number of action is bounded by n+n^2
    return instance.size()* instance.size() + instance.size();
  }
};

typedef CoNLLXDependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNunmberActions
> Pipe;

typedef CoNLLXDependencyMultiPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MinibatchLearner, MaxNunmberActions
> MultiPipe;

} //  end for namespace swap
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_SWAP_PIPE_H__
