#include "app/depparser/arceager/test.h"
#include "app/depparser/arceager/pipe.h"
#include "app/depparser/arceager/opt_utils.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

int test(int argc, char** argv) {
  namespace ae = ZuoPar::DependencyParser::ArcEager;
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

  ae::TestOption opts;
  if (!parse_test_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }
  ae::Pipe pipe(opts);
  pipe.run();

  return 0;
}
