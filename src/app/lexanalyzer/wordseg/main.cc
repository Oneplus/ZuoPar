#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/lexanalyzer/wordseg/opt.h"
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

  po::options_description optparser(usage);
  optparser.add_options()
    ("train,r", po::value<std::string>(), "The path to the reference file.")
    ("devel,d", po::value<std::string>(), "The path to the development data.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("beam", po::value<unsigned>()->default_value(64), "The size for beam.")
    ("algorithm", po::value<std::string>()->default_value("ap"),
    "The learning algorithm.\n"
    " ap - average perceptron\n"
    " pa - passive aggressive")
    ("update", po::value<std::string>()->default_value("early"),
    "Specify the update strategy.\n"
    " naive - no update\n"
    " early - early update (Collins 04)"
    " max - max violation (Huang 12)")
    ("report_stops", po::value<unsigned>()->default_value(1000), "The report frequency.")
    ("evaluate_stops", po::value<unsigned>()->default_value(5000), "The evaluation frequency.")
    ("script", po::value<std::string>(), "The path to the evaluation script.")
    ("output", po::value<std::string>(), "The path to the output file (temporal).")
    ("maxiter", po::value<unsigned>()->default_value(10), "The maximum number of iteration.")
    ("help,h", "Show help information.")
    ("verbose,v", "Logging every detail.");

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
  if (vm.count("help")) { std::cerr << optparser << std::endl; return 1; }

  cws::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::" APP ".\n";
  usage += "usage: " EXE " test [options]\n";
  usage += "options";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("input,i", po::value<std::string>(), "The path to the input file.")
    ("output,o", po::value<std::string>(), "The path to the output file.")
    ("beam,b", po::value<unsigned>()->default_value(64), "The size for beam.")
    ("script", po::value<std::string>(), "The path to the evaluation script.")
    ("verbose,v", "Logging every detail.");
  
  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);
  ZuoPar::Utility::init_boost_log(vm.count("verbose"));
  if (vm.count("help")) { std::cerr << optparser << std::endl; return 1; }

  cws::Pipe pipe(vm);
  pipe.test();
  return 0;
}

#include "frontend/template/main.h"
MAIN(APP, EXE)
