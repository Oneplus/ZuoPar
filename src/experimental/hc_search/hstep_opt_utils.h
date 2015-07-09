#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "experimental/hc_search/hstep_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_prepare_optparser(const std::string& usage);

po::options_description build_evaluate_optparser(const std::string& usage);

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

bool parse_prepare_option(const po::variables_map& vm, PrepareOption& opts);

bool parse_evaluate_option(const po::variables_map& vm, EvaluateOption& opts);

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__
