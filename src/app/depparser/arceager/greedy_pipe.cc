#include <fstream>
#include "app/depparser/arceager/greedy_pipe.h"
#include "app/depparser/arceager/action_utils.h"
#include "utils/io/dataset/dependency.h"
#include "utils/io/instance/dependency.h"
#include "utils/io/stream.h"
#include "utils/logging.h"
#include "utils/misc.h"
#include "types/internal/packed_scores.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

GreedySearchPipe::GreedySearchPipe(const GreedyLearnOption& opt):
  mode(kPipeLearn),
  weight(nullptr), decoder(nullptr),
  reference_path(opt.reference_path), model_path(opt.model_path),
  shuffle_times(opt.shuffle_times),
  root(opt.root) {

  if (load_model(opt.model_path)) { _INFO << "report: model is loaded.";}
  else                            { _INFO << "report: model is not loaded.";}
  _INFO << "report: root = " << root;
}

GreedySearchPipe::GreedySearchPipe(const GreedyTestOption& opt):
  mode(kPipeTest),
  weight(nullptr), decoder(nullptr),
  input_path(opt.input_path), output_path(opt.output_path), model_path(opt.model_path),
  shuffle_times(0) {
  if (load_model(opt.model_path)) { _INFO << "report: model is loaded.";}
  else                            { _INFO << "report: model is not loaded.";}
  _INFO << "report: root = " << root;
}

GreedySearchPipe::~GreedySearchPipe() {
  if (weight) { delete weight; }
  if (decoder) { delete decoder; }
}

bool GreedySearchPipe::setup() {
  dataset.clear();
  if (mode == kPipeLearn) {
    std::ifstream ifs(reference_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: failed to open reference file, training halt.";
      return false;
    }
    _INFO << "report: loading dataset from reference file.";
    IO::read_conllx_dependency_dataset(ifs, dataset, forms_alphabet,
        lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
        deprels_alphabet);
    _INFO << "report: dataset is loaded.";
  } else {
    std::ifstream ifs(input_path.c_str());
    if (!ifs.good()) {
      _ERROR << "#: Failed to open input file, testing halt";
      return false;
    }
    IO::read_conllx_dependency_dataset(ifs, dataset, forms_alphabet,
        lemmas_alphabet, cpostags_alphabet, postags_alphabet, feat_alphabet,
        deprels_alphabet, 0x1f);
  }
}

void GreedySearchPipe::test() {
  if (!setup()) { return; }

  auto N = dataset.size();
  decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
      Decoder::kLeft, 1, false, kEarlyUpdate, nullptr);

  std::ostream* os = IO::get_ostream(output_path.c_str());
  for (auto n = 0; n < N; ++ n) {
    const CoNLLXDependency& instance = dataset[n];
    auto L = instance.forms.size();
    std::vector<State> states(L* 2);
    states[0].copy(State(&instance));
    decoder->transit(states[0], ActionFactory::make_shift(), 0, &states[1]);
    for (auto step = 1; step < L*2-1; ++ step) {
      std::vector<Action> possible_actions;
      PackedScores<Action> scores;
      decoder->get_possible_actions(states[step], possible_actions);
      weight->batchly_score(states[step], possible_actions, true, scores);

      Action best_action; floatval_t best_score = -1e20;
      for (auto& act: possible_actions) {
        if (scores[act] > best_score) {
          best_action = act;
          best_score = scores[act];
        }
      }

      decoder->transit(states[step], best_action, 0, &states[step+ 1]);
    }

    CoNLLXDependency output;
    build_output(states[L*2-1], output);
    IO::write_dependency_instance((*os), output, forms_alphabet,
        postags_alphabet, deprels_alphabet);
  }
  if (os != &(std::cout) && os != NULL) { delete os; }
}

void GreedySearchPipe::learn() {
  if (!setup()) { return; }

  decoder = new Decoder(deprels_alphabet.size(), deprels_alphabet.encode(root),
      Decoder::kLeft, 1, false,
      kEarlyUpdate, nullptr);

  auto N = dataset.size();
  auto n_samples = 0;
  std::vector<size_t> ranks;
  Utility::shuffle(N, shuffle_times, ranks);
  for (int n = 0; n < N; ++ n) {
    const CoNLLXDependency& instance = dataset[ranks[n]];
    std::vector<Action> oracle_actions;
    std::vector<Action> possible_actions;
    ActionUtils::get_oracle_actions(instance, oracle_actions);

    // size_t L = data.forms.size();
    std::vector<State> states(oracle_actions.size() + 1);
    states[0].copy(State(&instance));
    // The third parameter is not needed, put zero to it.
    decoder->transit(states[0], ActionFactory::make_shift(), 0, &states[1]);
    for (auto step = 1; step < oracle_actions.size(); ++ step) {
      n_samples += 1;

      auto& oracle_action = oracle_actions[step];
      PackedScores<Action> scores;

      decoder->get_possible_actions(states[step], possible_actions);
      weight->batchly_score(states[step], possible_actions, false, scores);

      Action best_action; floatval_t best_score = -1e20;
      for (auto& act: possible_actions) {
        if (scores[act] > best_score) {
          best_action = act;
          best_score = scores[act];
        }
      }

      if (best_action != oracle_action) {
        weight->update(states[step], best_action, n_samples, -1);
        weight->update(states[step], oracle_action, n_samples, 1);
      }

      decoder->transit(states[step], oracle_action, 0, &states[step+ 1]);
    }
    if ((n+ 1) % 1000 == 0) { _INFO << "report: processed " << n+1 << " instances."; }
  }

  save_model(model_path);
}

void GreedySearchPipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save model.";
  } else {
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    weight->save(mfs);

    boost::archive::text_oarchive oa(mfs);
    oa << root;
    _INFO << "pipe: model is saved to " << model_path;
  }
}

bool GreedySearchPipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: model doesn't exists.";             return false; }
  if (!forms_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load forms alphabet.";    return false; }
  if (!postags_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load postags alphabet.";  return false; }
  if (!deprels_alphabet.load(mfs)) {
    _WARN << "pipe: failed to load deprels alphabet.";  return false; }
  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load weight.";            return false; }

  boost::archive::text_iarchive ia(mfs);
  ia >> root;
  return true;
}

void GreedySearchPipe::build_output(const State& source, CoNLLXDependency& output) {
  size_t len = source.ref->size();
  output.forms = source.ref->forms;
  output.postags = source.ref->postags;
  output.heads.resize(len);
  output.deprels.resize(len);

  for (size_t i = 0; i < len; ++ i) {
    output.heads[i] = source.heads[i];
    output.deprels[i] = source.deprels[i];
    if (output.heads[i] == -1) {
      output.deprels[i] = deprels_alphabet.encode(root);
    }
  }
}

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar
