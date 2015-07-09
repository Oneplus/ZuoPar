#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "experimental/hc_search/cstep_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_test_optparser(const std::string& usage);

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__
