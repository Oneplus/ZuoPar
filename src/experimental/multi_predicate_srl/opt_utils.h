#ifndef __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_UTILS_H__
#define __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "frontend/common_opt_utils.h"
#include "experimental/multi_predicate_srl/opt.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_test_optparser(const std::string& usage);

bool parse_extra_option(const po::variables_map& vm, ExtraOption& opts);

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts);

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_EXPERIMENTAL_MULTI_PREDICATE_SRL_OPT_UTILS_H__
