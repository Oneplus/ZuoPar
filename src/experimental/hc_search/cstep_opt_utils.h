#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__

#include <boost/program_options.hpp>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_test_optparser(const std::string& usage);

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__
