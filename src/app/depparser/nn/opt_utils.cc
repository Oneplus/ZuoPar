#include "app/depparser/nn/opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

namespace fe = ZuoPar::FrontEnd;

std::string SpecialOption::UNKNOWN = "-UNK-";
std::string SpecialOption::NIL     = "-NIL-";
std::string SpecialOption::ROOT    = "-ROOT-";
int SpecialOption::NONEXIST = -1;

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model",     po::value<std::string>(), "The path to the model.")
    ("embedding", po::value<std::string>(), "The path to the embedding file.")
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("development", po::value<std::string>(), "The path to the development file.\n")
    ("threads-number", po::value<int>(), "The number of training threads. [default=1]")
    ("init-range", po::value<double>(), "The initialization range. [default=0.01]")
    ("word-cutoff", po::value<int>(), "The frequency of rare word. [default=1]")
    ("max-iter", po::value<int>(), "The number of max iteration. [default=20000]")
    ("batch-size", po::value<int>(), "The size of batch. [default=10000]")
    ("hidden-size", po::value<int>(), "The size of hidden layer. [default=200]")
    ("embedding-size", po::value<int>(), "The size of embedding. [default=50]")
    ("features-number", po::value<int>(), "The number of features. [default=48]")
    ("precomputed-number", po::value<int>(), "The number of precomputed. [default=100000]")
    ("evaluation-stops", po::value<int>(), "Evaluation on per-iteration. [default=100]")
    ("ada-eps", po::value<double>(), "The EPS in AdaGrad. [defautl=1e-6]")
    ("ada-alpha", po::value<double>(), "The Alpha in AdaGrad. [default=0.01]")
    ("lambda", po::value<double>(), "The regularizer parameter. [default=1e-8]")
    ("dropout-probability", po::value<double>(), "The probability for dropout. [default=0.5]")
    ("save-intermediate", po::value<bool>(), "Save the intermediate. [default=false]")
    ("fix-embeddings", po::value<bool>(), "Fix the embeddings. [default=true]")
    ("verbose", "Logging more details")
    ;
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model", po::value<std::string>(), "The path to the model.")
    ("input", po::value<std::string>(), "The path to the reference.")
    ("embedding", po::value<std::string>(), "The path to the embedding file.")
    ("output", po::value<std::string>(), "The path to the output file.")
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

  if (!vm.count("embedding")) {
    _ERROR << "parse opt: embedding file must be specified [--embedding].";
    return false;
  } else {
    opts.embedding_file = vm["embedding"].as<std::string>();
  }
  return true;
}

bool parse_ada_option(const po::variables_map& vm, AdaOption& opts) {
  opts.ada_eps = 1e-6;
  if (vm.count("ada-eps")) { opts.ada_eps = vm["ada-eps"].as<double>(); }

  opts.ada_alpha = 0.01;
  if (vm.count("ada-alpha")) { opts.ada_alpha = vm["ada-alpha"].as<double>(); }

  opts.lambda = 1e-8;
  if (vm.count("lambda")) {
    opts.lambda = vm["lambda"].as<double>(); }

  opts.dropout_probability = 0.5;
  if (vm.count("dropout-probability")) {
    opts.dropout_probability = vm["dropout-probability"].as<double>(); }
  return true;
}

bool parse_network_option(const po::variables_map& vm, NetworkOption& opts) {
  opts.hidden_layer_size = 200;
  if (vm.count("hidden-size")) { opts.hidden_layer_size = vm["hidden_size"].as<double>(); }

  opts.embedding_size = 50;
  if (vm.count("embedding-size")) { opts.embedding_size = vm["embedding-size"].as<double>(); }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }

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

  if (!parse_ada_option(vm, static_cast<AdaOption&>(opts))) { return false; }
  if (!parse_network_option(vm, static_cast<NetworkOption&>(opts))) { return false; }

  opts.max_iter = 20000;
  if (vm.count("max-iter")) { opts.max_iter = vm["max-iter"].as<double>(); }

  opts.init_range = .01;
  if (vm.count("init-range")) { opts.init_range = vm["init-range"].as<double>(); }

  opts.nr_threads = 1;
  if (vm.count("threads-number")) { opts.nr_threads = vm["threads-number"].as<int>(); }

  opts.word_cutoff = 1;
  if (vm.count("word-cutoff")) { opts.word_cutoff = vm["word-cutoff"].as<int>(); }

  opts.batch_size = 10000;
  if (vm.count("batch-size")) { opts.batch_size = vm["batch-size"].as<int>(); }

  opts.nr_feature_types = 48;
  if (vm.count("features-number")) { opts.nr_feature_types = vm["features-number"].as<int>(); }

  opts.nr_precomputed = 100000;
  if (vm.count("precomputed-number")) {
    opts.nr_precomputed = vm["precomputed-number"].as<int>(); }

  opts.evaluation_stops = 100;
  if (vm.count("evaluation-stops")) { opts.evaluation_stops = vm["evaluation-stops"].as<int>(); }

  opts.clear_gradient_per_iter = 0;
  if (vm.count("clear-gradient-per-iter")) {
    opts.clear_gradient_per_iter = vm["clear-gradient-per-iter"].as<int>(); }

  opts.save_intermediate = true;
  if (vm.count("save-intermediate")) { opts.save_intermediate = vm["save-intermediate"].as<bool>(); }

  opts.fix_embeddings = false;
  if (vm.count("fix-embeddings")) { opts.fix_embeddings = vm["fix-embeddings"].as<bool>(); }
  return true;
}

bool parse_test_option(const po::variables_map& vm, TestOption& opts) {
  if (!parse_basic_option(vm, static_cast<BasicOption&>(opts))) { return false; }
  if (!vm.count("input")) {
    _ERROR << "parse opt: input file must be specified [--input].";
    return false;
  } else {
    opts.input_file = vm["input"].as<std::string>();
  }

  opts.output_file = "";
  if (!vm.count("output")) { opts.output_file = vm["output"].as<std::string>(); }

  return true;
}

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
