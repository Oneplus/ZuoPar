#include <iostream>
#include <cstring>  // strcmp
#include <boost/program_options.hpp>
#include "utils/logging.h"
#include "experimental/hc_search/arceager/opt.h"
#include "experimental/hc_search/arceager/opt_utils.h"
#include "experimental/hc_search/arceager/pipe.h"

#define __EXE__ "experimental/hc_arceager_depparser"

namespace fe = ZuoPar::FrontEnd;
namespace hc = ZuoPar::Experimental::HCSearchDependencyParser;
namespace po = boost::program_options;

/**
 * Perform the learning process of ZuoPar::arceager dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running learning.
 */
int learn_phase_one(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::HC-search arceager depparser phase(1).\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: " __EXE__ " learn-phase-one [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b", po::value<int>(), "The size for beam [default=64].")
    ("display,d", po::value<int>(), "The display interval [default=1000].")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s", po::value<int>(), "The flag for shuffling instance.\n"
                                    " 0  - not shuffle instance [default].\n"
                                    " >0 - perform s time shuffle (avoid fake shuffle.)")
    ("update,u", po::value<std::string>(), "Specify the early update strategy.\n"
                                           " naive - no update\n"
                                           " early - early update (Collins 04) [default]\n"
                                           " max - max violation (Huang 12)")
    ("verbose,v", "Logging every detail.")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ;

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  hc::LearnOneOption opts;
  if (!hc::parse_phase_one_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  hc::Pipe pipe(opts);
  pipe.run();
  return 0;
}

int learn_phase_two(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::HC-search arceager depparser phase(2).\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: " __EXE__ " learn-phase-two [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h", "Show help information.")
    ("algorithm,a", po::value<std::string>(), "The learning algorithm.\n"
                                              " ap - average perceptron [default]\n"
                                              " pa - passive aggressive")
    ("beam,b", po::value<int>(), "The size for beam [default=64].")
    ("display,d", po::value<int>(), "The display interval [default=1000].")
    ("reference,r", po::value<std::string>(), "The path to the reference file.")
    ("shuffle,s", po::value<int>(), "The flag for shuffling instance.\n"
                                    " 0  - not shuffle instance [default].\n"
                                    " >0 - perform s time shuffle (avoid fake shuffle.)")
    ("verbose,v", "Logging every detail.")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("phase-two-model", po::value<std::string>(), "The path to the phase two model.")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  hc::LearnTwoOption opts;
  if (!hc::parse_phase_two_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  hc::Pipe pipe(opts);
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
  std::string usage = "Testing component of ZuoPar::HC-search arceager dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: " __EXE__ "test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser(usage);
  optparser.add_options()
    ("help,h",                              "Show help information.")
    ("input,i",   po::value<std::string>(), "The path to the input file.")
    ("output,o",  po::value<std::string>(), "The path to the output file.")
    ("display,d", po::value<int>(),         "The display interval [default=1000].")
    ("beam,b",    po::value<int>(),         "The size for beam [default=64].")
    ("phase-one-model", po::value<std::string>(), "The path to the phase one model.")
    ("phase-two-model", po::value<std::string>(), "The path to the phase two model.")
    ("verbose,v",                           "Logging every detail.")
    ;

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  hc::TestOption opts;
  if (!hc::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  hc::Pipe pipe(opts);
  pipe.run();

  return 0;
}

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::HC-search arceager dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: " __EXE__;
  usage += " [learn-phase-one|learn-phase-two|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn-phase-one") == 0) {
    learn_phase_one(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "learn-phase-two") == 0) {
    learn_phase_two(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    test(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  return 0;
}
