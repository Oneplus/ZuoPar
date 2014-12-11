#include "app/depparser/arceager/pipe.h"
#include "app/depparser/arceager/learn.h"
#include "app/depparser/arceager/opt_utils.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

int learn(int argc, char** argv) {
  namespace ae = ZuoPar::DependencyParser::ArcEager;

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

  ae::LearnOption opts;
  if (!parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  ae::Pipe pipe(opts);
  pipe.run();
  return 0;
}
