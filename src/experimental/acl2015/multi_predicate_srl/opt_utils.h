#ifndef __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_OPT_UTILS_H__
#define __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "frontend/common_opt_utils.h"
#include "experimental/acl2015/multi_predicate_srl_v2/opt.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace po = boost::program_options;

bool parse_extra_option(const po::variables_map& vm, ExtraOption& opts);

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts);

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_OPT_UTILS_H__
