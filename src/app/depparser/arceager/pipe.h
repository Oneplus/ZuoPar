#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_PIPE_H__

#include "app/depparser/pipe.h"
#include "app/depparser/multi_pipe.h"
#include "app/depparser/arceager/action.h"
#include "app/depparser/arceager/action_utils.h"
#include "app/depparser/arceager/state.h"
#include "app/depparser/arceager/weight.h"
#include "app/depparser/arceager/decoder.h"
#include "app/depparser/arceager/learner.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

struct MaxNumberActions {
  int operator ()(const Dependency& instance) {
    return instance.size()* 2- 1;
  }
};

typedef CoNLLXDependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberActions
> Pipe;

typedef CoNLLXDependencyMultiPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MinibatchLearner, MaxNumberActions
> MultiPipe;

typedef GreedySearchCoNLLXDependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNumberActions
> GreedyPipe;

} //  end for namespace arceager
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_EAGER_PIPE_H__
