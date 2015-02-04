#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "experimental/hc_search/arceager/pipe.h"
#include "experimental/hc_search/arceager/action_utils.h"

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
  if (phase_one_load_model(phase_one_model_path) &&
      phase_two_load_model(phase_two_model_path)) {
    _INFO << "report(2): model " << phase_one_model_path << " is loaded.";
    _INFO << "report(2): model " << phase_two_model_path << " is loaded.";
  } else {
    _INFO << "report(2): model " << phase_one_model_path << " is not loaded.";
    _INFO << "report(2): model " << phase_two_model_path << " is not loaded.";
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
    _INFO << "report(t): model is loaded.";
  } else {
    _INFO << "report(t): model is not loaded.";
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

bool
Pipe::setup() {
  namespace ioutils = ZuoPar::IO;
  dataset.clear();
  if (mode_ext == kPipeLearnPhaseOne || mode_ext == kPipeLearnPhaseTwo) {
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
  return true;
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
  } else if (mode_ext == kPipeLearnPhaseTwo) {
    cost_learner = new CostLearner(cost_weight, this->algorithm);
  }

  size_t N = dataset.size();
  std::ostream* os = (mode_ext == kPipeTest? ioutils::get_ostream(output_path.c_str()): NULL);

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  int nr_total = 0;
  int nr_correct = 0;
  int nr_gold_in_beam = 0;
  int nr_gold_in_beam_not_best = 0;
  int nr_correct_in_beam = 0;
  int nr_correct_in_beam_not_best = 0;

  for (size_t n = 0; n < N; ++ n) {
    const Dependency& instance = dataset[ranks[n]];

    std::vector<Action> actions;
    if (mode_ext == kPipeLearnPhaseOne ||
        mode_ext == kPipeLearnPhaseTwo ||
        mode_ext == kPipeEvaluate) {
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
    } else if (mode_ext == kPipeLearnPhaseTwo) {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);
      floatval_t corret_score = cost_weight->score((*result.second), false);
      for (const State* candidate_result: final_results) {
        ++ nr_total;
        Dependency output;
        build_output((*candidate_result), output);
        int l = loss(output, instance, true);
        if (l != 0) {
          floatval_t predict_score = cost_weight->score((*candidate_result), false);
          if (predict_score >= corret_score) {
            cost_learner->set_timestamp(nr_total);
            cost_learner->learn(candidate_result, result.second, l);
          }
        }
      }
    } else if (mode_ext == kPipeEvaluate) {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      bool gold_in_beam = false;
      bool correct_in_beam = false;
      for (const State* candidate_result: final_results) {
        Dependency output;
        build_output((*candidate_result), output);
        if (0 == loss(output, instance, true)) { correct_in_beam = true; }
        if (candidate_result == result.second) { gold_in_beam = true; }
      }

      if (gold_in_beam) { ++ nr_gold_in_beam; }
      if (correct_in_beam) { ++ nr_correct_in_beam; }
      if (gold_in_beam && result.first != result.second) { ++ nr_gold_in_beam_not_best; }
      Dependency output;
      build_output((*result.first), output);
      if (correct_in_beam && 0 != loss(output, instance, true)) { ++ nr_correct_in_beam_not_best; }
    } else {
      std::vector<const State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      floatval_t best_score = -1.;
      const State* best_result = NULL;
      if (rerank) {
        for (const State* candidate_result: final_results) {
          floatval_t score = cost_weight->score((*candidate_result), true);
          if (score > best_score) {
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
  } else if (mode_ext == kPipeLearnPhaseTwo) {
    cost_learner->set_timestamp(nr_total);
    cost_learner->flush();
    _INFO << "pipe: nr errors: " << cost_learner->errors() << "/" << nr_total;
    phase_two_save_model(phase_two_model_path);
  } else if (mode_ext == kPipeEvaluate) {
    _INFO << "report: gold in beam: " << nr_gold_in_beam <<
      "/" << N << "=" << floatval_t(nr_gold_in_beam)/N;
    _INFO << "report: gold in beam (not best): " << nr_gold_in_beam_not_best
      << "/" << N << "=" << floatval_t(nr_gold_in_beam_not_best)/N;
    _INFO << "report: correct in beam: " << nr_correct_in_beam
      << "/" << N << "=" << floatval_t(nr_correct_in_beam)/N;
    _INFO << "report: correct in beam (not best): " << nr_correct_in_beam_not_best
      << "/" << N << "=" << floatval_t(nr_correct_in_beam_not_best)/N;
  }
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
Pipe::loss(const Dependency& predict, const Dependency& oracle, bool labeled) {
  int N = predict.size();
  int err = 0;
  for (int i = 0; i < N; ++ i) {
    if ((predict.heads[i] != oracle.heads[i]) ||
        (labeled && predict.deprels[i] != oracle.deprels[i])) {
      ++ err;
    }
  }
  return err;
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
