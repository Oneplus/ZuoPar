#include <iostream>
#include <cstring>  // strcmp
#include "opt.h"
#include "utils/logging.h"
#include "app/lexanalyzer/postagger/opt.h"
#include "app/lexanalyzer/postagger/pipe.h"
#include "app/lexanalyzer/postagger/opt_utils.h"

namespace po = boost::program_options;
namespace tagger = ZuoPar::LexicalAnalyzer::Postagger;
namespace fe = ZuoPar::FrontEnd;

int multi_learn(int argc, char** argv) {
  std::cerr << "Not implemented." << std::endl;
}

/**
 * Perform the learning process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running learning.
 */
int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::transition-based postagger.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: postagger learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("verbose,v", "Logging every detail.")
  ;

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

/**
 * Perform the testing process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running test.
 */
int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::transition-based postagger.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: postagger test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("input,i", po::value<std::string>(), "The path to the input file.")
    ("output,o", po::value<std::string>(), "The path to the output file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("verbose,v", "Logging every detail.")
  ;

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
MAIN("transition-based postagger.", "postagger")
