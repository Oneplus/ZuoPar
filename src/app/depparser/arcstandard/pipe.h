#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__

#include "app/depparser/pipe.h"
#include "app/depparser/multi_pipe.h"
#include "app/depparser/arcstandard/action.h"
#include "app/depparser/arcstandard/action_utils.h"
#include "app/depparser/arcstandard/state.h"
#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/decoder.h"
#include "app/depparser/arcstandard/learner.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

struct MaxNunmberActions {
  int operator ()(const Dependency& instance) {
    return instance.size()* 2- 1;
  }
};

typedef DependencyPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MaxNunmberActions
> Pipe;

typedef DependencyMultiPipe<
  Action, ActionUtils, State, Weight, Decoder, Learner, MinibatchLearner, MaxNunmberActions
> MultiPipe;

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
