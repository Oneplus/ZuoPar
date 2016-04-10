#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__

#include <boost/program_options.hpp>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_prepare_optparser(const std::string& usage);

po::options_description build_evaluate_optparser(const std::string& usage);

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_OPT_UTILS_H__
