#include "app/semchunker/mono/opt_utils.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

namespace fe = ZuoPar::FrontEnd;
namespace po = boost::program_options;

void _parse_semantic_option(const po::variables_map& vm, SemanticOption& opts) {
  opts.verb_class_path = "";
  if (vm.count("verb-class")) {
    opts.verb_class_path = vm["verb-class"].as<std::string>();
  }

  opts.predicate_tag = vm["predicate"].as<std::string>();
  opts.argument_prefix = vm["prefix"].as<std::string>();
}

bool parse_learn_option_ext(const po::variables_map& vm, LearnOption& opts) {
  if (!fe::parse_learn_option(vm, static_cast<fe::LearnOption&>(opts))) {
    return false;
  }
  _parse_semantic_option(vm, static_cast<SemanticOption&>(opts));
  return true;
}

bool parse_test_option_ext(const po::variables_map& vm, TestOption& opts) {
  if (!fe::parse_test_option(vm, static_cast<fe::TestOption&>(opts))) {
    return false;
  }
  _parse_semantic_option(vm, static_cast<SemanticOption&>(opts));
  opts.output_format = vm["format"].as<std::string>();
  return true;
}

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar
