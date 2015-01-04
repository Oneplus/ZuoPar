#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_OPT_UTILS_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_OPT_UTILS_H__

#include "frontend/common_opt_utils.h"
#include "app/seqlabeler/opt.h"

namespace ZuoPar {
namespace SequenceLabeler {

bool parse_constrain(const po::variables_map& vm, ConstrainOption& opts);

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

bool parse_multi_learn_option(const po::variables_map& vm, MultiLearnOption& opts);

bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_OPT_UTILS_H__
