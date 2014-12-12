#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_OPT_UTILS_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_OPT_UTILS_H__

#include "opt.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

namespace po = boost::program_options;
namespace tagger = ZuoPar::LexicalAnalyzer::Postagger;

/**
 * Parse commonly shared options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_option(const po::variables_map& vm, tagger::Option& opts);

/**
 * Parse the learning options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_learn_option(const po::variables_map& vm, tagger::LearnOption& opts);

/**
 * Parse the testing options.
 *
 *  @param[in]    vm    The variables map
 *  @param[out]   opts  The options.
 */
bool parse_test_option(const po::variables_map& vm, tagger::TestOption& opts);

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_OPT_UTILS_H__
