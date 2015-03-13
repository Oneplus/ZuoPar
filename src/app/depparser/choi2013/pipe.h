#ifndef __ZUOPAR_APP_DEPPARSER_CHOI2013_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_CHOI2013_PIPE_H__

#include "app/depparser/pipe.h"
#include "app/depparser/multi_pipe.h"
#include "app/depparser/choi2013/action.h"
#include "app/depparser/choi2013/action_utils.h"
#include "app/depparser/choi2013/state.h"
#include "app/depparser/choi2013/weight.h"
#include "app/depparser/choi2013/decoder.h"
#include "app/depparser/choi2013/learner.h"

namespace ZuoPar {
namespace DependencyParser {
namespace Choi2013 {

struct MaxNunmberActions {
  int operator ()(const Dependency& instance) {
    return instance.size()* instance.size() / 4;
  }
};

typedef DependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNunmberActions
> Pipe;

typedef DependencyMultiPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MinibatchLearner, MaxNunmberActions
> MultiPipe;

} //  end for namespace choi2013
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_CHOI2013_PIPE_H__
