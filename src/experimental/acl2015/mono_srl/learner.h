#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__

#include "system/structure_learner.h"
#include "experimental/acl2015/mono_srl/action.h"
#include "experimental/acl2015/mono_srl/state.h"
#include "experimental/acl2015/mono_srl/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoSRL {

typedef TransitionStructureLearner<Action, State, Weight> Learner;

} //  namespace monosrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__
