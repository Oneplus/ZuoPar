#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "app/depparser/arceager/opt.h"
#include "app/depparser/arceager/opt_utils.h"
#include "app/depparser/arceager/pipe.h"

namespace fe = ZuoPar::FrontEnd;
namespace ae = ZuoPar::DependencyParser::ArcEager;
namespace po = boost::program_options;

/**
 * Perform the learning process of ZuoPar::arceager dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running learning.
 */
int learn(int argc, char** argv) {

  std::string usage = "Training component of ZuoPar::arceager dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arceager_depparser learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("conll,c", "Specified the input in CoNLL format.")
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

  ae::Pipe pipe(opts);
  pipe.run();
  return 0;
}

/**
 * Perform the testing process of ZuoPar::arceager dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running test.
 */
int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::arceager dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arceager_depparser test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("model,m", po::value<std::string>(), "The path to the model.")
    ("input,i", po::value<std::string>(), "The path to the input file.")
    ("output,o", po::value<std::string>(), "The path to the output file.")
    ("display,d", po::value<int>(), "The display interval.")
    ("beam,b", po::value<int>(), "The size for beam.")
    ("conll,c", "Specified the input in CoNLL format.")
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
  ae::Pipe pipe(opts);
  pipe.run();

  return 0;
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::arceager dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arceager_depparser [learn|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
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
