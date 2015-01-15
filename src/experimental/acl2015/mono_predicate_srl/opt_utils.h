#ifndef __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_OPT_UTILS_H__
#define __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_OPT_UTILS_H__

#include "frontend/common_opt_utils.h"
#include "experimental/acl2015/mono_predicate_srl/opt.h"
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

namespace po = boost::program_options;

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts);

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_OPT_UTILS_H__
