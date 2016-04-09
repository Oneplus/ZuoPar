#ifndef __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__

#include <boost/program_options.hpp>

namespace ZuoPar {
namespace DependencyParser {

namespace po = boost::program_options;

po::options_description build_greedy_learn_optparser(const std::string& usage);

po::options_description build_greedy_test_optparser(const std::string& usage);

} //  namespace dependencyparser
} //  namespace zuopar

#endif // end for __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__
