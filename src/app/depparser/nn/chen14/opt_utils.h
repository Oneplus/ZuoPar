#ifndef __ZUOPAR_APP_DEPPARSER_NN_CHEN14_OPT_UTILS_H__
#define __ZUOPAR_APP_DEPPARSER_NN_CHEN14_OPT_UTILS_H__

#include "app/depparser/nn/chen14/opt.h"
#include "frontend/common_opt_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Chen2014 {

namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage);

po::options_description build_test_optparser(const std::string& usage);

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts);

bool parse_test_option(const po::variables_map& vm, TestOption& opts);

} //  namespace chen2014
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_CHEN14_OPT_UTILS_H__
