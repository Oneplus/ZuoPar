#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_OPT_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_OPT_UTILS_H__

#include "opt.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

namespace po = boost::program_options;
namespace as = ZuoPar::DependencyParser::ArcStandard;

/**
 * Parse commonly shared options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_option(const po::variables_map& vm, as::Option& opts);

/**
 * Parse the learning options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_learn_option(const po::variables_map& vm, as::LearnOption& opts);

/**
 * Parse the testing options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_test_option(const po::variables_map& vm, as::TestOption& opts);

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_OPT_UTILS_H__
