#include "experimental/acl2015/multi_predicate_srl_v2/opt_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
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

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }

  opts.output_format = "semchunks";
  if (vm.count("format")) {
    opts.output_format = vm["format"].as<std::string>();
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

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace frontend
} //  namespace zuopar
