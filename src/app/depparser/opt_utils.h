#ifndef __ZUOPAR_APP_DEPPARSER_OPT_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_OPT_UTILS_H__

#include "frontend/common_opt_utils.h"
#include "app/depparser/opt.h"

namespace ZuoPar {
namespace DependencyParser {

namespace po = boost::program_options;

/**
 * Build multi-learn option parser for dependency parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_multi_learn_optparser(const std::string& usage);

/**
 * Build learn option parser for dependency parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_learn_optparser(const std::string& usage);

/**
 * Build test option parser for dependency parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_test_optparser(const std::string& usage);

/**
 * Parse commonly shared options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_root_option(const po::variables_map& vm, RootOption& opts);

/**
 * Parse the learning options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

/**
 * Parse the learning options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts);

/**
 * Parse the testing options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_OPT_UTILS_H__
