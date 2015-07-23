#include <iostream>
#include <cstring>  // strcmp
#include "utils/logging.h"
#include "app/seqlabeler/trans/opt.h"
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
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::MultiLearnOption opts;
  if (!seq::parse_multi_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  seq::MultiPipe pipe(opts);
  pipe.run();
  return 0;
}

int learn(int argc, char** argv) {
  std::string usage = "Training component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler learn [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::LearnOption opts;
  if (!seq::parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  seq::Pipe pipe(opts);
  pipe.run();
  return 0;
}

int test(int argc, char** argv) {
  std::string usage = "Testing component of ZuoPar::sequence labeler.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: sequence_labeler test [options]\n";
  usage += "OPTIONS";

  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()("constrain,n", po::value<std::string>(), "The constrain file.");

  if (argc == 1) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, optparser), vm);

  seq::TestOption opts;
  if (!seq::parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  seq::Pipe pipe(opts);
  pipe.run();

  return 0;
}

#include "frontend/template/main.h"
MAIN("sequence labeler", "sequence_labeler")

