#ifndef __ZUOPAR_FRONTEND_COMMON_OPT_UTILS_H__
#define __ZUOPAR_FRONTEND_COMMON_OPT_UTILS_H__

#include "common_opt.h"
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace FrontEnd {

namespace po = boost::program_options;

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
