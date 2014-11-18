#include "app/depparser/arcstandard/pipe.h"
#include "app/depparser/arcstandard/learn.h"
#include "app/depparser/arcstandard/opt_utils.h"
#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

int learn(int argc, char** argv) {
  namespace as = ZuoPar::DependencyParser::ArcStandard;

  std::string usage = "Training component of ZuoPar::arcstandard dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arcstandard_depparser learn [options]\n";
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

  as::LearnOption opts;
  if (!parse_learn_option(vm, opts)) {
    std::cerr << optparser << std::endl;
    return 1;
  }

  as::Pipe pipe(opts);
  pipe.run();
  return 0;
}
