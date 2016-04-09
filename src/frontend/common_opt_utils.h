#ifndef __ZUOPAR_FRONTEND_COMMON_OPT_UTILS_H__
#define __ZUOPAR_FRONTEND_COMMON_OPT_UTILS_H__

#include "common_opt.h"
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace FrontEnd {

namespace po = boost::program_options;

/**
 * Build commonly used learn option parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_learn_optparser(const std::string& usage);

/**
 * Build commonly used multi learn option parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_multi_learn_optparser(const std::string& usage);

/**
 * Build commonly used test option parser.
 *
 *  @param[in]  usage   The usage.
 *  @return     parser  The option parser
 */
po::options_description build_test_optparser(const std::string& usage);

/**
 * Check if the required options are provided.
 *
 *
 *  @param[in]  vm        The options.
 *  @param[in]  required  The names of the required options.
 *  @return     bool      Return true if all the required options are set.
 */
bool check_required_options(const boost::program_options::variables_map& vm,
  const std::vector<std::string>& required);

void show_learn_options(const boost::program_options::variables_map& vm);

void show_multi_learn_options(const boost::program_options::variables_map& vm);

void show_test_options(const boost::program_options::variables_map& vm);

/**
 * Parse commonly shared options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_option(const po::variables_map& vm, Option& opts);

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

} //  namespace frontend
} //  namespace zuopar

#endif  //  end for __ZUOPAR_FRONTEND_COMMON_OPT_UTILS_H__
