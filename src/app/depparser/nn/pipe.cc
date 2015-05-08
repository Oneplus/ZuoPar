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
#include <boost/timer.hpp>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

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
  root = opts.root;

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

  _INFO << "TEST:: mode is activated.";
  _INFO << "report: input file: " << input_file;
  _INFO << "report: output file: " << output_file;
  _INFO << "report: model file: " << model_file;

  load_model(model_file);
}

void Pipe::display_learning_parameters() {
  if (learn_opt == nullptr) { _WARN << "pipe: learning parameter is not set."; return; }

  const LearnOption& _ = (*learn_opt);
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
  _INFO << "report: (Misc) save intermediate = " << _.save_intermediate << ".";
  _INFO << "report: (Misc) fix embedding = " << _.fix_embeddings << ".";
  _INFO << "report: (Misc) use dynamic oracle = " << _.use_dynamic_oracle << ".";

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

void Pipe::info() {
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
  _INFO << "report: root rel(str)=" << root;
  _INFO << "report: root rel(index)=" << deprels_alphabet.encode(root);

  decoder.set_root_relation(deprels_alphabet.encode(root));
  decoder.set_number_of_relations(deprels_alphabet.size()- 1);
}


void Pipe::build_alphabet() {
  std::unordered_map<std::string, int> frequencies;
  for (const auto& data: train_dataset) {
    for (auto i = 1; i < data.forms.size(); ++ i) {
      // Ignore the leading dummy root.
      postags_alphabet.insert(data.postags[i]);
      deprels_alphabet.insert(data.deprels[i]);
      frequencies[data.forms[i]] += 1;
    }
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

  info();
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
        //_TRACE << "report: form\'" << items[0] << "\' not occur in training data, ignore.";
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
      precomputed_features
      );
  _INFO << "report: classifier is initialized.";
}

void Pipe::generate_training_samples() {
  dataset.initialize(learn_opt->nr_feature_types, decoder.number_of_transitions());
  std::unordered_map<int, int> features_frequencies;

  auto nr_processed = 0;
  auto interval = train_dataset.size() / 10; if (interval == 0) { interval = 10; }
  for (const auto& data: train_dataset) {
    if (!DependencyUtils::is_tree(data.heads) ||
        !DependencyUtils::is_projective(data.heads)) {
      continue;
    }
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
    ActionUtils::get_oracle_actions2(dependency, oracle_actions);

    std::vector<State> states(oracle_actions.size()+ 1);
    states[0].copy(State(&dependency));
    decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
    for (auto step = 1; step < oracle_actions.size(); ++ step) {
      auto& oracle_action = oracle_actions[step];
      std::vector<int> attributes;
      get_features(states[step], attributes);
      if (attributes.size() != learn_opt->nr_feature_types) {
        _WARN << "#: number of feature types unequal to configed number";
      }

      if (!learn_opt->use_dynamic_oracle) {
        // If not using the dynamic oracle mode, caching all the training instances
        // in the dataset
        std::vector<Action> possible_actions;
        decoder.get_possible_actions(states[step], possible_actions);
        std::vector<floatval_t> classes(decoder.number_of_transitions(), -1.);
        for (auto l: decoder.transform(possible_actions)) {
          if (l == decoder.transform(oracle_action)) { classes[l] = 1.; }
          else { classes[l] = 0.; }
        }
        dataset.add(attributes, classes);
      }

      for (auto j = 0; j < attributes.size(); ++ j) {
        auto fid = attributes[j] * attributes.size() + j;
        features_frequencies[fid] += 1;
      }
      decoder.transit(states[step], oracle_action, &states[step+ 1]);
    }

    if (++ nr_processed % interval == 0) {
      _INFO << "#: generated training samples for " << nr_processed / interval << "0% sentences.";
    }
  }
  _INFO << "#: generated " <<dataset.size() << " training samples.";

  std::vector<std::pair<int, int> > top;
  if (features_frequencies.size() < learn_opt->nr_precomputed) {
    for (auto& rep: features_frequencies) { top.push_back(rep); }
  } else {
    top.resize(learn_opt->nr_precomputed);
    std::partial_sort_copy(features_frequencies.begin(),
        features_frequencies.end(), top.begin(), top.end(),
        [](std::pair<int, int> const& l, std::pair<int, int> const& r) {
          if (l.second != r.second) { return l.second > r.second; }
          return l.first < r.first;
        });
  }

  for (auto& t: top) { precomputed_features.push_back(t.first); }
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
  int S0RR = (S0R >= 0 ? s.right_most_child[S0R]: -1);

  int S1L  = (S1 >= 0  ? s.left_most_child[S1]:  -1);
  int S1R  = (S1 >= 0  ? s.right_most_child[S1]: -1);
  int S1L2 = (S1 >= 0  ? s.left_2nd_most_child[S1]:  -1);
  int S1R2 = (S1 >= 0  ? s.right_2nd_most_child[S1]: -1);
  int S1LL = (S1L >= 0 ? s.left_most_child[S1L]:  -1);
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
  decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
  for (auto step = 1; step < L*2-1; ++ step) {
    std::vector<int> attributes;
    get_features(states[step], attributes);

    std::vector<floatval_t> scores(decoder.number_of_transitions(), 0);
    classifier.score(attributes, scores);

    std::vector<Action> possible_actions;
    decoder.get_possible_actions(states[step], possible_actions);

    auto best = -1;
    for (auto l: decoder.transform(possible_actions)) {
      if (best == -1 || scores[best] < scores[l]) { best = l; }
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

std::pair<
  std::vector<Sample>::const_iterator,
  std::vector<Sample>::const_iterator
> Pipe::generate_training_samples_one_batch() {
  std::vector<Sample>::const_iterator begin;
  std::vector<Sample>::const_iterator end;

  if (learn_opt->use_dynamic_oracle) {
    dataset.samples.clear();

    while (dataset.samples.size() < learn_opt->batch_size) {
      const RawCoNLLXDependency& data = train_dataset[cursor ++];
      if (cursor == train_dataset.size()) { cursor = 0; }

      if (!DependencyUtils::is_tree(data.heads) ||
          !DependencyUtils::is_projective(data.heads)) {
        continue;
      }
      Dependency dependency;
      size_t L = data.forms.size();
      for (size_t i = 0; i < L; ++ i) {
        dependency.push_back(
            forms_alphabet.encode(data.forms[i]),
            postags_alphabet.encode(data.postags[i]),
            data.heads[i],
            deprels_alphabet.encode(data.deprels[i]));
      }

      std::vector<State> states(L*2);
      states[0].copy(State(&dependency));
      decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
      for (auto step = 1; step < L*2-1; ++ step) {
        std::vector<int> attributes;
        get_features(states[step], attributes);
        if (attributes.size() != learn_opt->nr_feature_types) {
          _WARN << "#: number of feature types unequal to configed number";
        }

        std::vector<floatval_t> scores;
        classifier.score(attributes, scores);

        std::vector<Action> possible_actions;
        decoder.get_possible_actions(states[step], possible_actions);
 
        std::vector<floatval_t> classes(decoder.number_of_transitions(), -1.);
        auto oracle = -1;
        auto best_score = 0;
        for (auto& act: possible_actions) {
          auto l = decoder.transform(act);
          classes[l] = 0.;
          decoder.transit(states[step], act, &states[step+1]);
          auto c = states[step+1].cost(dependency.heads, dependency.deprels);
          // if (c == 0) _INFO << act << " " << c;
          if (c == 0 && (oracle == -1 || scores[l] > scores[oracle])) {
            oracle = l;
          }
        }
        if (oracle == -1) {
          _ERROR << "nani! " << step << " " << cursor;
          exit(1);
        }
        classes[oracle] = 1.;

        dataset.add(attributes, classes);
        decoder.transit(states[step], decoder.transform(oracle), &states[step+ 1]);
      }
    }
    begin = dataset.samples.begin();
    end = dataset.samples.end();
  } else {
    const std::vector<Sample>& entire_samples = dataset.samples;
    begin = entire_samples.begin() + cursor;
    end = entire_samples.end();
    if (cursor + learn_opt->batch_size < entire_samples.size()) {
      end = entire_samples.begin() + cursor + learn_opt->batch_size;
      cursor += learn_opt->batch_size;
    } else {
      cursor = 0;
    }
  }
  return std::make_pair(begin, end);
}

void Pipe::learn() {
  if (!setup()) { return; }
  build_alphabet();
  generate_training_samples();
  initialize_classifier();

  cursor = 0;
  floatval_t best_uas = -1;
  for (size_t iter = 0; iter < learn_opt->max_iter; ++ iter) {
    boost::timer t;

    std::vector<Sample>::const_iterator begin;
    std::vector<Sample>::const_iterator end;
    std::tie(begin, end) = generate_training_samples_one_batch();

    classifier.compute_ada_gradient_step(begin, end);
    _INFO << "pipe (iter#" << (iter+ 1) << "): cost=" << classifier.get_cost()
      << ", accuracy(%)=" << classifier.get_accuracy()
      << " (" << t.elapsed() << ")";

    classifier.take_ada_gradient_step();

    if (devel_dataset.size() > 0 && (iter+1) % learn_opt->evaluation_stops == 0) {
      _INFO << "eval: start evaluating ...";
      classifier.precomputing();

      std::vector<int> heads;
      std::vector<std::string> deprels;
      auto corr_heads = 0, corr_deprels = 0, nr_tokens = 0;

      t.restart();
      for (const auto& data: devel_dataset) {
        predict(data, heads, deprels);

        auto L = heads.size();
        for (auto i = 1; i < L; ++ i) { // ignore dummy root
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
      auto uas = (floatval_t)corr_heads/nr_tokens;
      auto las = (floatval_t)corr_deprels/nr_tokens;
      _INFO << "eval: evaluating done. UAS=" << uas << " LAS=" << las << " (" << t.elapsed() << ")";

      if (best_uas < uas && learn_opt->save_intermediate) {
        best_uas = uas;
        save_model(model_file);
        _INFO << "report: model saved to " << model_file;
      }
    }
  }
}


void Pipe::test() {
  if (!setup()) { return; }
  classifier.precomputing();

  std::vector<int> heads;
  std::vector<std::string> deprels;

  std::ostream* os = IO::get_ostream(output_file);
  boost::timer t;
  for (const auto& data: test_dataset) {
    predict(data, heads, deprels);

    auto L = heads.size();
    for (auto i = 1; i < L; ++ i) {
      (*os) << i << "\t"
        << data.forms[i] << "\t"
        << "_" << "\t"
        << data.cpostags[i] << "\t"
        << data.postags[i] << "\t";
      if (data.feats[i].size()) {
        (*os) << boost::algorithm::join(data.feats[i], "|") << "\t"; }
      else { (*os) << "_\t"; }
      (*os) << heads[i] << "\t" << deprels[i] << std::endl;
    }
    (*os) << std::endl;
  }
  _INFO << "#: elapsed " << t.elapsed();
}

void Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "#: failed to open file.";
    return;
  }
  boost::archive::text_iarchive ia(mfs);
  ia >> root;
  classifier.load(mfs);
  forms_alphabet.load(mfs);
  postags_alphabet.load(mfs);
  deprels_alphabet.load(mfs);
  kFormInFeaturespace = 0;
  kNilForm = forms_alphabet.encode(SpecialOption::NIL);

  kPostagInFeaturespace = forms_alphabet.size();
  kNilPostag = postags_alphabet.encode(SpecialOption::NIL) + kPostagInFeaturespace;

  kDeprelInFeaturespace = forms_alphabet.size()+ postags_alphabet.size();
  kNilDeprel = deprels_alphabet.encode(SpecialOption::NIL) + kDeprelInFeaturespace;

  info();
}

void Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "#: failed to open file.";
    return;
  }
  boost::archive::text_oarchive oa(mfs);
  oa << root;
  classifier.save(mfs);
  forms_alphabet.save(mfs);
  postags_alphabet.save(mfs);
  deprels_alphabet.save(mfs);
}

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
