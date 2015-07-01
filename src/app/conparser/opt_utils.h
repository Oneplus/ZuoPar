#ifndef __ZUOPAR_APP_CONPARSER_OPT_UTILS_H__
#define __ZUOPAR_APP_CONPARSER_OPT_UTILS_H__

#include "frontend/common_opt_utils.h"
#include "app/conparser/opt.h"

namespace ZuoPar {
namespace ConstituentParser {

namespace po = boost::program_options;

po::options_description build_multi_learn_optparser(const std::string& usage);

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_test_optparser(const std::string& usage);

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts);

bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace constitunetparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_CONPARSER_OPT_UTILS_H__
