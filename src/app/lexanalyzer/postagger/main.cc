#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/lexanalyzer/postagger/opt.h"
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

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  fe::LearnOption opts;
  if (!parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  tagger::Pipe pipe(opts);
  pipe.run();
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
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  fe::TestOption opts;
  if (!parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  tagger::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
