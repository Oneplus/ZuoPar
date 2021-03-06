#include "utils/logging.h"
#include "app/depparser/nn/weiss15/opt_utils.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

namespace fe = ZuoPar::FrontEnd;

std::string SpecialOption::UNKNOWN = "-UNK-";
std::string SpecialOption::NIL     = "-NIL-";
std::string SpecialOption::ROOT    = "-ROOT-";

po::options_description build_pretrain_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model",     po::value<std::string>(), "The path to the model.")
    ("embedding", po::value<std::string>(), "The path to the embedding file.")
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("development", po::value<std::string>(), "The path to the development file.\n")
    ("algorithm", po::value<std::string>()->default_value("adagrad"), "The learning algorithm.")
    ("threads", po::value<int>()->default_value(1), "The number of threads [default=1].")
    ("init-range", po::value<double>()->default_value(0.01), "The initialization range. [default=0.01]")
    ("word-cutoff", po::value<int>()->default_value(1), "The frequency of rare word. [default=1]")
    ("max-iter", po::value<int>()->default_value(20000), "The number of max iteration. [default=20000]")
    ("batch-size", po::value<int>()->default_value(10000), "The size of batch. [default=10000]")
    ("hidden-size", po::value<int>()->default_value(200), "The size of hidden layer. [default=200]")
    ("embedding-size", po::value<int>()->default_value(50), "The size of embedding. [default=50]")
    ("lambda", po::value<double>()->default_value(1e-8), "The regularizer parameter. [default=1e-8]")
    ("precomputed-number", po::value<int>()->default_value(100000), "The number of precomputed. [default=100000]")
    ("evaluation-stops", po::value<int>()->default_value(100), "Evaluation on per-iteration. [default=100]")
    ("evaluation-method", po::value<std::string>()->default_value("conllx"), "Evaluation method:\n"
     " - conllx: exclude unicode punctuation [default]\n"
     " - chen14en: exclude ``'':,. , (require gold POS in conll feat column)\n"
     " - chen14ch: exclude PU, (require gold POS in conll feat colum)")
    ("ada-eps", po::value<double>()->default_value(1e-6), "The 'eps' in AdaGrad. [default=1e-6]")
    ("ada-alpha", po::value<double>()->default_value(0.01), "The 'alpha' in AdaGrad. [default=0.01]")
    ("dropout-probability", po::value<double>()->default_value(0.5), "The probability for dropout. [default=0.5]")
    ("oracle", po::value<std::string>()->default_value("static"),
     "The oracle type\n"
     " - static: The static oracle [default]\n"
     " - nondet: The non-deterministic oracle\n"
     " - explore: The explore oracle.")
    ("save-intermediate", po::value<bool>()->default_value(true), "Save the intermediate. [default=true]")
    ("fix-embeddings", po::value<bool>()->default_value(false), "Fix the embeddings. [default=false]")
    ("root", po::value<std::string>()->default_value("ROOT"), "The root tag, case sensative. [default=ROOT]")
    ("debug", po::value<bool>()->default_value(false), "Perform gradient check in training. [default=false]")
    ("verbose", "Logging more details")
    ("help,h", "Show help information.")
    ;
  return optparser;
}

po::options_description build_pretest_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model", po::value<std::string>(), "The path to the model.")
    ("input", po::value<std::string>(), "The path to the reference.")
    ("output", po::value<std::string>(), "The path to the output file.")
    ("help,h", "Show help information.")
    ;
  return optparser;
}

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("pretrain-model", po::value<std::string>(), "The path to the pretrained model.")
    ("model", po::value<std::string>(), "The path to the model.")
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("development", po::value<std::string>(), "The path to the development file.")
    ("algorithm", po::value<std::string>()->default_value("ap"), "The learning algorithm.\n"
     " - pa: passive aggressive\n"
     " - ap: averaged perceptron [default]")
    ("beam-size", po::value<int>()->default_value(1), "The beam-size [default=1]")
    ("max-iter", po::value<int>()->default_value(10), "The maximum iteration [default=10].")
    ("evaluation-method", po::value<std::string>()->default_value("conllx"), "Evaluation method:\n"
     " - conllx: exclude unicode punctuation [default]\n"
     " - chen14en: exclude ``'':,. , (require gold POS in conll feat column)\n"
     " - chen14ch: exclude PU, (require gold POS in conll feat colum)")
    ("avg", po::value<bool>()->default_value(false), "Use averaged parameter [default=false]")
    ("help,h", "Show help information.")
    ;
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("pretrain-model", po::value<std::string>(), "The path to the pretrained model.")
    ("model", po::value<std::string>(), "The path to the model.")
    ("beam-size", po::value<int>()->default_value(1), "The beam-size [default=1]")
    ("input", po::value<std::string>(), "The path to the reference.")
    ("output", po::value<std::string>(), "The path to the output file.")
    ("help,h", "Show help information.")
    ;
  return optparser;
}

bool parse_basic_option(const po::variables_map& vm, BasicOption& opts) {
  namespace utils = ZuoPar::Utility;
  utils::init_boost_log(vm.count("verbose"));

  if (vm.count("help")) { return false; }

  if (!vm.count("model")) {
    _ERROR << "parse opt: model file must be specified [--model].";
    return false;
  } else {
    opts.model_file = vm["model"].as<std::string>();
  }

  return true;
}

bool parse_ada_option(const po::variables_map& vm, AdaOption& opts) {
  opts.ada_eps = vm["ada-eps"].as<double>();
  opts.ada_lr = vm["ada-alpha"].as<double>();

  return true;
}

bool parse_asgd_option(const po::variables_map& vm, MomentumASGDOption& opts) {
  opts.momentum_mu = 0.9;
  opts.momentum_lr = 0.05;
  opts.momentum_gamma = 0.2;
  return true;
}

bool parse_network_option(const po::variables_map& vm, NetworkOption& opts) {
  opts.hidden_layer_size = vm["hidden-size"].as<int>();
  opts.embedding_size = vm["embedding-size"].as<int>();
  opts.lambda = vm["lambda"].as<double>();

  return true;
}

bool parse_pretrain_option(const po::variables_map& vm, PretrainOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }

  if (!vm.count("embedding")) {
    _ERROR << "parse opt: embedding file must be specified [--embedding].";
    return false;
  } else {
    opts.embedding_file = vm["embedding"].as<std::string>();
  }

  if (!vm.count("reference")) {
    _ERROR << "parse opt: reference file must be specified [--reference].";
    return false;
  } else {
    opts.reference_file = vm["reference"].as<std::string>();
  }

  opts.devel_file = "";
  if (vm.count("development")) {
    opts.devel_file = vm["development"].as<std::string>();
  }

  opts.algorithm = vm["algorithm"].as<std::string>();
  if (opts.algorithm != "adagrad" && opts.algorithm != "asgd") {
    opts.algorithm = "adagrad";
  }

  if (opts.algorithm == "adagrad") {
    if (!parse_ada_option(vm, static_cast<AdaOption&>(opts))) { return false; }
  } else if (opts.algorithm == "asgd") {
    if (!parse_asgd_option(vm, static_cast<MomentumASGDOption&>(opts))) { return false; }
  }
  if (!parse_network_option(vm, static_cast<NetworkOption&>(opts))) { return false; }

  opts.max_iter = vm["max-iter"].as<int>();
  opts.init_range = vm["init-range"].as<double>();
  opts.word_cutoff = vm["word-cutoff"].as<int>();
  opts.batch_size = vm["batch-size"].as<int>();
  opts.nr_threads = vm["threads"].as<int>();
  opts.nr_precomputed = vm["precomputed-number"].as<int>();
  opts.evaluation_stops = vm["evaluation-stops"].as<int>();

  opts.evaluation_method = vm["evaluation-method"].as<std::string>();
  if (opts.evaluation_method != "conllx" &&
      opts.evaluation_method != "chen14en" &&
      opts.evaluation_method != "chen14ch") {
    opts.evaluation_method = "conllx";
  }

  opts.oracle = vm["oracle"].as<std::string>();
  if (opts.oracle != "static" && opts.oracle != "nondet" && opts.oracle != "explore") {
    opts.oracle = "static";
  }

  opts.save_intermediate = vm["save-intermediate"].as<bool>();
  opts.dropout_probability = vm["dropout-probability"].as<double>();
  opts.fix_embeddings = vm["fix-embeddings"].as<bool>();
  opts.root = vm["root"].as<std::string>();
  opts.debug = vm["debug"].as<bool>();

  return true;
}

bool parse_pretrain_model_option(const po::variables_map& vm, PretrainModelOption& opts) {
  if (!vm.count("pretrain-model")) {
    _ERROR << "parse opt: pretrained model must be specified [--pretrain-model]";
    return false;
  } else {
    opts.pretrain_model_file = vm["pretrain-model"].as<std::string>();
  }

  return true;
}

bool parse_beam_size_option(const po::variables_map& vm, BeamsizeOption& opts) {
  opts.beam_size = vm["beam-size"].as<int>();
  if (opts.beam_size <= 0) { opts.beam_size = 1; }

  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }
  if (!parse_pretrain_model_option(vm, static_cast<PretrainModelOption&>(opts))) {
    return false;
  }
  if (!vm.count("reference")) {
    _ERROR << "parse opt: reference file must be specified [--reference].";
    return false;
  } else {
    opts.reference_file = vm["reference"].as<std::string>();
  }

  opts.devel_file = "";
  if (vm.count("development")) {
    opts.devel_file = vm["development"].as<std::string>();
  }

  opts.max_iter = vm["max-iter"].as<int>();

  opts.algorithm = vm["algorithm"].as<std::string>();
  if (opts.algorithm != "pa" && opts.algorithm != "ap") {
    opts.algorithm = "ap";
  }

  opts.averaged = vm["avg"].as<bool>();

  opts.evaluation_method = vm["evaluation-method"].as<std::string>();
  if (opts.evaluation_method != "conllx" &&
      opts.evaluation_method != "chen14en" &&
      opts.evaluation_method != "chen14ch") {
    opts.evaluation_method = "conllx";
  }
  
  parse_beam_size_option(vm, static_cast<BeamsizeOption&>(opts));
  return true;
}

bool parse_pretest_option(const po::variables_map& vm, PretestOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }
  if (!vm.count("input")) {
    _ERROR << "parse opt: input file must be specified [--input].";
    return false;
  } else {
    opts.input_file = vm["input"].as<std::string>();
  }

  opts.output_file = "";
  if (vm.count("output")) { opts.output_file = vm["output"].as<std::string>(); }

  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }

  if (!parse_pretrain_model_option(vm, static_cast<PretrainModelOption&>(opts))) {
    return false;
  }

  if (!vm.count("pretrain-model")) {
    _ERROR << "parse opt: pretrained model must be specified [--pretrain-model]";
    return false;
  } else {
    opts.pretrain_model_file = vm["pretrain-model"].as<std::string>();
  }

  if (!vm.count("input")) {
    _ERROR << "parse opt: input file must be specified [--input].";
    return false;
  } else {
    opts.input_file = vm["input"].as<std::string>();
  }

  opts.output_file = "";
  if (vm.count("output")) { opts.output_file = vm["output"].as<std::string>(); }

  parse_beam_size_option(vm, static_cast<BeamsizeOption&>(opts));
  return true;
}


} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
