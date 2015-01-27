#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__

#include "system/structure_learner.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"
#include "experimental/acl2015/multi_predicate_srl1/state.h"
#include "experimental/acl2015/multi_predicate_srl1/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

typedef TransitionStructureLearner<ActionCollection, State, Weight> Learner;

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_SRL_LEARNER_H__
