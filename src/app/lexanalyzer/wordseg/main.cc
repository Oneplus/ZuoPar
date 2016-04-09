#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/lexanalyzer/wordseg/opt_utils.h"
#include "app/lexanalyzer/wordseg/pipe.h"

#define APP "transition-based Chinese word segmentor"
#define EXE "wordseg"

namespace po = boost::program_options;
namespace cws = ZuoPar::LexicalAnalyzer::ChineseWordSegmentor;
namespace fe = ZuoPar::FrontEnd;

int multi_learn(int argc, char** argv) {
  std::cerr << "Not implemented." << std::endl;
  return 1;
}

int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " learn [options]\n";
  usage += "options";

  po::options_description optparser = fe::build_learn_optparser(usage);
  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }

  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, {"train", "script"})) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_learn_options(vm);
  cws::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " test [options]\n";
  usage += "options";

  po::options_description optparser = fe::build_test_optparser(usage);
  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "input", "model", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_test_options(vm);
  cws::Pipe pipe(vm);
  pipe.test();
  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
