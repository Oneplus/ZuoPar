#include "app/depparser/nn/chen14/opt_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Chen2014 {

namespace fe = ZuoPar::FrontEnd;

std::string SpecialOption::UNKNOWN = "-UNK-";
std::string SpecialOption::NIL     = "-NIL-";
std::string SpecialOption::ROOT    = "-ROOT-";

po::options_description build_learn_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model",     po::value<std::string>(), "The path to the model.")
    ("embedding", po::value<std::string>(), "The path to the embedding file.")
    ("reference", po::value<std::string>(), "The path to the reference file.")
    ("development", po::value<std::string>(), "The path to the development file.\n")
    ("threads", po::value<int>(), "The number of threads.")
    ("init-range", po::value<double>(), "The initialization range. [default=0.01]")
    ("word-cutoff", po::value<int>(), "The frequency of rare word. [default=1]")
    ("max-iter", po::value<int>(), "The number of max iteration. [default=20000]")
    ("batch-size", po::value<int>(), "The size of batch. [default=10000]")
    ("hidden-size", po::value<int>(), "The size of hidden layer. [default=200]")
    ("embedding-size", po::value<int>(), "The size of embedding. [default=50]")
    ("lambda", po::value<double>(), "The regularizer parameter. [default=1e-8]")
    ("precomputed-number", po::value<int>(), "The number of precomputed. [default=100000]")
    ("evaluation-stops", po::value<int>(), "Evaluation on per-iteration. [default=100]")
    ("evaluation-method", po::value<std::string>(), "Evaluation method:\n"
     " - conllx: exclude unicode punctuation [default]\n"
     " - chen14en: exclude ``'':,. , (require gold POS in conll feat column)\n"
     " - chen14ch: exclude PU, (require gold POS in conll feat colum)")
    ("ada-eps", po::value<double>(), "The 'eps' in AdaGrad. [defautl=1e-6]")
    ("ada-alpha", po::value<double>(), "The 'alpha' in AdaGrad. [default=0.01]")
    ("dropout-probability", po::value<double>(), "The probability for dropout. [default=0.5]")
    ("oracle", po::value<std::string>(),
     "The oracle type\n"
     " - static: The static oracle [default]\n"
     " - nondet: The non-deterministic oracle\n"
     " - explore: The explore oracle.")
    ("save-intermediate", po::value<bool>(), "Save the intermediate. [default=true]")
    ("fix-embeddings", po::value<bool>(), "Fix the embeddings. [default=false]")
    ("use-distance", po::value<bool>(), "Specify to use distance feature. [default=false]")
    ("use-valency", po::value<bool>(), "Specify to use valency feature. [default=false]")
    ("use-cluster", po::value<bool>(), "Specify to use cluster feature. [default=false]")
    ("cluster", po::value<std::string>(), "Specify the path to the cluster file.")
    ("root", po::value<std::string>(), "The root tag, case sensative. [default=ROOT]")
    ("debug", po::value<bool>(), "Perform gradient check in training. [default=false]")
    ("verbose", "Logging more details")
    ("help,h", "Show help information.")
    ;
  return optparser;
}

po::options_description build_test_optparser(const std::string& usage) {
  po::options_description optparser = po::options_description(usage);
  optparser.add_options()
    ("model", po::value<std::string>(), "The path to the model.")
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
  opts.ada_eps = 1e-6;
  if (vm.count("ada-eps")) { opts.ada_eps = vm["ada-eps"].as<double>(); }

  opts.ada_lr = 0.01;
  if (vm.count("ada-alpha")) { opts.ada_lr = vm["ada-alpha"].as<double>(); }

  return true;
}

bool parse_network_option(const po::variables_map& vm, NetworkOption& opts) {
  opts.hidden_layer_size = 200;
  if (vm.count("hidden-size")) { opts.hidden_layer_size = vm["hidden-size"].as<int>(); }

  opts.embedding_size = 50;
  if (vm.count("embedding-size")) { opts.embedding_size = vm["embedding-size"].as<int>(); }

  opts.lambda = 1e-8;
  if (vm.count("lambda")) { opts.lambda = vm["lambda"].as<double>(); }

  return true;
}

bool parse_feature_option(const po::variables_map& vm, FeatureOption& opts) {
  opts.use_distance = false;
  if (vm.count("use-distance")) { opts.use_distance = vm["use-distance"].as<bool>(); }

  opts.use_valency = false;
  if (vm.count("use-valency")) { opts.use_valency = vm["use-valency"].as<bool>(); }

  opts.use_cluster = false;
  if (vm.count("use-cluster")) { opts.use_cluster = vm["use-cluster"].as<bool>(); }

  if (opts.use_cluster) {
    if (vm.count("cluster")) {
      opts.cluster_file = vm["cluster"].as<std::string>();
    } else {
      return false;
    }
  }
  return true;
}

bool parse_learn_option(const po::variables_map& vm, LearnOption& opts) {
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


  if (!parse_ada_option(vm, static_cast<AdaOption&>(opts))) { return false; }
  if (!parse_network_option(vm, static_cast<NetworkOption&>(opts))) { return false; }
  if (!parse_feature_option(vm, static_cast<FeatureOption&>(opts))) { return false; }

  opts.max_iter = 20000;
  if (vm.count("max-iter")) { opts.max_iter = vm["max-iter"].as<int>(); }

  opts.init_range = .01;
  if (vm.count("init-range")) { opts.init_range = vm["init-range"].as<double>(); }

  opts.word_cutoff = 1;
  if (vm.count("word-cutoff")) { opts.word_cutoff = vm["word-cutoff"].as<int>(); }

  opts.batch_size = 10000;
  if (vm.count("batch-size")) { opts.batch_size = vm["batch-size"].as<int>(); }

  opts.nr_threads = 1;
  if (vm.count("threads")) { opts.nr_threads = vm["threads"].as<int>(); }

  opts.nr_precomputed = 100000;
  if (vm.count("precomputed-number")) {
    opts.nr_precomputed = vm["precomputed-number"].as<int>(); }

  opts.evaluation_stops = 100;
  if (vm.count("evaluation-stops")) { opts.evaluation_stops = vm["evaluation-stops"].as<int>(); }

  opts.evaluation_method = "conllx";
  if (vm.count("evaluation-method")) {
    opts.evaluation_method = vm["evaluation-method"].as<std::string>();
    if (opts.evaluation_method != "conllx" &&
        opts.evaluation_method != "chen14en" &&
        opts.evaluation_method != "chen14ch") {
      opts.evaluation_method = "conllx";
    }
  }

  opts.oracle = "static";
  if (vm.count("oracle")) {
    opts.oracle = vm["oracle"].as<std::string>();
    if (opts.oracle != "static" && opts.oracle != "nondet" && opts.oracle != "explore") {
      opts.oracle = "static";
    }
  }

  opts.save_intermediate = true;
  if (vm.count("save-intermediate")) {
    opts.save_intermediate = vm["save-intermediate"].as<bool>();
  }

  opts.dropout_probability = 0.5;
  if (vm.count("dropout-probability")) {
    opts.dropout_probability = vm["dropout-probability"].as<double>(); }

  opts.fix_embeddings = false;
  if (vm.count("fix-embeddings")) { opts.fix_embeddings = vm["fix-embeddings"].as<bool>(); }

  opts.root = "ROOT";
  if (vm.count("root")) { opts.root = vm["root"].as<std::string>(); } 

  opts.debug = false;
  if (vm.count("debug")) { opts.debug = vm["debug"].as<bool>(); }

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
  if (vm.count("output")) { opts.output_file = vm["output"].as<std::string>(); }

  return true;
}

} //  namespace chen2014
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
