#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "frontend/common_opt_utils.h"
#include "experimental/hc_search/arceager/opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

bool parse_option_ext(const po::variables_map& vm, fe::Option& opts);

bool parse_learn_option_ext(const po::variables_map& vm, fe::LearnOption& opts);

bool parse_phase_one_learn_option(const po::variables_map& vm, LearnOneOption& opts);

bool parse_phase_two_learn_option(const po::variables_map& vm, LearnTwoOption& opts);

bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_OPT_UTILS_H__
