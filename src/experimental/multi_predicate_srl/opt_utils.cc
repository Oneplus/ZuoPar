#include "experimental/multi_predicate_srl/opt_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = fe::build_learn_optparser(usage);
  optparser.add_options()
    ("cube-height", po::value<int>(), "The height of cube for cube-proning[default=8]")
    ("verb-class", po::value<std::string>(), "The path to the verb class.")
    ("predicate", po::value<std::string>(), "The predicate tag [default=\"V\"].")
    ("prefix", po::value<std::string>(), "The argument prefix [default=\"A\"].");

  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = fe::build_test_optparser(usage);
  optparser.add_options()
    ("cube-height", po::value<int>(), "The height of cube for cube-proning[default=8]")
    ("verb-class", po::value<std::string>(), "The path to the verb class.")
    ("predicate", po::value<std::string>(), "The predicate tag [default=\"V\"].")
    ("prefix", po::value<std::string>(), "The argument prefix [default=\"A\"].")
    ("format", po::value<std::string>(), "The output format\n"
                                         " - semchunk: the semantic chunk [default]\n"
                                         " - conll: the conll props.");

  return optparser;
}


bool parse_extra_option(const po::variables_map& vm, ExtraOption& opts) {
  opts.cube_height = 8;
  if (vm.count("cube-height")) {
    opts.cube_height = vm["cube-height"].as<int>();
  }

  opts.verb_class_path = "";
  if (vm.count("verb-class")) {
    opts.verb_class_path = vm["verb-class"].as<std::string>();
  }

  opts.predicate_tag = "V";
  if (vm.count("predicate")) {
    opts.predicate_tag = vm["predicate"].as<std::string>();
  }

  opts.argument_prefix = "A";
  if (vm.count("prefix")) {
    opts.argument_prefix = vm["prefix"].as<std::string>();
  }

  return true;
}

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
  }
  if (!parse_extra_option(vm, static_cast<ExtraOption&>(opts))) {
    return false;
  }
  return true;
}

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }
  if (!parse_extra_option(vm, static_cast<ExtraOption&>(opts))) {
    return false;
  }

  opts.output_format = "semchunks";
  if (vm.count("format")) {
    opts.output_format = vm["format"].as<std::string>();
  }

  return true;
}

} //  namespace multipredicatesrl
} //  namespace frontend
} //  namespace zuopar
