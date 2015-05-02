#include <iostream>
#include "app/depparser/nn/opt.h"
#include "app/depparser/nn/opt_utils.h"
#include "app/depparser/nn/pipe.h"

#define APP "neural network arcstandard dependency parser"
#define EXE "nndepparser_arcstandard"

namespace nn = ZuoPar::DependencyParser::NeuralNetwork;
namespace po = boost::program_options;

int multi_learn(int argc, char** argv) {
  std::cerr << "not implemented." << std::endl;
  return 0;
}

int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " learn [options]\n";
  usage += "options";

  po::options_description optparser = nn::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  nn::LearnOption opts;
  if (!nn::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  nn::Pipe pipe(opts);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " test [options]\n";
  usage += "options";

  po::options_description optparser = nn::build_test_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  nn::TestOption opts;
  if (!nn::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  nn::Pipe pipe(opts);
  pipe.test();
  return 0;
}

#include "frontend/template/main.h"
#include <cstring>
MAIN(APP, EXE)
