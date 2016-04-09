#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/seqlabeler/trans/opt_utils.h"
#include "app/seqlabeler/trans/pipe.h"
#include "app/seqlabeler/trans/multi_pipe.h"

namespace po = boost::program_options;
namespace seq = ZuoPar::SequenceLabeler;
namespace fe = ZuoPar::FrontEnd;

int multi_learn(int argc, char** argv) {
  std::string usage = "Multi-threaded training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler multi-learn [options]\n";
  usage += "OPTIONS";
  po::options_description optparser = fe::build_multi_learn_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");

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
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_multi_learn_options(vm);
  seq::MultiPipe pipe(vm);
  pipe.learn();
  return 0;
}

int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()("constrain", po::value<std::string>(), "The constrain file.");

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
  if (!ZuoPar::FrontEnd::check_required_options(vm, { "train", "script" })) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ZuoPar::FrontEnd::show_learn_options(vm);
  seq::Pipe pipe(vm);
  pipe.learn();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()("constrain", po::value<std::string>(), "The constrain file.");

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
  seq::Pipe pipe(vm);
  pipe.test();
  return 0;
}

#include "frontend/template/main.h"
MAIN("sequence labeler", "sequence_labeler")

