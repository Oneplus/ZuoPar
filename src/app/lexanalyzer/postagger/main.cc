#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
// #include "app/lexanalyzer/postagger/opt.h"
#include "app/lexanalyzer/postagger/pipe.h"
#include "app/lexanalyzer/postagger/opt_utils.h"

#define APP "transition-based postagger"
#define EXE "postagger"

namespace po = boost::program_options;
namespace tagger = ZuoPar::LexicalAnalyzer::Postagger;
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
  optparser.add_options()
    ("delimiter", po::value<char>()->default_value('/'), "The delimiter");

  if (argc == 1) { std::cerr << optparser << std::endl; return 1; }

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, optparser), vm);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << optparser << std::endl;
    return 1;
  }

  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) { std::cerr << optparser << std::endl; return 1; }

  tagger::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " test [options]\n";
  usage += "options";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()
    ("script", po::value<std::string>(), "The path to the evaluation script.");

  if (argc == 1) { std::cerr << optparser << std::endl; return 1; }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) { std::cerr << optparser << std::endl; return 1; }

  tagger::Pipe pipe(vm);
  pipe.test();
  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
