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

void show_learn_options(const boost::program_options::variables_map& vm);

void show_test_options(const boost::program_options::variables_map& vm);

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_CSTEP_OPT_UTILS_H__
