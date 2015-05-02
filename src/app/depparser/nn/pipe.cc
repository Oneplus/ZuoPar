#include <fstream>
#include <algorithm>
#include "app/depparser/nn/pipe.h"
#include "app/depparser/nn/action.h"
#include "app/depparser/nn/action_utils.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

Pipe::Pipe(const LearnOption& opts)
  : mode(kPipeLearn), learn_opt(&opts), test_opt(nullptr),
  forms_alphabet(false),
  lemmas_alphabet(false),
  cpostags_alphabet(false),
  postags_alphabet(false),
  feats_alphabet(false),
  deprels_alphabet(false) {
  reference_file = opts.reference_file;
  devel_file = opts.devel_file;
  model_file = opts.model_file;
  embedding_file = opts.embedding_file;

  _INFO << "LEARN:: mode is activated.";
  _INFO << "report: reference file: " << reference_file;
  _INFO << "report: development file: " << devel_file;
  _INFO << "report: model file: " << model_file;
  _INFO << "report: embedding file: " << embedding_file;

  display_learning_parameters();
}

Pipe::Pipe(const TestOption& opts)
  : mode(kPipeTest), learn_opt(nullptr), test_opt(&opts),
  forms_alphabet(false),
  lemmas_alphabet(false),
  cpostags_alphabet(false),
  postags_alphabet(false),
  feats_alphabet(false),
  deprels_alphabet(false) {
  input_file = opts.input_file;
  output_file = opts.output_file;
  model_file = opts.model_file;
  embedding_file = opts.embedding_file;

  _INFO << "TEST:: mode is activated.";
  _INFO << "report: input file: " << input_file;
  _INFO << "report: output file: " << output_file;
  _INFO << "report: model file: " << model_file;
  _INFO << "report: embedding file: " << embedding_file;
}

void Pipe::display_learning_parameters() {
  if (learn_opt == nullptr) { _WARN << "pipe: learning parameter is not set."; return; }

  const LearnOption& _ = (*learn_opt);
  _INFO << "report: (Misc) training threads = " << _.nr_threads << ".";
  _INFO << "report: (Misc) word cutoff frequency = " << _.word_cutoff << ".";
  _INFO << "report: (Misc) init range = " << _.init_range << ".";
  _INFO << "report: (Misc) max iteration = " << _.max_iter << ".";
  _INFO << "report: (AdaGrad) eps = " << _.ada_eps << ".";
  _INFO << "report: (AdaGrad) alpha = " << _.ada_alpha << ".";
  _INFO << "report: (AdaGrad) lambda = " << _.lambda << ".";
  _INFO << "report: (AdaGrad) dropout probability = " << _.dropout_probability << ".";
  _INFO << "report: (Network) hidden size = " << _.hidden_layer_size << ".";
  _INFO << "report: (Network) embedding size = " << _.embedding_size << ".";
  _INFO << "report: (Misc) evaluate on each " << _.evaluation_stops << " iterations.";
  _INFO << "report: (Misc) clear gradient per iteration " << _.clear_gradient_per_iter <<".";
  _INFO << "report: (Misc) save intermediate " << _.save_intermediate << ".";
}

bool Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  if (mode == kPipeLearn) {
    train_dataset.clear();
    devel_dataset.clear();
    std::ifstream ifs(reference_file);
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file, training halted.";
      return false;
    }
    _INFO << "#: loading dataset from reference file.";
    ioutils::read_raw_conllx_dependency_dataset(ifs, train_dataset);
    _INFO << "report: " << train_dataset.size() << " training instance(s) is loaded.";
    check_dataset(train_dataset);

    std::ifstream ifs2(devel_file);
    if (!ifs2.good()) {
      _WARN << "#: development file is not loaded.";
    } else {
      ioutils::read_raw_conllx_dependency_dataset(ifs2, devel_dataset);
      _INFO << "report: " << devel_dataset.size() << " developing instance(s) is loaded.";
      check_dataset(devel_dataset);
    }
  } else {
    test_dataset.clear();
    std::ifstream ifs(input_file);
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file, testing halted.";
      return false;
    }
    ioutils::read_raw_conllx_dependency_dataset(ifs, test_dataset);
    _INFO << "report: " << test_dataset.size() << " testing instance(s) is loaded.";
  }
  return true;
}

void Pipe::check_dataset(const std::vector<RawCoNLLXDependency>& dataset) {
  int nr_non_trees = 0;
  int nr_non_projective_trees = 0;
  for (const RawCoNLLXDependency& data: dataset) {
    if (!DependencyUtils::is_tree(data.heads)) {
      ++ nr_non_trees;
    } else if (!DependencyUtils::is_projective(data.heads)) {
      ++ nr_non_projective_trees;
    }
  }
  _INFO << "report: " << nr_non_trees << " tree(s) are illeagl.";
  _INFO << "report: " << nr_non_projective_trees << " tree(s) are legal but not projective.";
}

void Pipe::build_alphabet() {
  std::unordered_map<std::string, int> frequencies;
  for (const auto& data: train_dataset) {
    for (const auto& form: data.forms) { frequencies[form] += 1; }
    for (const auto& postag: data.postags) { postags_alphabet.insert(postag); }
    for (const auto& deprel: data.deprels) { deprels_alphabet.insert(deprel); }
  }
  for (auto& entry: frequencies) {
    if (entry.second >= learn_opt->word_cutoff) { forms_alphabet.insert(entry.first); }
  }

  kFormInFeaturespace = 0;
  forms_alphabet.insert(SpecialOption::UNKNOWN);
  kNilForm = forms_alphabet.insert(SpecialOption::NIL);
  forms_alphabet.insert(SpecialOption::ROOT);

  kPostagInFeaturespace = forms_alphabet.size();
  postags_alphabet.insert(SpecialOption::UNKNOWN);
  kNilPostag = postags_alphabet.insert(SpecialOption::NIL) + kPostagInFeaturespace;
  postags_alphabet.insert(SpecialOption::ROOT);

  kDeprelInFeaturespace = forms_alphabet.size()+ postags_alphabet.size();
  kNilDeprel = deprels_alphabet.insert(SpecialOption::NIL) + kDeprelInFeaturespace;

  _INFO << "report: " << forms_alphabet.size() << " forms(s) are detected.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) are detected.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) are detected.";
  _INFO << "report: form located at: [" << kFormInFeaturespace << " ... "
    << kPostagInFeaturespace- 1 << "]";
  _INFO << "report: postags located at: [" << kPostagInFeaturespace << " ... "
    << kDeprelInFeaturespace- 1 << "]";
  _INFO << "report: deprels located at: [" << kDeprelInFeaturespace << " ... "
    << kDeprelInFeaturespace+ deprels_alphabet.size() - 1 << "]";
  _INFO << "report: form nil=" << kNilForm;
  _INFO << "report: postag nil=" << kNilPostag;
  _INFO << "report: deprel nil=" << kNilDeprel;

  decoder.set_number_of_relations(deprels_alphabet.size());
}

void Pipe::initialize_classifier() {
  namespace algo = boost::algorithm;

  _INFO << "#: start to load embedding";
  std::vector< std::vector<floatval_t> > embeddings;
  std::ifstream ifs(embedding_file);
  if (!ifs.good()) {
    _WARN << "#: failed to open embedding file, embedding will be randomly initialized.";
  } else {
    std::string line;
    auto interval = IO::number_of_lines(ifs) / 10;
    auto nr_lines = 1;

    ifs.close(); ifs.clear(); ifs.open(embedding_file);

    while (std::getline(ifs, line)) {
      if (nr_lines++ % interval == 0) {
        _INFO << "#: loaded " << nr_lines / interval << "0% embeddings.";
      }

      algo::trim(line);
      if (line.size() == 0) { continue; }
      std::vector<std::string> items;
      algo::split(items, line, boost::is_any_of("\t "), boost::token_compress_on);
      int form = forms_alphabet.encode(items[0]);
      // TODO to lower fails with utf8 input
      // int transformed_form = forms_alphabet.encode(items[0].tolower());
      if (form == -1) {
        _TRACE << "report: form\'" << items[0] << "\' not occur in training data, ignore.";
        continue;
      }

      if (items.size() != learn_opt->embedding_size + 1) {
        _WARN << "report: embedding dimension not match to configuration.";
        continue;
      }

      std::vector<floatval_t> embedding;
      embedding.push_back(form);
      for (unsigned i = 1; i < items.size(); ++ i) {
        embedding.push_back( boost::lexical_cast<floatval_t>(items[i]) );
      }
      embeddings.push_back( embedding );
    }
  }

  _INFO << "report: " << embeddings.size() << " embedding is loaded.";
  classifier.initialize(forms_alphabet.size(),
      postags_alphabet.size(),
      deprels_alphabet.size(),
      (*learn_opt),
      embeddings,
      (&dataset),
      precomputed_features
      );
  _INFO << "report: classifier is initialized.";
}

void Pipe::generate_training_samples() {
  dataset.initialize(learn_opt->nr_feature_types, decoder.number_of_transitions());
  std::unordered_map<int, int> encoded_position_frequencies;

  auto nr_processed = 0;
  auto interval = train_dataset.size() / 10; if (interval == 0) { interval = 10; }
  for (auto data: train_dataset) {
    Dependency dependency;
    size_t L = data.forms.size();
    for (size_t i = 0; i < L; ++ i) {
      dependency.push_back(
          forms_alphabet.encode(data.forms[i]),
          postags_alphabet.encode(data.postags[i]),
          data.heads[i],
          deprels_alphabet.encode(data.deprels[i]));
    }

    std::vector<Action> oracle_actions;
    ActionUtils::get_oracle_actions(dependency, oracle_actions);

    std::vector<State> states(oracle_actions.size()+ 1);
    states[0].copy(State(&dependency));

    for (auto step = 0; step < oracle_actions.size(); ++ step) {
      auto oracle_action = oracle_actions[step];
      std::vector<int> attributes;
      get_features(states[step], attributes);

      std::vector<Action> possible_actions;
      decoder.get_possible_actions(states[step], possible_actions);

      std::vector<floatval_t> classes(decoder.number_of_transitions(), -1.);
      for (auto l: decoder.transform(possible_actions)) {
        if (l == decoder.transform(oracle_action)) { classes[l] = 1.; }
        else { classes[l] = 0.; }
      }

      dataset.add(attributes, classes);

      for (auto j = 0; j < attributes.size(); ++ j) {
        auto fid = attributes[j] * attributes.size() + j;
        encoded_position_frequencies[fid] += 1;
      }

      decoder.transit(states[step], oracle_action, &states[step+ 1]);
    }

    if (++ nr_processed % interval == 0) {
      _INFO << "#: generated training samples for " << nr_processed / interval << "0% sentences.";
    }
  }
  _INFO << "#: generated " <<dataset.size() << " training samples.";

  std::vector<std::pair<int, int> > top;
  if (encoded_position_frequencies.size() < learn_opt->nr_precomputed) {
    for (auto& rep: encoded_position_frequencies) { top.push_back(rep); }
  } else {
    top.resize(learn_opt->nr_precomputed);
    std::partial_sort_copy(encoded_position_frequencies.begin(),
        encoded_position_frequencies.end(), top.begin(), top.end(),
        [](std::pair<int, int> const& l, std::pair<int, int> const& r) {
          return l.second > r.second;
        });
  }

  for (auto t: top) { precomputed_features.push_back(t.first); }
}

void Pipe::get_features(const State& s, std::vector<int>& features) {
  int S0 = (s.stack.size() > 0 ? s.stack[s.stack.size() - 1]: -1);
  int S1 = (s.stack.size() > 1 ? s.stack[s.stack.size() - 2]: -1);
  int S2 = (s.stack.size() > 2 ? s.stack[s.stack.size() - 3]: -1);
  int N0 = (s.buffer < s.ref->size()    ? s.buffer:    -1);
  int N1 = (s.buffer+ 1 < s.ref->size() ? s.buffer+ 1: -1);
  int N2 = (s.buffer+ 2 < s.ref->size() ? s.buffer+ 2: -1);

  int S0L  = (S0 >= 0  ? s.left_most_child[S0]:  -1);
  int S0R  = (S0 >= 0  ? s.right_most_child[S0]: -1);
  int S0L2 = (S0 >= 0  ? s.left_2nd_most_child[S0]:  -1);
  int S0R2 = (S0 >= 0  ? s.right_2nd_most_child[S0]: -1);
  int S0LL = (S0L >= 0 ? s.left_most_child[S0L]:  -1);
  int S0LR = (S0L >= 0 ? s.right_most_child[S0L]: -1);
  int S0RL = (S0R >= 0 ? s.left_most_child[S0R]:  -1);
  int S0RR = (S0R >= 0 ? s.right_most_child[S0R]: -1);

  int S1L  = (S1 >= 0  ? s.left_most_child[S1]:  -1);
  int S1R  = (S1 >= 0  ? s.right_most_child[S1]: -1);
  int S1L2 = (S1 >= 0  ? s.left_2nd_most_child[S1]:  -1);
  int S1R2 = (S1 >= 0  ? s.right_2nd_most_child[S1]: -1);
  int S1LL = (S1L >= 0 ? s.left_most_child[S1L]:  -1);
  int S1LR = (S1L >= 0 ? s.right_most_child[S1L]: -1);
  int S1RL = (S1R >= 0 ? s.left_most_child[S1R]:  -1);
  int S1RR = (S1R >= 0 ? s.right_most_child[S1R]: -1);

#define FORM(id)    ((id != -1) ? (s.ref->forms[id]): kNilForm)
#define POSTAG(id)  ((id != -1) ? (s.ref->postags[id]+ kPostagInFeaturespace): kNilPostag)
#define DEPREL(id)  ((id != -1) ? (s.deprels[id]+ kDeprelInFeaturespace): kNilDeprel)
#define PUSH(feat)  do { features.push_back( feat ); } while (0); 

  PUSH( FORM(S0) );   PUSH( POSTAG(S0) );
  PUSH( FORM(S1) );   PUSH( POSTAG(S1) );
  PUSH( FORM(S2) );   PUSH( POSTAG(S2) );
  PUSH( FORM(N0) );   PUSH( POSTAG(N0) );
  PUSH( FORM(N1) );   PUSH( POSTAG(N1) );
  PUSH( FORM(N2) );   PUSH( POSTAG(N2) );
  PUSH( FORM(S0L) );  PUSH( POSTAG(S0L) );  PUSH( DEPREL(S0L) );
  PUSH( FORM(S0R) );  PUSH( POSTAG(S0R) );  PUSH( DEPREL(S0R) );
  PUSH( FORM(S0L2) ); PUSH( POSTAG(S0L2) ); PUSH( DEPREL(S0L2) );
  PUSH( FORM(S0R2) ); PUSH( POSTAG(S0R2) ); PUSH( DEPREL(S0R2) );
  PUSH( FORM(S0LL) ); PUSH( POSTAG(S0LL) ); PUSH( DEPREL(S0LL) );
  PUSH( FORM(S0RR) ); PUSH( POSTAG(S0RR) ); PUSH( DEPREL(S0RR) );
  PUSH( FORM(S1L) );  PUSH( POSTAG(S1L) );  PUSH( DEPREL(S1L) );
  PUSH( FORM(S1R) );  PUSH( POSTAG(S1R) );  PUSH( DEPREL(S1R) );
  PUSH( FORM(S1L2) ); PUSH( POSTAG(S1L2) ); PUSH( DEPREL(S1L2) );
  PUSH( FORM(S1R2) ); PUSH( POSTAG(S1R2) ); PUSH( DEPREL(S1R2) );
  PUSH( FORM(S1LL) ); PUSH( POSTAG(S1LL) ); PUSH( DEPREL(S1LL) );
  PUSH( FORM(S1RR) ); PUSH( POSTAG(S1RR) ); PUSH( DEPREL(S1RR) );

#undef FORM
#undef POSTAG
#undef DEPREL
#undef PUSH
}

void Pipe::learn() {
  if (!setup()) { return; }
  build_alphabet();
  generate_training_samples();
  initialize_classifier();

  for (size_t iter = 0; iter < learn_opt->max_iter; ++ iter) {
    boost::timer::cpu_timer timer;
    classifier.compute_ada_gradient_step();
    _INFO << "pipe (iter#" << (iter+ 1) << "): cost=" << classifier.get_cost()
      << ", accuracy(%)=" << classifier.get_accuracy()
      << " (" << timer.elapsed().user << ")";

    classifier.take_ada_gradient_step();

    if (devel_dataset.size() > 0 && (iter+1) % learn_opt->evaluation_stops == 0) {
      _INFO << "start evaluating ...";
      classifier.precomputing();

      std::vector<int> heads;
      std::vector<std::string> deprels;
      auto corr_heads = 0, corr_deprels = 0, nr_tokens = 0;
      for (auto data: devel_dataset) {
        predict(data, heads, deprels);
        auto L = heads.size();
        for (auto i = 0; i < L; ++ i) {
          if (boost::u32regex_match(data.forms[i], boost::make_u32regex("[[:P*:]]"))) {
            continue;
          }
          ++ nr_tokens;
          if (heads[i] == data.heads[i]) {
            ++ corr_heads;
            if (deprels[i] == data.deprels[i]) { ++ corr_deprels; }
          }
        }
      }
      _INFO << "evaluating done. UAS=" << (floatval_t)corr_heads/ nr_tokens
        << " LAS=" << (floatval_t)corr_deprels/nr_tokens;
    }
  }
}

void Pipe::predict(const RawCoNLLXDependency& data,
    std::vector<int>& heads,
    std::vector<std::string>& deprels) {
  Dependency dependency;
  size_t L = data.forms.size();
  for (size_t i = 0; i < L; ++ i) {
    auto form = forms_alphabet.encode(data.forms[i]);
    auto postag = postags_alphabet.encode(data.postags[i]);
    dependency.push_back(
        (form >= 0 ? form: forms_alphabet.encode(SpecialOption::UNKNOWN)),
        postag,
        -1, -1);
  }

  std::vector<State> states(L*2);
  states[0].copy(State(&dependency));
  for (auto step = 0; step < L*2-1; ++ step) {
    std::vector<int> attributes;
    get_features(states[step], attributes);

    std::vector<floatval_t> classes(decoder.number_of_transitions(), 0.);
    classifier.score(attributes, classes);

    std::vector<Action> possible_actions;
    decoder.get_possible_actions(states[step], possible_actions);

    auto best = -1;
    for (auto l: decoder.transform(possible_actions)) {
      if (best == -1 || classes[best] < classes[l]) { best = l; }
    }

    auto act = decoder.transform(best);
    decoder.transit(states[step], act, &states[step+ 1]);
  }

  heads.resize(L);
  deprels.resize(L);
  for (auto i = 0; i < L; ++ i) {
    heads[i] = states[L*2-1].heads[i];
    deprels[i] = deprels_alphabet.decode(states[L*2-1].deprels[i]);
  }
}


void Pipe::test() {
}

void Pipe::load_model(const std::string& model_path) {
}

void Pipe::save_model(const std::string& model_path) {
}

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
