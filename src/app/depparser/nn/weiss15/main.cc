#include <iostream>
#include <cstring>
#include "app/depparser/nn/weiss15/opt.h"
#include "app/depparser/nn/weiss15/opt_utils.h"
#include "app/depparser/nn/weiss15/pipe.h"

#define APP "Weiss 2015's parser"
#define EXE "nndepparser_weiss15"

namespace nn = ZuoPar::DependencyParser::NeuralNetwork::Weiss2015;
namespace po = boost::program_options;

int pretrain(int argc, char** argv) {
  std::string usage = "Pretraining component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " pre-train [options]\n";
  usage += "options";

  po::options_description optparser = nn::build_pretrain_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  nn::PretrainOption opts;
  if (!nn::parse_pretrain_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  nn::Pipe pipe(opts);
  pipe.pretrain();

  return 0;
}

int learn(int argc, char** argv) {
  /*std::string usage = "Training component of ZuoPar::" APP ".\n";
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
  return 0;*/
}

int test(int argc, char** argv) {
  /*std::string usage = "Testing component of ZuoPar::" APP ".\n";
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
  return 0;*/
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::" APP ".\n";
  usage += "Usage: " EXE " [pre-train|learn|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "pre-train") == 0) {
    pretrain(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "learn") == 0) {
    learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    test(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  return 0;
}
