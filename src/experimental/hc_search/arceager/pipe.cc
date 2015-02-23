#include "utils/io/stream.h"
#include "utils/io/dataset/dependency.h"
#include "experimental/hc_search/arceager/pipe.h"
#include "experimental/hc_search/arceager/action_utils.h"
#include "experimental/hc_search/arceager/knowledge.h"
#include <boost/lexical_cast.hpp>
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

  if (opts.method == "best") {
    learn_one_method = kPipeLearnOneBest;
  } else if (opts.method == "worst") {
    learn_one_method = kPipeLearnOneWorst;
  } else if (opts.method == "random") {
    learn_one_method = kPipeLearnOneRandom;
  } else if (opts.method == "regular") {
    learn_one_method = kPipeLearnOneRegular;
  }
  _INFO << "report: (phase#1.learn) learning method = " << opts.method;

  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _INFO << "report: (phase#1.learn) model " << phase_one_model_path << " is loaded.";
  } else {
    _INFO << "report: (phase#1.learn) model " << phase_one_model_path << " is not loaded.";
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

  language = opts.language;
  _INFO << "report: (phase#2.learn) language = " << language;

  if (opts.method == "or") { learn_two_method = kPipeLearnTwoOracleAgainstRest; }
  else if (opts.method == "ngb") {  learn_two_method = kPipeLearnTwoNaiveGoodAgainstBad;  }
  else if (opts.method == "rgb") {  learn_two_method = kPipeLearnTwoRelexedGoodAgainstBad; }
  _INFO << "report: (phase#2.learn) learning method = " << opts.method;

  ignore_punctuation = opts.ignore_punctuation;
  _INFO << "report: (phase#2.learn) " << (opts.ignore_punctuation ? "" : "not ")
    << "ignore punctuation when calculating loss.";

  extract_punctuation = opts.extract_punctuation;
  _INFO << "report: (phase#2.learn) " << (opts.extract_punctuation ? "" : "not ")
    << "extract punctuation when feature extraction.";

  phase_one_model_path = opts.phase_one_model_path;
  phase_two_model_path = opts.phase_two_model_path;
  if (phase_one_load_model(phase_one_model_path) && phase_two_load_model(phase_two_model_path)) {
    _INFO << "report: (phase#2.learn) model " << phase_two_model_path << " is loaded.";
  } else {
    _INFO << "report: (phase#2.learn) model " << phase_two_model_path << " is not loaded.";
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
  language = opts.language;
  _INFO << "report: (phase#2.prepare) language = " << language;
  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _INFO << "report: (phase#2.prepare) model is loaded.";
  } else {
    _INFO << "report: (phase#2.prepare) model is not loaded.";
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
  root = opts.root;

  extract_punctuation = opts.extract_punctuation;
  _INFO << "report: (phase#2.learn) " << (opts.extract_punctuation ? "" : "not ")
    << "extract punctuation when feature extraction.";

  language = opts.language;
  _INFO << "report: (test) language = " << language;
  phase_one_model_path = opts.phase_one_model_path;
  phase_two_model_path = opts.phase_two_model_path;
  if (!rerank && phase_one_load_model(phase_one_model_path)) {
    _INFO << "report: (test) model is loaded.";
    _INFO << "report: (test) reranking will not be performed.";
  } else if (rerank
      && phase_one_load_model(phase_one_model_path)
      && phase_two_load_model(phase_two_model_path)) {
    _INFO << "report: (test) model is loaded.";
    _INFO << "report: (test) reranking will be performed.";
  } else {
    _ERROR << "report: (test) model is not loaded.";
    exit(1);
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
  language = opts.language;
  _INFO << "report: (evaluate) language = " << language;
  phase_one_model_path = opts.phase_one_model_path;
  if (phase_one_load_model(phase_one_model_path)) {
    _ERROR << "report: (evaluate) model is loaded.";
    exit(1);
  } else {
    _INFO << "report: (evaluate) model is not loaded.";
  }
}

Pipe::~Pipe() {
  if (heuristic_learner) { delete heuristic_learner; }
  if (heuristic_weight)  { delete heuristic_weight; }
  if (cost_learner)      { delete cost_learner; }
  if (cost_weight)       { delete cost_weight; }
  if (decoder)           { delete decoder; }
}

bool
Pipe::phase_one_load_model(const std::string& phase_one_model_path) {
  heuristic_weight = new HeuristicWeight;

  std::ifstream mfs(phase_one_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: Phase#1 model doesn't exist.";
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
    _WARN << "pipe: failed to load Phase#1 weight.";
    return false;
  }
  mfs.close();
  return true;
}

bool
Pipe::phase_two_load_model(const std::string& phase_two_model_path) {
  cost_weight = new CostWeight;
  std::ifstream mfs(phase_two_model_path);

  if (!mfs.good()) {
    _WARN << "pipe: phase#2 model doesn't exist.";
    return false;
  }
  if (!cost_weight->load(mfs)) {
    _WARN << "pipe: failed to load Phase#2 weight.";
    return false;
  }
  mfs.close();
  return true;
}

void
Pipe::phase_one_save_model(const std::string& phase_one_model_path) {
  std::ofstream mfs(phase_one_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save Phase#1 model.";
  } else {
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    heuristic_weight->save(mfs);
    _INFO << "pipe: Phase#1 model saved to " << phase_one_model_path;
  }
  mfs.close();
}

void
Pipe::phase_two_save_model(const std::string& phase_two_model_path) {
  std::ofstream mfs(phase_two_model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save Phase#2 model.";
  } else {
    cost_weight->save(mfs);
    _INFO << "pipe: Phase#2 model saved to " << phase_two_model_path;
  }
  mfs.close();
}

void
Pipe::build_knowledge() {
  if (language == "en") {
    // punctuation
    PUNC_POS.insert(postags_alphabet.encode("``"));
    PUNC_POS.insert(postags_alphabet.encode(","));
    PUNC_POS.insert(postags_alphabet.encode(":"));
    PUNC_POS.insert(postags_alphabet.encode("."));
    PUNC_POS.insert(postags_alphabet.encode("''"));
    PUNC_POS.insert(postags_alphabet.encode("$"));
    PUNC_POS.insert(postags_alphabet.encode("#"));
    PUNC_POS.insert(postags_alphabet.encode("-LRB-"));
    PUNC_POS.insert(postags_alphabet.encode("-RRB-"));
    // Conjunction.
    CONJ_POS.insert(postags_alphabet.encode("CC"));
    // Preposition
    ADP_POS.insert(postags_alphabet.encode("IN"));
    ADP_POS.insert(postags_alphabet.encode("RP"));
    // Verb
    VERB_POS.insert(postags_alphabet.encode("MD"));
    VERB_POS.insert(postags_alphabet.encode("VB"));
    VERB_POS.insert(postags_alphabet.encode("VBD"));
    VERB_POS.insert(postags_alphabet.encode("VBN"));
    VERB_POS.insert(postags_alphabet.encode("VBG"));
    VERB_POS.insert(postags_alphabet.encode("VBP"));
    VERB_POS.insert(postags_alphabet.encode("VBZ"));
    VERB_POS.insert(postags_alphabet.encode("VP"));
    _INFO << "report: postag knowledge for English is built.";
  } else if (language == "ch") {
    // Punctuation
    PUNC_POS.insert(postags_alphabet.encode("PU"));
    // Conjunction.
    CONJ_POS.insert(postags_alphabet.encode("CC"));
    // Preposition
    ADP_POS.insert(postags_alphabet.encode("P"));
    //
    VERB_POS.insert(postags_alphabet.encode("VV"));
    VERB_POS.insert(postags_alphabet.encode("VA"));
    VERB_POS.insert(postags_alphabet.encode("VC"));
    VERB_POS.insert(postags_alphabet.encode("VE"));
    _INFO << "report: postag knowledge for Chinese is built.";
  } else {
    _INFO << "pipe: unknown language, no knowledge is built.";
  }
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
Pipe::setup2() {
  namespace algo = boost::algorithm;

  std::ifstream ifs(reference_path.c_str());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file.";
    _ERROR << "#: training halt.";
    return false;
  }
  _INFO << "report: loading dataset from reference file.";

  dataset2.clear();
  std::string instance, buffer;
  // Loop over the instances
  size_t n = 0;
  while (std::getline(ifs, buffer)) {
    algo::trim(buffer);
    if (buffer.size() == 0) {
      std::istringstream iss(instance);
      std::vector< std::vector<std::string> > mat;

      std::string item_context, header;
      std::getline(iss, header); // the header
      int nr_good = 0, nr_bad = 0;
      std::vector<std::string> items;
      std::vector< std::pair<bool, floatval_t> > info;
      algo::trim(header);
      algo::split(items, header, boost::is_any_of("\t "), boost::token_compress_on);
      for (const std::string& token: items) {
        if (algo::starts_with(token, "good:")) {
          ++ nr_good;
          floatval_t score =
            boost::lexical_cast<floatval_t>(token.substr(token.find_first_of(':')+ 1));
          info.push_back( std::make_pair(true, score) );
        }
        if (algo::starts_with(token, "bad:")) {
          ++ nr_bad;
          floatval_t score =
            boost::lexical_cast<floatval_t>(token.substr(token.find_first_of(':')+ 1));
          info.push_back( std::make_pair(false, score) );
        }
      }

      while (std::getline(iss, item_context)) {
        items.clear();
        algo::trim(item_context);
        algo::split(items, item_context, boost::is_any_of("\t "), boost::token_compress_on);
        mat.push_back(items);
      }

      bool bad_format = false; int col = mat[0].size();
      for (int r = 0; r < mat.size(); ++ r) {
        if (mat[r].size() != col) {
          _WARN << "Unmatched number of columns"; bad_format = true; }
        if (bad_format) { break; }
      }
      if (bad_format) { continue; }

      int N = mat.size(), M = mat[0].size();
      RerankingInstance ri;

      ri.init(N, nr_good, nr_bad);
      for (int rank = 0, ig = 0, ib = 0; rank < info.size(); ++ rank) {
        if (info[rank].first) {
          ri.good[ig].rank = rank+ 1;
          ri.good[ig].score = info[rank].second;
          ++ ig;
        } else {
          ri.bad[ib].rank = rank+ 1;
          ri.bad[ib].score = info[rank].second;
          ++ ib;
        }
      }
      for (int r = 0; r < mat.size(); ++ r) {
        ri.forms[r] = forms_alphabet.encode(mat[r][0].c_str());
        ri.postags[r] = postags_alphabet.encode(mat[r][1].c_str());

        size_t delimiter_position;
        delimiter_position = mat[r][2].find_first_of('/');
        ri.oracle.heads[r] = atoi(mat[r][2].substr(0, delimiter_position).c_str());
        ri.oracle.deprels[r] = deprels_alphabet.encode(
            mat[r][2].substr(delimiter_position+ 1).c_str());

        for (int c = 3, ig = 0, ib = 0; c < M; ++ c) {
          delimiter_position = mat[r][c].find_first_of('/');
          if (info[c- 3].first) {
            ri.good[ig].heads[r] = atoi(mat[r][c].substr(0, delimiter_position).c_str());
            ri.good[ig].deprels[r] = deprels_alphabet.encode(
                mat[r][c].substr(delimiter_position+ 1).c_str());
            ++ ig;
          } else {
            ri.bad[ib].heads[r] = atoi(mat[r][c].substr(0, delimiter_position).c_str());
            ri.bad[ib].deprels[r] = deprels_alphabet.encode(
                mat[r][c].substr(delimiter_position+ 1).c_str());
            ++ ib;
          }
        }
      }
      dataset2.push_back(ri);

      if ((n+ 1) % display_interval == 0) {
        _INFO << "pipe: loaded #" << (n+ 1) << " instances.";
      }
      ++ n;
      instance = "";
    } else {
      instance += buffer + "\n";
    }
  }
  _INFO << "report: load #" << dataset2.size() << " instances.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is found.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is found.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is found.";
  build_knowledge();
  return true;
}

void
Pipe::wrapper_to_instance(const DependencyWrapper& wrapper, Dependency& instance) {
  instance.forms= wrapper.forms;
  instance.postags = wrapper.postags;
  instance.heads = wrapper.heads;
  instance.deprels = wrapper.deprels;
}

void
Pipe::train_one_pair(const Dependency& oracle, const Dependency& good, const Dependency& bad,
    int good_rank, int bad_rank,
    floatval_t good_phase_one_score, floatval_t bad_phase_one_score, int timestamp) {
  State good_state, bad_state;
  good_state.build(good, good_rank, good_phase_one_score);
  bad_state.build(bad, bad_rank, bad_phase_one_score);

  floatval_t good_score = cost_weight->score(good_state, false);
  floatval_t bad_score = cost_weight->score(bad_state, false);

  int dummy;
  int good_loss = loss(good, oracle, true, ignore_punctuation, dummy);
  int bad_loss = loss(bad, oracle, true, ignore_punctuation, dummy);
  int delta_loss = bad_loss - good_loss;
  //_TRACE << "loss: " << loss1;

  if (delta_loss > 0 && good_score - bad_score < delta_loss - 1e-8) {
    cost_learner->set_timestamp(timestamp);
    cost_learner->learn(&bad_state, &good_state,
        delta_loss, &bad_score, &good_score);
  }
}

void
Pipe::run2_learn_naive(const std::vector< train_tuple_t >& train_data) {
  size_t N = train_data.size();

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  cost_learner = new CostLearner(cost_weight, this->algorithm);

  for (size_t n = 0; n < N; ++ n) {
    Dependency oracle_instance, good_instance, bad_instance;
    wrapper_to_instance(train_data[ranks[n]].get<0>(), oracle_instance);
    wrapper_to_instance(train_data[ranks[n]].get<1>(), good_instance);
    wrapper_to_instance(train_data[ranks[n]].get<2>(), bad_instance);

    train_one_pair(oracle_instance, good_instance, bad_instance, 1, 1, 0, 0, n+ 1);
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
Pipe::run2_learn_oracle_against_rest() {
  std::vector< train_tuple_t > train_data;

  for (const RerankingInstance& instance: dataset2) {
    DependencyWrapper oracle(instance.forms, instance.postags,
        instance.oracle.heads, instance.oracle.deprels);

    for (const RerankingInstanceParse& parse: instance.good) {
      train_data.push_back( boost::make_tuple(
            oracle,
            oracle,
            DependencyWrapper(instance.forms, instance.postags, parse.heads, parse.deprels)
            ) );
    }

    for (const RerankingInstanceParse& parse: instance.bad) {
      train_data.push_back( boost::make_tuple(
            oracle,
            oracle,
            DependencyWrapper(instance.forms, instance.postags, parse.heads, parse.deprels)
            ) );
    }
  }
  _INFO << "report: generate #" << train_data.size() << " training pairs.";

  run2_learn_naive(train_data);
};

void
Pipe::run2_learn_naive_good_against_bad() {
  std::vector< train_tuple_t > train_data;

  for (const RerankingInstance& instance: dataset2) {
    DependencyWrapper oracle(instance.forms, instance.postags,
        instance.oracle.heads, instance.oracle.deprels);
    for (const RerankingInstanceParse& good: instance.good) {
      for (const RerankingInstanceParse& bad: instance.bad) {
        train_data.push_back( boost::make_tuple(
              oracle,
              DependencyWrapper(instance.forms, instance.postags, good.heads, good.deprels),
              DependencyWrapper(instance.forms, instance.postags, bad.heads, bad.deprels)
              ) );
      }
    }
  }
  _INFO << "report: generate #" << train_data.size() << " training pairs.";
  run2_learn_naive(train_data);
};

void
Pipe::run2_learn_relaxed_good_against_bad() {
  size_t N = dataset2.size();

  std::vector<std::size_t> ranks;
  for (size_t n = 0; n < N; ++ n) { ranks.push_back(n); }
  while (shuffle_times --) { std::random_shuffle(ranks.begin(), ranks.end()); }

  cost_learner = new CostLearner(cost_weight, this->algorithm);

  for (size_t n = 0; n < N; ++ n) {
    const RerankingInstance& instance = dataset2[ranks[n]];

    int worst_good_position = -1, best_bad_position = -1;
    floatval_t highest_phase_one_score = -1e20;
    floatval_t lowest_phase_one_score = 1e20;
    for (const RerankingInstanceParse& parse: instance.good) {
      highest_phase_one_score = std::max(highest_phase_one_score, parse.score);
      lowest_phase_one_score = std::min(lowest_phase_one_score, parse.score);
    }
    for (const RerankingInstanceParse& parse: instance.bad) {
      highest_phase_one_score = std::max(highest_phase_one_score, parse.score);
      lowest_phase_one_score = std::min(lowest_phase_one_score, parse.score);
    }
    floatval_t delta = highest_phase_one_score - lowest_phase_one_score;

    floatval_t worst_good_score, best_bad_score;

    for (int i = 0; i < instance.good.size(); ++ i) {
      Dependency dependency(instance.forms, instance.postags,
          instance.good[i].heads, instance.good[i].deprels);
      State state;
      state.build(dependency, instance.good[i].rank,
          (instance.good[i].score- lowest_phase_one_score) / delta);
      floatval_t score = cost_weight->score(state, false);
      if (worst_good_position == -1 || score < worst_good_score) {
        worst_good_position = i;
        worst_good_score = score;
      }
    }

    for (int i = 0; i < instance.bad.size(); ++ i) {
      Dependency dependency(instance.forms, instance.postags,
          instance.bad[i].heads, instance.bad[i].deprels);
      State state;
      state.build(dependency, instance.bad[i].rank,
          (instance.bad[i].score-lowest_phase_one_score) / delta);
      floatval_t score = cost_weight->score(state, false);
      if (best_bad_position == -1 || score > best_bad_score) {
        best_bad_position = i;
        best_bad_score = score;
      }
    }

    if (best_bad_position == -1 || worst_good_position == -1) {
      _WARN << "best bad or worst good was not found.";
      _INFO << "# good = " << instance.good.size()
        << ", # bad = " << instance.bad.size()
        << ", id = " << n;
      continue;
    }

    Dependency oracle_instance(instance.forms, instance.postags,
        instance.oracle.heads, instance.oracle.deprels);
    Dependency good_instance(instance.forms, instance.postags,
        instance.good[worst_good_position].heads,
        instance.good[worst_good_position].deprels);
    Dependency bad_instance(instance.forms, instance.postags,
        instance.bad[best_bad_position].heads,
        instance.bad[best_bad_position].deprels);

    train_one_pair(oracle_instance, good_instance, bad_instance, 
        instance.good[worst_good_position].rank, instance.good[worst_good_position].score,
        instance.bad[best_bad_position].rank, instance.bad[best_bad_position].score,
        n+ 1);
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
Pipe::run2() {
  namespace ioutils = ZuoPar::IO;
  if (mode_ext != kPipeLearnPhaseTwo) {
    _WARN << "Pipe: learn phase two not specified.";
    return;
  }

  if (!setup2()) {
    return;
  }

  if (learn_two_method == kPipeLearnTwoOracleAgainstRest) {
    run2_learn_oracle_against_rest();
  } else if (learn_two_method == kPipeLearnTwoNaiveGoodAgainstBad) {
    run2_learn_naive_good_against_bad();
  } else if (learn_two_method == kPipeLearnTwoRelexedGoodAgainstBad) {
    run2_learn_relaxed_good_against_bad();
  } else {
    _ERROR << "unknown learning method.";
  }
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
  int nr_oracle_positive_recalled_heads = 0;
  int nr_oracle_negative_recalled_heads = 0;
  int nr_oracle_positive_recalled_deprels = 0;
  int nr_oracle_negative_recalled_deprels = 0;
  int nr_deprels = 0;
  floatval_t avg_uas = 0.;
  floatval_t avg_las = 0.;

  for (size_t n = 0; n < N; ++ n) {
    const Dependency& instance = dataset[ranks[n]];

    std::vector<Action> actions;
    if (mode_ext == kPipeLearnPhaseOne
        || mode_ext == kPipePreparePhaseTwo
        || mode_ext == kPipeEvaluate) {
      ActionUtils::get_oracle_actions(instance, actions);
    }

    int max_nr_actions = instance.size() * 2 - 1;
    State init_state(instance);
    Decoder::const_decode_result_t result = decoder->decode(init_state, actions,
        max_nr_actions);

    if (mode_ext == kPipeLearnPhaseOne) {
      if (learn_one_method == kPipeLearnOneRegular) {
        heuristic_learner->set_timestamp(n+ 1);
        heuristic_learner->learn(result.first, result.second);
      } else {
        int round = decoder->get_ending_round();
        std::vector<State*> final_results;
        decoder->get_results_in_beam(final_results, round);

        bool oracle_in_beam = false;
        for (const State* candidate_result: final_results) {
          if (candidate_result == result.second) {
            oracle_in_beam = true;
            break;
          }
        }

        if (!oracle_in_beam) {
          if (learn_one_method == kPipeLearnOneBest) {
            heuristic_learner->set_timestamp(n+ 1);
            heuristic_learner->learn(result.first, result.second);
          } else if (learn_one_method == kPipeLearnOneRandom) {
            const State* random_state = final_results[rand() % final_results.size()];
            heuristic_learner->set_timestamp(n+ 1);
            heuristic_learner->learn(random_state, result.second);
          } else if (learn_one_method == kPipeLearnOneWorst) {
            const State* worst_state = NULL;
            floatval_t worst_score = 1e20;
            for (const State* candidate_result: final_results) {
              if (worst_state == NULL || worst_score > candidate_result->score) {
                worst_state = candidate_result;
                worst_score = candidate_result->score;
              }
            }
            heuristic_learner->set_timestamp(n+ 1);
            heuristic_learner->learn(worst_state, result.second);
          }
        }
      }
    } else if (mode_ext == kPipePreparePhaseTwo) {
      std::vector<State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);
      int best_loss = instance.size(), dummy;
      for (const State* candidate_result: final_results) {
        Dependency output; build_output((*candidate_result), output);
        int loss1 = loss(output, instance, true, true, dummy);
        if (loss1 < best_loss) { best_loss = loss1; }
      }

      std::sort(final_results.begin(), final_results.end(),
          [](const State* x,  const State* y) -> bool { return x->score > y->score; });

      (*os) << "#forms\tpostags\t" << result.second->score;
      for (const State* candidate_result: final_results) {
        (*os) << "\t" << candidate_result->score;
      }
      (*os) << std::endl;
      for (size_t i = 0; i < instance.size(); ++ i) {
        (*os) << forms_alphabet.decode(instance.forms[i]) << " "
          << postags_alphabet.decode(instance.postags[i]) << " "
          << instance.heads[i] << "/"
          << (instance.heads[i] == -1? root: deprels_alphabet.decode(instance.deprels[i]));
        for (const State* candidate_result: final_results) {
          (*os) << " " << candidate_result->heads[i] << "/"
            << (candidate_result->heads[i] == -1? root:
                deprels_alphabet.decode(candidate_result->deprels[i]));
        }
        (*os) << std::endl;
      }
      (*os) << std::endl;
    } else if (mode_ext == kPipeEvaluate) {
      std::vector<State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      bool correct_in_beam = false;
      bool correct_in_beam_not_best = true;
      int oracle_positive_recalled_heads = 0;
      int oracle_positive_recalled_deprels = 0;
      int oracle_negative_recalled_heads = instance.size();
      int oracle_negative_recalled_deprels = instance.size();
      int nr_effective_tokens = 0;
      int loss1, loss2;
      floatval_t one_avg_uas = 0, one_avg_las = 0;
      for (const State* candidate_result: final_results) {
        Dependency output; build_output((*candidate_result), output);
        loss1 = loss(output, instance, true, true, nr_effective_tokens);
        loss2 = loss(output, instance, false, true, nr_effective_tokens);

        if (0 == loss1) {
          correct_in_beam = true;
          if (candidate_result == result.first) {
            // Once the correct is found in the beam, this flag should be
            // set false.
            correct_in_beam_not_best = false;
          }
        }

        int recalled_heads = nr_effective_tokens - loss2;
        int recalled_deprels = nr_effective_tokens - loss1;
        one_avg_las += floatval_t(recalled_deprels) / nr_effective_tokens;
        one_avg_uas += floatval_t(recalled_heads) / nr_effective_tokens;

        if (oracle_positive_recalled_heads < recalled_heads) {
          oracle_positive_recalled_heads = recalled_heads;
        }

        if (oracle_negative_recalled_heads > recalled_heads) {
          oracle_negative_recalled_heads = recalled_heads;
        }

        if (oracle_positive_recalled_deprels < recalled_deprels) {
          oracle_positive_recalled_deprels = recalled_deprels;
        }

        if (oracle_negative_recalled_deprels > recalled_deprels) {
          oracle_negative_recalled_deprels = recalled_deprels;
        }
      }

      nr_oracle_positive_recalled_heads += oracle_positive_recalled_heads;
      nr_oracle_negative_recalled_heads += oracle_negative_recalled_heads;
      nr_oracle_positive_recalled_deprels += oracle_positive_recalled_deprels;
      nr_oracle_negative_recalled_deprels += oracle_negative_recalled_deprels;
      nr_deprels += nr_effective_tokens;
      if (one_avg_las > 0 && one_avg_uas > 0) {
        avg_uas += one_avg_uas / final_results.size();
        avg_las += one_avg_las / final_results.size();
      } else {
        avg_uas += 1.;
        avg_las += 1.;
      }

      if (correct_in_beam) {
        ++ nr_correct_in_beam;
        if (correct_in_beam_not_best) { ++ nr_correct_in_beam_not_best; }
      }
    } else {
      std::vector<State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);

      const State* best_result = NULL;
      if (rerank) {
        std::sort(final_results.begin(), final_results.end(),
          [](const State* x,  const State* y) -> bool { return x->score > y->score; });

        floatval_t best_score = -1.;
        floatval_t highest_phase_one_score = final_results[0]->score;
        floatval_t lowest_phase_one_score = final_results.back()->score;
        floatval_t delta = highest_phase_one_score - lowest_phase_one_score;
        for (int i = 0; i < final_results.size(); ++ i) {
          State* candidate_result = final_results[i];
          for (int j = 0; j < candidate_result->ref->size(); ++ j) {
            if (candidate_result->heads[j] == -1) {
              candidate_result->deprels[j] = deprels_alphabet.encode(root.c_str());
            }
          }
          candidate_result->score = (candidate_result->score - lowest_phase_one_score)/ delta;
          candidate_result->top0 = (i+ 1);
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
    _INFO << "report: UAS upperbound: " << nr_oracle_positive_recalled_heads
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_positive_recalled_heads)/nr_deprels;
    _INFO << "report: LAS upperbound: " << nr_oracle_positive_recalled_deprels
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_positive_recalled_deprels)/nr_deprels;
    _INFO << "report: UAS lowerbound: " << nr_oracle_negative_recalled_heads
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_negative_recalled_heads)/nr_deprels;
    _INFO << "report: LAS lowerbound: " << nr_oracle_negative_recalled_deprels
      << "/" << nr_deprels << "=" << floatval_t(nr_oracle_negative_recalled_deprels)/nr_deprels;
    _INFO << "report: averaged UAS: " << avg_uas / N;
    _INFO << "report: averaged LAS: " << avg_las / N;
  }

  if (os != NULL && os != &std::cout) { delete os; }
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
    if (ignore_punctuation && is_punctuation(predict.postags[i])) {
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

bool Pipe::is_punctuation(const postag_t& postag) {
  return (PUNC_POS.find(postag) != PUNC_POS.end());
}

} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
