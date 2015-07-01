#include <fstream>
#include <algorithm>
#include "app/depparser/utils.h"
#include "app/depparser/nn/action.h"
#include "app/depparser/nn/action_utils.h"
#include "app/depparser/nn/weiss15/pipe.h"
#include "utils/logging.h"
#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "utils/math/fast_binned.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/timer/timer.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

Pipe::Pipe(const PretrainOption& opts)
  : mode(kPipePretrain),
  pretrain_opt(&opts), pretest_opt(nullptr), learn_opt(nullptr), test_opt(nullptr),
  forms_alphabet(false), postags_alphabet(false), deprels_alphabet(false),
  cursor(0), nr_feature_types(0) {
  reference_file = opts.reference_file;
  devel_file = opts.devel_file;
  model_file = opts.model_file;
  embedding_file = opts.embedding_file;
  root = opts.root;

  _INFO << "PRETRAIN:: mode is activated.";
  _INFO << "report: reference file: " << reference_file;
  _INFO << "report: development file: " << devel_file;
  _INFO << "report: model file: " << model_file;
  _INFO << "report: embedding file: " << embedding_file;

  display_pretraining_parameters();
}

Pipe::Pipe(const PretestOption& opts)
  : mode(kPipePretest),
  pretrain_opt(nullptr), pretest_opt(&opts), learn_opt(nullptr), test_opt(nullptr),
  forms_alphabet(false), postags_alphabet(false), deprels_alphabet(false),
  cursor(0), nr_feature_types(0) {

  input_file = opts.input_file;
  output_file = opts.output_file;
  model_file = opts.model_file;

  _INFO << "TEST:: mode is activated.";
  _INFO << "report: input file: " << input_file;
  _INFO << "report: output file: " << output_file;
  _INFO << "report: model file: " << model_file;

  load_model(model_file);
}


Pipe::Pipe(const LearnOption& opts)
  : mode(kPipeLearn), pretrain_opt(nullptr), learn_opt(&opts), test_opt(nullptr),
  forms_alphabet(false), postags_alphabet(false), deprels_alphabet(false),
  cursor(0), nr_feature_types(0) {

  reference_file = opts.reference_file;
  devel_file = opts.devel_file;
  model_file = opts.model_file;

  _INFO << "LEARN: mode is activated.";
  _INFO << "report: reference file: " << reference_file;
  _INFO << "report: development file: " << devel_file;
  _INFO << "report: model file: " << model_file;
  _INFO << "report: pretrained model file: " << opts.pretrain_model_file;
  _INFO << "report: beam size: " << opts.beam_size;
  _INFO << "report: max iteration: " << opts.max_iter;

  load_model(opts.pretrain_model_file);
}

Pipe::Pipe(const TestOption& opts)
  : mode(kPipeTest), pretrain_opt(nullptr), learn_opt(nullptr), test_opt(&opts),
  forms_alphabet(false), postags_alphabet(false), deprels_alphabet(false),
  cursor(0), nr_feature_types(0) {

  input_file = opts.input_file;
  output_file = opts.output_file;
  model_file = opts.model_file;

  _INFO << "TEST:: mode is activated.";
  _INFO << "report: input file: " << input_file;
  _INFO << "report: output file: " << output_file;
  _INFO << "report: model file: " << model_file;
  _INFO << "report: pretrained model file: " << opts.pretrain_model_file;

  load_model(opts.pretrain_model_file);
  load_structured_model(opts.model_file);
}

void Pipe::display_pretraining_parameters() {
  if (pretrain_opt == nullptr) { _WARN << "pipe: pretraining parameter is not set."; return; }

  const PretrainOption& _ = (*pretrain_opt);
  _INFO << "report: (Misc) word cutoff frequency = " << _.word_cutoff << ".";
  _INFO << "report: (Misc) init range = " << _.init_range << ".";
  _INFO << "report: (Misc) max iteration = " << _.max_iter << ".";
  _INFO << "report: (AdaGrad) dropout probability = " << _.dropout_probability << ".";
  _INFO << "report: (Network) hidden size = " << _.hidden_layer_size << ".";
  _INFO << "report: (Network) embedding size = " << _.embedding_size << ".";
  _INFO << "report: (Network) lambda = " << _.lambda << ".";
  _INFO << "report: (Misc) evaluate on each " << _.evaluation_stops << " iterations.";
  _INFO << "report: (Misc) save intermediate = " << _.save_intermediate << ".";
  _INFO << "report: (Misc) fix embedding = " << _.fix_embeddings << ".";
  _INFO << "report: (Misc) oracle = " << _.oracle << ".";
}

bool Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  if (mode == kPipeLearn || mode == kPipePretrain) {
    train_dataset.clear();
    devel_dataset.clear();
    std::ifstream ifs(reference_file);
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file, training halted.";
      return false;
    }
    _INFO << "#: loading dataset from reference file.";
    ioutils::read_raw_conllx_dependency_dataset(ifs, train_dataset, true, SpecialOption::ROOT);
    _INFO << "report: " << train_dataset.size() << " training instance(s) is loaded.";
    check_dataset(train_dataset);

    std::ifstream ifs2(devel_file);
    if (!ifs2.good()) {
      _WARN << "#: development file is not loaded.";
    } else {
      ioutils::read_raw_conllx_dependency_dataset(ifs2, devel_dataset, true, SpecialOption::ROOT);
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
    ioutils::read_raw_conllx_dependency_dataset(ifs, test_dataset, true, SpecialOption::ROOT);
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
  _INFO << "report: " << nr_non_trees << " tree(s) are illegal.";
  _INFO << "report: " << nr_non_projective_trees << " tree(s) are legal but not projective.";
}

void Pipe::info() {
  _INFO << "report: " << forms_alphabet.size() << " forms(s) are detected.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) are detected.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) are detected.";
  extractor.display();
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
    if (entry.second >= pretrain_opt->word_cutoff) { forms_alphabet.insert(entry.first); }
  }

  forms_alphabet.insert(SpecialOption::UNKNOWN);
  forms_alphabet.insert(SpecialOption::NIL);
  forms_alphabet.insert(SpecialOption::ROOT);

  postags_alphabet.insert(SpecialOption::UNKNOWN);
  postags_alphabet.insert(SpecialOption::NIL);
  postags_alphabet.insert(SpecialOption::ROOT);

  deprels_alphabet.insert(SpecialOption::NIL);
}

void Pipe::build_feature_space() {
  extractor.initialize(
      forms_alphabet.encode(SpecialOption::NIL), forms_alphabet.size(),
      postags_alphabet.encode(SpecialOption::NIL), postags_alphabet.size(),
      deprels_alphabet.encode(SpecialOption::NIL), deprels_alphabet.size());

  info();
}

void Pipe::initialize_classifier() {
  namespace algo = boost::algorithm;

  _INFO << "#: start to load embedding";
  std::unordered_map< std::string, std::vector<floatval_t> > raw_embeddings;
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

      algo::trim(line); if (line.size() == 0) { continue; }
      std::vector<std::string> items;
      algo::split(items, line, boost::is_any_of("\t "), boost::token_compress_on);

      if (items.size() != pretrain_opt->embedding_size + 1) {
        _WARN << "report: embedding dimension not match to configuration.";
        continue;
      }

      std::vector<floatval_t> embedding;
      for (unsigned i = 1; i < items.size(); ++ i) {
        embedding.push_back( boost::lexical_cast<floatval_t>(items[i]) );
      }
      raw_embeddings[items[0]] = embedding;
    }
  }

  std::vector< std::vector<floatval_t> > embeddings;
  for (auto i = 0; i < forms_alphabet.size(); ++ i) {
    std::string token = forms_alphabet.decode(i);
    auto result = raw_embeddings.find(token);
    std::vector<floatval_t> embedding;
    if (result != raw_embeddings.end()) {
      embedding.push_back(i);
      for (const floatval_t& v: result->second) { embedding.push_back(v); }
    } else {
      result = raw_embeddings.find(boost::algorithm::to_lower_copy(token));
      if (result != raw_embeddings.end()) {
        embedding.push_back(i);
        for (const floatval_t& v: result->second) { embedding.push_back(v); }
      }
    }

    if (embedding.size() > 0) embeddings.push_back( embedding );
  }
  _INFO << "report: " << embeddings.size() << " embedding is loaded.";

  classifier.initialize(extractor.n_dim(), deprels_alphabet.size()*2-1,
      nr_feature_types, (*pretrain_opt), embeddings, precomputed_features
      );
  _INFO << "report: classifier is initialized.";
}

void Pipe::generate_training_samples() {
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
    transduce_instance_to_dependency(data, dependency, true);

    std::vector<Action> oracle_actions;
    ActionUtils::get_oracle_actions2(dependency, oracle_actions);

    std::vector<State> states(oracle_actions.size()+ 1);
    states[0].copy(State(&dependency));
    decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
    for (auto step = 1; step < oracle_actions.size(); ++ step) {
      auto& oracle_action = oracle_actions[step];
      std::vector<int> attributes;
      extractor.get_features(states[step], attributes);

      if (nr_feature_types == 0) {
        nr_feature_types = attributes.size();
      } else if (attributes.size() != nr_feature_types) {
        _WARN << "#: number of feature types unequal to configed number";
      }

      if (pretrain_opt->oracle == "static") {
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
  if (features_frequencies.size() < pretrain_opt->nr_precomputed) {
    for (auto& rep: features_frequencies) { top.push_back(rep); }
  } else {
    top.resize(pretrain_opt->nr_precomputed);
    std::partial_sort_copy(features_frequencies.begin(),
        features_frequencies.end(), top.begin(), top.end(),
        [](std::pair<int, int> const& l, std::pair<int, int> const& r) {
          if (l.second != r.second) { return l.second > r.second; }
          return l.first < r.first;
        });
  }

  for (auto& t: top) { precomputed_features.push_back(t.first); }
}

void Pipe::transduce_instance_to_dependency(const RawCoNLLXDependency& data,
    Dependency& dependency, bool with_reference) {
  auto L = data.size();
  for (size_t i = 0; i < L; ++ i) {
    auto form = (i == 0? 
        forms_alphabet.encode(SpecialOption::ROOT):
        forms_alphabet.encode(data.forms[i])
        );
    auto postag = (i == 0?
        postags_alphabet.encode(SpecialOption::ROOT):
        postags_alphabet.encode(data.postags[i])
        );
    dependency.push_back(
        (form >= 0 ? form: forms_alphabet.encode(SpecialOption::UNKNOWN)),
        postag,
        with_reference? data.heads[i]: -1,
        with_reference? deprels_alphabet.encode(data.deprels[i]): -1);
  }
}

void Pipe::predict(const RawCoNLLXDependency& data,
    std::vector<int>& heads,
    std::vector<std::string>& deprels) {
  size_t L = data.forms.size();
  Dependency dependency;
  transduce_instance_to_dependency(data, dependency, false);
  std::vector<State> states(L*2);
  states[0].copy(State(&dependency));
  decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
  for (auto step = 1; step < L*2-1; ++ step) {
    std::vector<int> attributes;
    extractor.get_features(states[step], attributes);

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

  if (pretrain_opt->oracle == "static") {
    const std::vector<Sample>& entire_samples = dataset.samples;
    begin = entire_samples.begin() + cursor;
    end = entire_samples.end();
    if (cursor + pretrain_opt->batch_size < entire_samples.size()) {
      end = entire_samples.begin() + cursor + pretrain_opt->batch_size;
      cursor += pretrain_opt->batch_size;
    } else {
      cursor = 0;
    }
  } else {
    dataset.samples.clear();

    while (dataset.samples.size() < pretrain_opt->batch_size) {
      const RawCoNLLXDependency& data = train_dataset[cursor ++];
      if (cursor == train_dataset.size()) { cursor = 0; }

      if (!DependencyUtils::is_tree(data.heads) ||
          !DependencyUtils::is_projective(data.heads)) {
        continue;
      }
      Dependency dependency;
      size_t L = data.forms.size();
      transduce_instance_to_dependency(data, dependency, true);

      auto prev_cost = 0;
      std::vector<State> states(L*2);
      states[0].copy(State(&dependency));
      decoder.transit(states[0], ActionFactory::make_shift(), &states[1]);
      for (auto step = 1; step < L*2-1; ++ step) {
        std::vector<int> attributes;
        extractor.get_features(states[step], attributes);

        if (attributes.size() != nr_feature_types) {
          _WARN << "#: number of feature types unequal to configed number";
        }
        std::vector<floatval_t> scores;
        classifier.score(attributes, scores);

        std::vector<Action> possible_actions;
        decoder.get_possible_actions(states[step], possible_actions);

        std::vector<floatval_t> classes(decoder.number_of_transitions(), -1.);
        std::vector<int> costs(decoder.number_of_transitions(), 1024);

        auto oracle = -1;
        auto best_trans = -1;
        for (auto& act: possible_actions) {
          auto l = decoder.transform(act);
          classes[l] = 0.;

          decoder.transit(states[step], act, &states[step+1]);
          costs[l] = states[step+1].cost(dependency.heads, dependency.deprels);
          if (costs[l] - prev_cost == 0 &&
              (oracle == -1 || scores[oracle] < scores[l])) {
            oracle = l;
          }

          if (best_trans == -1 || scores[best_trans] < scores[l]) {
            best_trans = l;
          }
        }

        classes[oracle] = 1.;

        dataset.add(attributes, classes);
        if (pretrain_opt->oracle == "nondet") {
          decoder.transit(states[step], decoder.transform(oracle), &states[step+ 1]);
          prev_cost = costs[oracle];
        } else if (pretrain_opt->oracle == "explore") {
          decoder.transit(states[step], decoder.transform(best_trans), &states[step+ 1]);
          prev_cost = costs[best_trans];
        }
      }
    }
    begin = dataset.samples.begin();
    end = dataset.samples.end();
  }
  return std::make_pair(begin, end);
}

void Pipe::pretrain() {
  if (!setup()) { return; }
  build_alphabet();
  build_feature_space();
  generate_training_samples();
  initialize_classifier();

  cursor = 0;
  floatval_t best_uas = -1;
  for (size_t iter = 0; iter < pretrain_opt->max_iter; ++ iter) {
    boost::timer::cpu_timer t;

    std::vector<Sample>::const_iterator begin;
    std::vector<Sample>::const_iterator end;
    std::tie(begin, end) = generate_training_samples_one_batch();

    classifier.compute_cost_and_gradient(begin, end,
        (pretrain_opt->debug && (iter+1)% pretrain_opt->evaluation_stops == 0));
    if (pretrain_opt->algorithm == "asgd") {
      classifier.take_momentum_asgd_step(0.1 + (0.9999 - 0.1) / pretrain_opt->max_iter * iter);
    } else {
      classifier.take_adagrad_step();
    }

    _INFO << "pipe (iter#" << (iter+ 1) << "): cost=" << classifier.get_cost()
      << ", accuracy(%)=" << classifier.get_accuracy()
      << " (" << t.elapsed().wall/ 1e9 << ")";

    if (devel_dataset.size() > 0 && (iter+1) % pretrain_opt->evaluation_stops == 0) {
      _INFO << "eval: start evaluating ...";
      classifier.precomputing();

      std::vector<int> heads;
      std::vector<std::string> deprels;
      auto corr_heads = 0, corr_deprels = 0, nr_tokens = 0;

      t.start();
      for (const auto& data: devel_dataset) {
        predict(data, heads, deprels);

        std::tuple<int, int, int> eval;
        if (pretrain_opt->evaluation_method == "conllx") {
          eval = DependencyParserUtils::evaluate_conllx(data, heads, deprels);
        } else if (pretrain_opt->evaluation_method == "chen14en") {
          eval = DependencyParserUtils::evaluate_chen14en(data, heads, deprels);
        } else if (pretrain_opt->evaluation_method == "chen14ch") {
          eval = DependencyParserUtils::evaluate_chen14ch(data, heads, deprels);
        }

        nr_tokens += std::get<0>(eval);
        corr_heads += std::get<1>(eval);
        corr_deprels += std::get<2>(eval);
      }
      auto uas = (floatval_t)corr_heads/nr_tokens;
      auto las = (floatval_t)corr_deprels/nr_tokens;
      _INFO << "eval: evaluating done. UAS=" << uas << " LAS=" << las
        << " (" << t.elapsed().wall /1e9 << ")";

      if (best_uas < uas && pretrain_opt->save_intermediate) {
        best_uas = uas;
        save_model(model_file);
        _INFO << "report: model saved to " << model_file;
      }
    }
  }
}

void Pipe::learn() {
  if (!setup()) { return; }
  classifier.precomputing();

  model.initialize(&extractor, &classifier, &decoder);
  model.zeros();

  learner = new Learner(&model,
      (learn_opt->algorithm == "ap"? kAveragePerceptron: kPassiveAggressive));
  search = new StructuredDecoder(
      deprels_alphabet.size()- 1, deprels_alphabet.encode(root),
      learn_opt->beam_size, false, kEarlyUpdate, &model);

  floatval_t best_uas = -1;
  _INFO << "start learning ...";
  auto nr_total = 0;
  for (auto iter = 0; iter < learn_opt->max_iter; ++ iter) {
    search->set_avg(false);
    auto nr_processed = 0;
    auto interval = train_dataset.size() / 10;
    if (interval == 0) { interval = 10; }

    _INFO << "# iteration: " << iter;

    std::random_shuffle(train_dataset.begin(), train_dataset.end());
    for (const auto& data: train_dataset) {
      if (!DependencyUtils::is_tree(data.heads) ||
          !DependencyUtils::is_projective(data.heads)) {
        continue;
      }
      Dependency dependency;
      size_t L = data.forms.size();
      transduce_instance_to_dependency(data, dependency, true);

      std::vector<Action> oracle_actions;
      ActionUtils::get_oracle_actions2(dependency, oracle_actions);

      State init_state(&dependency);
      StructuredDecoder::const_decode_result_t result =
        search->decode(init_state, oracle_actions, 2 * L- 1);

      learner->set_timestamp(nr_total+ 1);
      learner->learn(result.first, result.second);

      model.average();
      if (++ nr_processed % interval == 0) {
        _INFO << "#: preceptron trained " << nr_processed / interval << "0% sentences.";
      }
    }
    _INFO << "#: preceptron training is done.";

    auto corr_heads = 0, corr_deprels = 0, nr_tokens = 0;
    if (learn_opt->averaged) { search->set_avg(true); }
    boost::timer::cpu_timer t;
    for (const auto& data: devel_dataset) {
      Dependency dependency;
      size_t L = data.forms.size();
      transduce_instance_to_dependency(data, dependency, true);

      std::vector<Action> oracle_actions;

      State init_state(&dependency);
      StructuredDecoder::const_decode_result_t result =
        search->decode(init_state, oracle_actions, 2 * L- 1);

      std::vector<int> heads;
      std::vector<std::string> deprels;

      heads.resize(L); deprels.resize(L);
      for (size_t i = 1; i < L; ++ i) {
        heads[i] = result.first->heads[i];
        deprels[i] = deprels_alphabet.decode(result.first->deprels[i]);
      }
      std::tuple<int, int, int> eval;
      if (learn_opt->evaluation_method == "conllx") {
        eval = DependencyParserUtils::evaluate_conllx(data, heads, deprels);
      } else if (learn_opt->evaluation_method == "chen14en") {
        eval = DependencyParserUtils::evaluate_chen14en(data, heads, deprels);
      } else if (learn_opt->evaluation_method == "chen14ch") {
        eval = DependencyParserUtils::evaluate_chen14ch(data, heads, deprels);
      }

      nr_tokens += std::get<0>(eval);
      corr_heads += std::get<1>(eval);
      corr_deprels += std::get<2>(eval);
    }
    auto uas = (floatval_t)corr_heads/nr_tokens;
    auto las = (floatval_t)corr_deprels/nr_tokens;
    _INFO << "eval: evaluating done. UAS=" << uas << " LAS=" << las
      << " (" << t.elapsed().wall /1e9 << ")";

    if (best_uas < uas) {
      best_uas = uas;
      _INFO << "report: model saved to " << model_file;
      save_structured_model(model_file);
    }
  }
}

void Pipe::pretest() {
  if (!setup()) { return; }
  classifier.precomputing();

  std::vector<int> heads;
  std::vector<std::string> deprels;

  std::ostream* os = IO::get_ostream(output_file);
  boost::timer::cpu_timer t;
  for (const auto& data: test_dataset) {
    predict(data, heads, deprels);

    Dependency dependency;
    size_t L = data.forms.size();
    transduce_instance_to_dependency(data, dependency, true);

    std::vector<Action> oracle_actions;

    State init_state(&dependency);
    StructuredDecoder::const_decode_result_t result =
      search->decode(init_state, oracle_actions, 2 * L- 1);

    std::vector<int> heads;
    std::vector<std::string> deprels;

    heads.resize(L); deprels.resize(L);
    for (size_t i = 0; i < L; ++ i) {
      heads[i] = result.first->heads[i];
      deprels[i] = deprels_alphabet.decode(result.second->deprels[i]);
    }

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
  _INFO << "#: elapsed " << t.elapsed().wall / 1e9;
}

void Pipe::test() {
  if (!setup()) { return; }
  classifier.precomputing();

  model.initialize(&extractor, &classifier, &decoder);

  search = new StructuredDecoder(
      deprels_alphabet.size()- 1, deprels_alphabet.encode(root),
      test_opt->beam_size, true, kEarlyUpdate, &model);

  std::vector<int> heads;
  std::vector<std::string> deprels;

  std::ostream* os = IO::get_ostream(output_file);
  boost::timer::cpu_timer t;
  for (const auto& data: test_dataset) {
    Dependency dependency;
    size_t L = data.forms.size();
    transduce_instance_to_dependency(data, dependency, true);

    std::vector<Action> oracle_actions;

    State init_state(&dependency);
    StructuredDecoder::const_decode_result_t result =
      search->decode(init_state, oracle_actions, 2 * L- 1);

    std::vector<int> heads;
    std::vector<std::string> deprels;

    heads.resize(L); deprels.resize(L);
    for (size_t i = 1; i < L; ++ i) {
      heads[i] = result.first->heads[i];
      deprels[i] = deprels_alphabet.decode(result.first->deprels[i]);
    }

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
  _INFO << "#: elapsed " << t.elapsed().wall/1e9;
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

  extractor.initialize(
      forms_alphabet.encode(SpecialOption::NIL),
      forms_alphabet.size(),
      postags_alphabet.encode(SpecialOption::NIL),
      postags_alphabet.size(),
      deprels_alphabet.encode(SpecialOption::NIL),
      deprels_alphabet.size());

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

void Pipe::load_structured_model(const std::string& model_path) {
  std::ifstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "#: failed to open file.";
    return;
  }
  model.load(mfs);
}

void Pipe::save_structured_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "#: failed to open file.";
    return;
  }
  model.save(mfs);
}

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar
