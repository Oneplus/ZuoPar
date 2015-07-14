#ifndef __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__

#include <boost/program_options.hpp>
#include "app/depparser/greedy_opt.h"

namespace ZuoPar {
namespace DependencyParser {

namespace po = boost::program_options;

po::options_description build_greedy_learn_optparser(const std::string& usage);

po::options_description build_greedy_test_optparser(const std::string& usage);

bool parse_greedy_learn_option(const po::variables_map& vm, GreedyLearnOption& opts);

bool parse_greedy_test_option(const po::variables_map& vm, GreedyTestOption& opts);

} //  namespace dependencyparser
} //  namespace zuopar

#endif // end for __ZUOPAR_APP_DEPPARSER_GREEDY_OPT_UTILS_H__
