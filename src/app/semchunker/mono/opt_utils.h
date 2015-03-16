#ifndef __ZUOPAR_APP_SEMCHUNKER_MONO_OPT_UTILS_H__
#define __ZUOPAR_APP_SEMCHUNKER_MONO_OPT_UTILS_H__

#include "frontend/common_opt_utils.h"
#include "app/semchunker/mono/opt.h"
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace po = boost::program_options;

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts);

} //  namespace monopredicate
} //  namespace semchunker
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEMCHUNKER_MONO_OPT_UTILS_H__
