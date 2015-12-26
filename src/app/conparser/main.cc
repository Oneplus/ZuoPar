#include <iostream>
#include <cstring>
#include "app/conparser/opt.h"
#include "app/conparser/opt_utils.h"

#define APP "constituent parser"
#define EXE "conparser"

namespace cp = ZuoPar::ConstituentParser;
namespace po = boost::program_options;

int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " multi-learn [options]\n";
  usage += "options";
  return 0;
}

int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " learn [options]\n";
  usage += "options";
  po::options_description optparser = cp::build_learn_optparser(usage);

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  cp::LearnOption opts;
  if (!cp::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
