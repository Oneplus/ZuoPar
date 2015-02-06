#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "experimental/hc_search/arceager/pipe.h"
#include "experimental/hc_search/arceager/action_utils.h"
#include "experimental/hc_search/arceager/knowledge.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {

Pipe::Pipe(const LearnOneOption& opts)
  : mode_ext(kPipeLearnPhaseOne),
  heuristic_weight(0),
  heuristic_learner(0),
  cost_weight(0),
  cost_learner(0),
  decoder(0),
  fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
  root = opts.root;
  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _INFO << "report(1): model " << phase_one_model_path << " is loaded.";
  } else {
    _INFO << "report(1): model " << phase_one_model_path << " is not loaded.";
  }
}

Pipe::Pipe(const LearnTwoOption& opts)
  : mode_ext(kPipeLearnPhaseTwo),
  heuristic_weight(0),
  heuristic_learner(0),
  cost_weight(0),
  cost_learner(0),
  decoder(0),
  fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {
  root = opts.root;
  phase_one_model_path = opts.phase_one_model_path;
  phase_two_model_path = opts.phase_two_model_path;
  if (phase_one_load_model(phase_one_model_path) && phase_two_load_model(phase_two_model_path)) {
    _INFO << "report(2): model " << phase_two_model_path << " is loaded.";
  } else {
    _INFO << "report(2): model " << phase_two_model_path << " is not loaded.";
  }
}

Pipe::Pipe(const PrepareTwoOption& opts)
  : mode_ext(kPipePreparePhaseTwo),
  heuristic_weight(0),
  heuristic_learner(0),
  cost_weight(0),
  cost_learner(0),
  decoder(0),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)){
  root = opts.root;
  oracle = opts.oracle;
  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _INFO << "report(p): model is loaded.";
  } else {
    _INFO << "report(p): model is not loaded.";
  }
}

Pipe::Pipe(const TestOption& opts)
  : mode_ext(kPipeTest),
  heuristic_weight(0),
  heuristic_learner(0),
  cost_weight(0),
  cost_learner(0),
  decoder(0),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)){
  rerank = opts.rerank;
  root = opts.root;
  phase_one_model_path = opts.phase_one_model_path;
  phase_two_model_path = opts.phase_two_model_path;
  if (!rerank && phase_one_load_model(phase_one_model_path)) {
    _INFO << "report(no-rerank): model is loaded.";
  } else if (rerank
      && phase_one_load_model(phase_one_model_path)
      && phase_two_load_model(phase_two_model_path)) {
    _INFO << "report(rerank): model is loaded.";
  } else {
    _INFO << "report(t): model is not loaded.";
  }
}

Pipe::Pipe(const EvaluateOption& opts)
  : mode_ext(kPipeEvaluate),
  heuristic_weight(0),
  heuristic_learner(0),
  cost_weight(0),
  cost_learner(0),
  decoder(0),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)){
  root = opts.root;
  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _INFO << "report(e): model is loaded.";
  } else {
    _INFO << "report(e): model is not loaded.";
  }
}

bool
Pipe::phase_one_load_model(const std::string& phase_one_model_path) {
  heuristic_weight = new HeuristicWeight;

  std::ifstream mfs(phase_one_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exist.";
    return false;
  }
  if (!forms_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";
    return false;
  }
  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";
    return false;
  }
  if (!deprels_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load deprels alphabet.";
    return false;
  }
  if (!heuristic_weight->load(mfs)) {
    _WARN << "pipe: failed to load phase one weight.";
    return false;
  }
  return true;
}

bool
Pipe::phase_two_load_model(const std::string& phase_two_model_path) {
  cost_weight = new CostWeight;
  std::ifstream mfs(phase_two_model_path);

  if (!mfs.good()) {
    _WARN << "pipe: phase two model doesn't exist.";
    return false;
  }

  if (!cost_weight->load(mfs)) {
    _WARN << "pipe: failed to load phase two weight.";
    return false;
  }
  return true;
}

void
Pipe::phase_one_save_model(const std::string& phase_one_model_path) {
  std::ofstream mfs(phase_one_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save phase(1) model.";
  } else {
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    heuristic_weight->save(mfs);
    _INFO << "pipe: phase(1) model saved to " << phase_one_model_path;
  }
}

void
Pipe::phase_two_save_model(const std::string& phase_two_model_path) {
  std::ofstream mfs(phase_two_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save phase(2) model.";
  } else {
    cost_weight->save(mfs);
    _INFO << "pipe: phase(2) model saved to " << phase_two_model_path;
  }
}

void
Pipe::build_knowledge() {
  punctuation_forms.insert(forms_alphabet.encode("."));
  punctuation_forms.insert(forms_alphabet.encode(","));
  punctuation_forms.insert(forms_alphabet.encode("?"));
  punctuation_forms.insert(forms_alphabet.encode("!"));
  punctuation_forms.insert(forms_alphabet.encode(";"));
  punctuation_forms.insert(forms_alphabet.encode("-LRB-"));
  punctuation_forms.insert(forms_alphabet.encode("-RRB-"));
  punctuation_forms.insert(forms_alphabet.encode("`"));
  punctuation_forms.insert(forms_alphabet.encode("``"));
  punctuation_forms.insert(forms_alphabet.encode("'"));
  punctuation_forms.insert(forms_alphabet.encode("''"));

  punctuation_postags.insert(postags_alphabet.encode("."));
  punctuation_postags.insert(postags_alphabet.encode(","));
  punctuation_postags.insert(postags_alphabet.encode("?"));
  punctuation_postags.insert(postags_alphabet.encode("!"));
  punctuation_postags.insert(postags_alphabet.encode(";"));
  punctuation_postags.insert(postags_alphabet.encode("-LRB-"));
  punctuation_postags.insert(postags_alphabet.encode("-RRB-"));
  punctuation_postags.insert(postags_alphabet.encode("`"));
  punctuation_postags.insert(postags_alphabet.encode("``"));
  punctuation_postags.insert(postags_alphabet.encode("'"));
  punctuation_postags.insert(postags_alphabet.encode("''"));
}

bool
Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  dataset.clear();
  if (mode_ext == kPipeLearnPhaseOne) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file.";
      _ERROR << "#: training halt.";
      return false;
    }
    _INFO << "report: loading dataset from reference file.";
    ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet);
  } else {
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open input file.";
      _ERROR << "#: testing halt.";
      return false;
    }
    ioutils::read_dependency_dataset(ifs, dataset, forms_alphabet,
        postags_alphabet, deprels_alphabet, ((1<<1)|(1<<2)));
  }
  _INFO << "report: " << dataset.size() << " instance(s) is loaded.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is loaded.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is loaded.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is loaded.";
  build_knowledge();
  return true;
}

bool
Pipe::read_dataset2() {
  namespace algo = boost::algorithm;

  std::ifstream ifs(reference_path.c_str());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file.";
    _ERROR << "#: training halt.";
    return false;
  }
  _INFO << "report: loading dataset from reference file.";

  dataset2.clear();
  std::string data_context((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  boost::regex INSTANCE_DELIMITER("\n\n");
  boost::sregex_token_iterator instance(data_context.begin(),
      data_context.end(), INSTANCE_DELIMITER, -1);
  boost::sregex_token_iterator eos;

  // Loop over the instances
  size_t n = 0;
  while (instance != eos) {
    std::istringstream iss(*instance);
    Dependency predict, oracle;

    std::string item_context;
    while (std::getline(iss, item_context)) {
      std::vector<std::string> items;
      algo::trim(item_context);
      algo::split(items, item_context, boost::is_any_of("\t "), boost::token_compress_on);

      // Format: form postag gold-head gold-deprel predict-head predict-deprel
      form_t form = forms_alphabet.encode(items[0].c_str());
      postag_t postag = postags_alphabet.encode(items[1].c_str());
      deprel_t oracle_deprel = deprels_alphabet.encode(items[3].c_str());
      deprel_t predict_deprel = deprels_alphabet.encode(items[5].c_str());
      oracle.push_back(form, postag, atoi(items[2].c_str()), oracle_deprel);
      predict.push_back(form, postag, atoi(items[4].c_str()), predict_deprel);
    }

    dataset2.push_back(std::make_pair(predict, oracle));
    if ((n+ 1) % display_interval == 0) {
      _INFO << "pipe: loaded #" << (n+ 1) << " instances.";
    }
    ++ n;
    instance ++;
  }
  _INFO << "report: load #" << dataset2.size() << " instances.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is found.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is found.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is found.";

  return true;
}

void
Pipe::learn2() {
  namespace ioutils = ZuoPar::IO;
  if (mode_ext != kPipeLearnPhaseTwo) {
    _WARN << "Pipe: learn phase two not specified.";
    return;
  }

  if (!read_dataset2()) {
    return;
  }

  size_t N = dataset2.size();
  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  cost_learner = new CostLearner(cost_weight, this->algorithm);

  for (size_t n = 0; n < N; ++ n) {
    const Dependency& predict = dataset2[ranks[n]].first;
    const Dependency& oracle = dataset2[ranks[n]].second;
    State predict_state, oracle_state;
    predict_state.build(predict);
    oracle_state.build(oracle);

    floatval_t predict_score = cost_weight->score(predict_state, false);
    floatval_t oracle_score = cost_weight->score(oracle_state, false);

    int pseudo;
    int l = loss(predict, oracle, true, true, pseudo);
    _TRACE << "loss: " << l;
    if (l != 0 && predict_score >= oracle_score - 1e-8) {
      cost_learner->set_timestamp(n+ 1);
      cost_learner->learn(&predict_state, &oracle_state, l,
          &oracle_score, &predict_score);
    }

    if ((n+ 1) % display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  cost_learner->set_timestamp(N);
  cost_learner->flush();

  _INFO << "pipe: nr errors: " << cost_learner->errors() << "/" << N;
  phase_two_save_model(phase_two_model_path);
}

void
Pipe::run() {
  namespace ioutils = ZuoPar::IO;
  if (!setup()) { return; }

  deprel_t root_tag = deprels_alphabet.encode(root.c_str());
  if (mode_ext == kPipeLearnPhaseOne) {
    decoder = new Decoder(deprels_alphabet.size(), root_tag,
        beam_size, false, update_strategy, heuristic_weight);
  } else {
    decoder = new Decoder(deprels_alphabet.size(), root_tag,
        beam_size, true, UpdateStrategy::kNaive, heuristic_weight);
  }

  if (mode_ext == kPipeLearnPhaseOne) {
    heuristic_learner = new HeuristicLearner(heuristic_weight, this->algorithm);
  }

  size_t N = dataset.size();
  std::ostream* os = (mode_ext == kPipeTest|| mode_ext == kPipePreparePhaseTwo?
      ioutils::get_ostream(output_path.c_str()): NULL);

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  int nr_correct_in_beam = 0;
  int nr_correct_in_beam_not_best = 0;
  int nr_oracle_recalled_heads = 0;
  int nr_oracle_recalled_deprels = 0;
  int nr_deprels = 0;

  for (size_t n = 0; n < N; ++ n) {
    const Dependency& instance = dataset[ranks[n]];

    std::vector<Action> actions;
    if (mode_ext == kPipeLearnPhaseOne || mode_ext == kPipeEvaluate) {
      ActionUtils::get_oracle_actions(instance, actions);
    }

    int max_nr_actions = instance.size() * 2 - 1;
    State init_state(instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state, actions,
        max_nr_actions);

    if (mode_ext == kPipeLearnPhaseOne) {
      if (!result.first->complete()) {
        heuristic_learner->set_timestamp(n+ 1);
        heuristic_learner->learn(result.first, result.second);
      }
    } else if (mode_ext == kPipePreparePhaseTwo) {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);
      if (oracle) {
        for (const State* candidate_result: final_results) {
          write_prepared_data((*candidate_result), (*result.second), (*os));
        }
      } else {
        int best_loss = instance.size(), dummy;
        for (const State* candidate_result: final_results) {
          Dependency output; build_output((*candidate_result), output);
          int loss1 = loss(output, instance, true, true, dummy);
          if (loss1 < best_loss) { best_loss = loss1; }
        }
        std::vector<const State*> Y_good;
        std::vector<const State*> Y_bad;
        for (const State* candidate_result: final_results) {
          Dependency output; build_output((*candidate_result), output);
          int loss1 = loss(output, instance, true, true, dummy);
          if (loss1 == best_loss) { Y_good.push_back(candidate_result); }
          else { Y_bad.push_back(candidate_result); }
        }

        for (const State* good: Y_good) {
          for (const State* bad: Y_bad) {
            write_prepared_data((*good), (*bad), (*os));
          }
        }
      }
    } else if (mode_ext == kPipeEvaluate) {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      bool correct_in_beam = false;
      bool correct_in_beam_not_best = false;
      int oracle_recalled_heads = 0;
      int oracle_recalled_deprels = 0;
      int nr_effective_tokens = 0;
      int loss1, loss2;
      for (const State* candidate_result: final_results) {
        Dependency output; build_output((*candidate_result), output);
        loss1 = loss(output, instance, true, true, nr_effective_tokens);
        loss2 = loss(output, instance, false, true, nr_effective_tokens);

        if (0 == loss1) {
          correct_in_beam = true;
          if (candidate_result != result.first) {
            correct_in_beam_not_best = true;
          }
        }

        if (oracle_recalled_heads < nr_effective_tokens- loss2) {
          oracle_recalled_heads = nr_effective_tokens- loss2;
        }

        if (oracle_recalled_deprels < nr_effective_tokens- loss1) {
          oracle_recalled_deprels = nr_effective_tokens- loss1;
        }
      }

      nr_oracle_recalled_heads += oracle_recalled_heads;
      nr_oracle_recalled_deprels += oracle_recalled_deprels;
      nr_deprels += nr_effective_tokens;

      if (correct_in_beam) {
        ++ nr_correct_in_beam;
        if (correct_in_beam_not_best) { ++ nr_correct_in_beam_not_best; }
      }
    } else {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      const State* best_result = NULL;
      if (rerank) {
        floatval_t best_score = -1.;
        for (const State* candidate_result: final_results) {
          floatval_t score = cost_weight->score((*candidate_result), true);
          if (best_result == NULL || score > best_score) {
            best_score = score;
            best_result = candidate_result;
          }
        }
      } else {
        best_result = result.first;
      }
      Dependency output;
      build_output((*best_result), output);
      ioutils::write_dependency_instance((*os), output, forms_alphabet,
          postags_alphabet, deprels_alphabet);
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode_ext == kPipeLearnPhaseOne) {
    heuristic_learner->set_timestamp(N);
    heuristic_learner->flush();
    _INFO << "pipe: nr errors: " << heuristic_learner->errors();
    phase_one_save_model(phase_one_model_path);
  } else if (mode_ext == kPipeEvaluate) {
    _INFO << "report: correct in beam: " << nr_correct_in_beam
      << "/" << N << "=" << floatval_t(nr_correct_in_beam)/N;
    _INFO << "report: correct in beam (not best): " << nr_correct_in_beam_not_best
      << "/" << nr_correct_in_beam
      << "=" << floatval_t(nr_correct_in_beam_not_best)/nr_correct_in_beam;
    _INFO << "report: UAS upperbound: " << nr_oracle_recalled_heads
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_recalled_heads)/nr_deprels;
    _INFO << "report: LAS upperbound: " << nr_oracle_recalled_deprels
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_recalled_deprels)/nr_deprels;
  }
}

void
Pipe::write_prepared_data(const State& good, const State& bad, std::ostream& os) {
  size_t len = good.ref->size();
  // Format: form postag gold-head gold-deprel predict-head predict-deprel
  for (size_t i = 0; i < len; ++ i) {
    os << forms_alphabet.decode(good.ref->forms[i]) << "\t"
       << postags_alphabet.decode(good.ref->postags[i]) << "\t"
       << good.heads[i] << "\t";
    if (good.heads[i] == -1) { os << root << "\t"; }
    else { os << deprels_alphabet.decode(good.deprels[i]) << "\t"; }
    os << bad.heads[i] << "\t";
    if (bad.heads[i] == -1) { os << root << std::endl; }
    else { os << deprels_alphabet.decode(bad.deprels[i]) << std::endl; }
  }
  os << std::endl;
}

void
Pipe::build_output(const State& source, Dependency& output) {
  output.forms = source.ref->forms;
  output.postags = source.ref->postags;
  size_t len = source.ref->size();
  if (output.heads.size() != len) { output.heads.resize(len); }
  if (output.deprels.size() != len) { output.deprels.resize(len); }
  for (size_t i = 0; i < len; ++ i) {
    output.heads[i] = source.heads[i];
    output.deprels[i] = source.deprels[i];
    if (output.heads[i] == -1) {
      output.deprels[i] = deprels_alphabet.encode(root.c_str());
    }
  }
}

int
Pipe::loss(const Dependency& predict, const Dependency& oracle,
    bool labeled, bool ignore_punctuation, int& nr_effective_tokens) {
  int N = predict.size();
  int err = 0;

  nr_effective_tokens = 0;
  for (int i = 0; i < N; ++ i) {
    if (ignore_punctuation && is_punctuation(predict.forms[i])) {
      continue;
    }
    ++ nr_effective_tokens;
    if ((predict.heads[i] != oracle.heads[i]) ||
        (labeled && predict.deprels[i] != oracle.deprels[i])) {
      ++ err;
    }
  }
  return err;
}

bool Pipe::is_punctuation(const char* token) {
  return boost::regex_match(token,
      boost::regex("^[,?!:;]$|^-LRB-$|^-RRB-$|^[.]+$|^[`]+$|^[']+$"));
}

bool
Pipe::is_punctuation(const form_t& form) {
  const char *token = forms_alphabet.decode(form);
  if (NULL == token) { return false; }
  return is_punctuation(token);
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
