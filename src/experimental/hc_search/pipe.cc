#include "experimental/hc_search/pipe.h"
#include "experimental/hc_search/knowledge.h"
#include "utils/io/instance/csv.h"
#include "utils/misc.h"
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

using DependencyParser::DependencyParserUtils;

Pipe::Pipe(const LearnOption& opts)
  : weight(NULL), learner(NULL),
  fe::CommonPipeConfigure(static_cast<const fe::LearnOption&>(opts)) {

  if (opts.ranker == "gold")        { rank_strategy = kGold;   }
  else if (opts.ranker == "coarse") { rank_strategy = kCoarse; }
  else if (opts.ranker == "fine")   { rank_strategy = kFine;   }

  language = opts.language;
  if (opts.evaluation == "punc") {
    evaluate_strategy = DependencyParser::DependencyParserUtils::kIncludePunctuation;
  } else if (opts.evaluation == "conllx") {
    evaluate_strategy = DependencyParser::DependencyParserUtils::kCoNLLx;
  } else {
    if (opts.language == "en") {
      evaluate_strategy = DependencyParser::DependencyParserUtils::kChen14en;
    } else if (opts.language == "ch") {
      evaluate_strategy = DependencyParser::DependencyParserUtils::kChen14ch;
    }
  }

  _INFO << "report: (cstep.learn) ranking strategy = " << opts.ranker;
  _INFO << "report: (cstep.learn) language = " << opts.language;
  _INFO << "report: (cstep.learn) evaluation strategy = " << opts.evaluation;

  if (load_model(model_path)) {
    _INFO << "report: (cstep.learn) model " << model_path << " is loaded.";
  } else {
    _INFO << "report: (cstep.learn) model " << model_path << " is not loaded.";
  }
}


Pipe::Pipe(const TestOption& opts)
  : weight(NULL), learner(NULL),
  fe::CommonPipeConfigure(static_cast<const fe::TestOption&>(opts)){
  alpha = opts.alpha;
  _INFO << "report: (cstep.test) alpha = " << alpha;
  if (load_model(model_path)) {
    _INFO << "report: (cstep.test) model " << model_path << " is loaded.";
  } else {
    _INFO << "report: (cstep.test) model " << model_path << " is not loaded.";
  }
}


Pipe::~Pipe() {
}


void Pipe::test() {
  if (!load_data(input_path, false)) { return; }
  build_knowledge();

  for (auto& data: dataset) {
    Dependency& inst = data.instance;
    std::vector<RerankingTree>& trees = data.trees;
    std::vector<floatval_t> scores(trees.size(), 0.);

    for (auto i = 0; i < trees.size(); ++ i) {
      auto& t = trees[i];
      ScoreContext ctx(inst.size(), inst.forms, inst.postags, t.heads, t.deprels);
      t.c_score = weight->score(ctx, false);
      scores[i] = t.h_score* alpha + (1- alpha)* t.c_score;
    }

    floatval_t best_score = -1e20; int best_i = -1;
    for (auto i = 0; i < scores.size(); ++ i) {
      if (best_i < 0 || best_score < scores[i]) {
        best_i = i; best_score = scores[i];
      }
    }

    for (auto i = 0; i < inst.size(); ++ i) {
      std::cout << forms_alphabet.decode(inst.forms[i]) << "\t"
        << postags_alphabet.decode(inst.postags[i]) << "\t"
        << trees[best_i].heads[i] << "\t"
        << trees[best_i].deprels[i] << std::endl;
    }
    std::cout << std::endl;
  }
}


bool Pipe::check_instance(const std::vector<std::string>& info,
    const std::vector< std::vector< std::string> >& mat) const {
  bool bad_format = false;

  auto col = mat[0].size();
  if (col != info.size()) { _WARN << "Unmatched number of columns in header"; return false; }
  if (col <= 3) {  _WARN << "Number of columns less than 3!"; return false; }
  for (auto& row: mat) {
    if (row.size() != col) { _WARN << "Unmatched number of columns"; return false; }
  }
  if (info[0] != "#forms" || info[1] != "postags") {
    _WARN << "Wrong name of header"; return false;
  }
  return true;
}


void Pipe::column_to_reranking_tree(int i, const std::vector< std::string >& header,
    const std::vector< std::vector<std::string> >& mat,
    RerankingTree& tree) {
  auto N = mat.size();

  tree.resize(N);
  tree.h_score = boost::lexical_cast<floatval_t>(header[i]);
  for (auto row = 0; row < N; ++ row) {
    auto delimiter_position = mat[row][i].find_first_of('/');
    tree.heads[row] = atoi(mat[row][i].substr(0, delimiter_position).c_str());
    tree.deprels[row] = deprels_alphabet.insert(mat[row][i].substr(delimiter_position+ 1));
  }
}


bool Pipe::load_data(const std::string& filename, bool with_oracle) {
  namespace algo = boost::algorithm;

  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file.";
    _ERROR << "#: training halt.";
    return false;
  }
  _INFO << "report: loading dataset from reference file.";

  dataset.clear();

  std::string instance, buffer;
  // Loop over the instances
  size_t n = 0;
  while (std::getline(ifs, buffer)) {
    algo::trim(buffer);
    if (buffer.size() != 0) {
      instance += buffer + "\n";
      continue;
    }

    std::istringstream iss(instance);

    // Parse header
    std::string item_context, header;
    std::getline(iss, header); // the header

    std::vector<std::string> info;
    algo::trim(header);
    algo::split(info, header, boost::is_any_of("\t "), boost::token_compress_on);

    std::vector< std::vector<std::string> > mat;
    IO::read_csv(iss, mat);

    // Format checking ...
    if (!check_instance(info, mat)) { continue; }

    // Constructing reranking instance
    int N = mat.size(), M = mat[0].size();

    // The dependency part
    RerankingInstance ri; Dependency& inst = ri.instance;
    size_t delimiter_position;
    for (int row = 0; row < N; ++ row) {
      auto form = forms_alphabet.insert(mat[row][0]);
      auto postag = postags_alphabet.insert(mat[row][1]);

      inst.forms.push_back(form);
      inst.postags.push_back(postag);
    }

    // The tree part
    if (with_oracle) {
      column_to_reranking_tree(2, info, mat, ri.oracle);
      ri.oracle.c_score = 0;
      ri.oracle.loss = 0;
    }

    ri.trees.resize(M - 3);
    for (int col = 3; col < M; ++ col) {
      column_to_reranking_tree(col, info, mat, ri.trees[col- 3]);
      ri.trees[col- 3].c_score = 0;
      if (with_oracle) {
        ri.trees[col- 3].loss = wrong(ri.instance, ri.oracle.heads, ri.oracle.deprels,
            ri.trees[col- 3].heads, ri.trees[col- 3].deprels, true);
      } else {
        ri.trees[col- 3].loss = 0;
      }
    }

    dataset.push_back(ri);
    if ((n+ 1) % display_interval == 0) {
      _INFO << "pipe: loaded #" << (n+ 1) << " instances.";
    }
    ++ n;
    instance = "";
  }

  _INFO << "report: load #" << dataset.size() << " instances.";
  _INFO << "report: " << forms_alphabet.size() << " form(s) is found.";
  _INFO << "report: " << postags_alphabet.size() << " postag(s) is found.";
  _INFO << "report: " << deprels_alphabet.size() << " deprel(s) is found.";
  return true;
}


void Pipe::build_knowledge() {
  if (language == "en") {
    // punctuation
    PUNC_POS.insert(postags_alphabet.encode("``"));
    PUNC_POS.insert(postags_alphabet.encode(","));
    PUNC_POS.insert(postags_alphabet.encode(":"));
    PUNC_POS.insert(postags_alphabet.encode("."));
    PUNC_POS.insert(postags_alphabet.encode("''"));
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
    _INFO << "report(cstep): postag knowledge for English is built.";
  } else if (language == "ch") {
    // Punctuation
    PUNC_POS.insert(postags_alphabet.encode("PU"));
    // Conjunction.
    CONJ_POS.insert(postags_alphabet.encode("CC"));
    // Preposition
    ADP_POS.insert(postags_alphabet.encode("P"));
    // Verb
    VERB_POS.insert(postags_alphabet.encode("VV"));
    VERB_POS.insert(postags_alphabet.encode("VA"));
    VERB_POS.insert(postags_alphabet.encode("VC"));
    VERB_POS.insert(postags_alphabet.encode("VE"));
    _INFO << "report(cstep): postag knowledge for Chinese is built.";
  } else {
    _INFO << "pipe(cstep): unknown language, no knowledge is built.";
  }
}


int Pipe::wrong(const Dependency& instance,
    const std::vector<int>& predict_heads,
    const std::vector<int>& predict_deprels,
    const std::vector<int>& heads,
    const std::vector<int>& deprels,
    bool labeled) const {
  std::vector<std::string> forms;
  std::vector<std::string> postags;
  for (auto i = 0; i < instance.size(); ++ i) {
    forms.push_back( forms_alphabet.decode(instance.forms[i]) );
    postags.push_back( postags_alphabet.decode(instance.postags[i]) );
  }

  if (labeled) {
    std::tuple<int, int, int> eval;
    if (evaluate_strategy == DependencyParserUtils::kIncludePunctuation) { 
      eval = DependencyParserUtils::evaluate_inc_punc(forms, postags,
          predict_heads, predict_deprels, heads, deprels, false);
    } else if (evaluate_strategy == DependencyParserUtils::kCoNLLx) {
      eval = DependencyParserUtils::evaluate_conllx(forms, postags,
          predict_heads, predict_deprels, heads, deprels, false);
    } else if (evaluate_strategy == DependencyParserUtils::kChen14en) {
      eval = DependencyParserUtils::evaluate_chen14en(forms, postags,
          predict_heads, predict_deprels, heads, deprels, false);
    } else {
      eval = DependencyParserUtils::evaluate_chen14ch(forms, postags,
          predict_heads, predict_deprels, heads, deprels, false);
    }
    return std::get<0>(eval) - std::get<2>(eval);
  } else {
    std::tuple<int, int> eval;
    if (evaluate_strategy == DependencyParserUtils::kIncludePunctuation) { 
      eval = DependencyParserUtils::evaluate_inc_punc(
          forms, postags, predict_heads, heads, false);
    } else if (evaluate_strategy == DependencyParserUtils::kCoNLLx) {
      eval = DependencyParserUtils::evaluate_conllx(
          forms, postags, predict_heads, heads, false);
    } else if (evaluate_strategy == DependencyParserUtils::kChen14en) {
      eval = DependencyParserUtils::evaluate_chen14en(
          forms, postags, predict_heads, heads, false);
    } else {
      eval = DependencyParserUtils::evaluate_chen14ch(
          forms, postags, predict_heads, heads, false);
    }
    return std::get<0>(eval) - std::get<1>(eval);
  }
  return instance.size();
}


void Pipe::generate_training_samples() {
  for (auto& data: dataset) {
    std::vector<int> losses;
    auto M = data.trees.size();

    losses.resize(M);
    for (auto i = 0; i < M; ++ i) {
      // TODO: should use gold postags, not auto postags. (?)
      losses[i] = data.trees[i].loss;
    }

    if (rank_strategy == kGold) {
      Sample sample;
      sample.ref = &data.instance;
      sample.good.push_back(&data.oracle);
      for (auto i = 0; i < M; ++ i) {
        if (losses[i] != 0) { sample.bad.push_back(&data.trees[i]); }
      }
      samples.push_back(sample);
    } else {
      std::set<int> ranks(losses.begin(), losses.end());
      for (auto loss: ranks) {
        Sample sample;
        sample.ref = &data.instance;
        for (auto i = 0; i < M; ++ i) {
          if (losses[i] == loss) {
            sample.good.push_back(&data.trees[i]);
          } else if (losses[i] > loss) {
            sample.bad.push_back(&data.trees[i]);
          }
        }
        if (sample.good.size() == 0 || sample.bad.size() == 0) { continue; }

        samples.push_back(sample);
        if (rank_strategy == kCoarse) { break; }
      }
    }
  }
  _INFO << "report: (cstep.learn) generate #" << samples.size() << " training samples.";
}


void Pipe::learn() {
  if (!load_data(reference_path, true)) { return; }
  build_knowledge();

  generate_training_samples();

  size_t N = samples.size();
  std::vector<size_t> ranks;
  Utility::shuffle(N, shuffle_times, ranks);
  learner = new Learner(weight, this->algorithm);

  for (size_t n = 0; n < N; ++ n) {
    const Sample& sample = samples[ranks[n]];
    const Dependency* inst = sample.ref;

    RerankingTree* worst_good_tree = NULL;
    RerankingTree* best_bad_tree = NULL;
    floatval_t worst_good_score = 1e20;
    floatval_t best_bad_score = -1e20;

    for (int i = 0; i < sample.good.size(); ++ i) {
      RerankingTree* t = sample.good[i];
      ScoreContext ctx(inst->size(), inst->forms, inst->postags, t->heads, t->deprels);
      t->c_score = weight->score(ctx, false);
      if (worst_good_tree == NULL || t->c_score < worst_good_tree->c_score) {
        worst_good_tree = t;
      }
    }

    for (int i = 0; i < sample.bad.size(); ++ i) {
      RerankingTree* t = sample.bad[i];
      ScoreContext ctx(inst->size(), inst->forms, inst->postags, t->heads, t->deprels);
      t->c_score = weight->score(ctx, false);
      if (best_bad_tree == NULL || t->c_score > best_bad_tree->c_score) {
        best_bad_tree = t;
      }
    }

    if (best_bad_tree == NULL || worst_good_tree == NULL) {
      _WARN << "best bad or worst good was not found.";
      _INFO << "# good = " << sample.good.size() << ", # bad = " << sample.bad.size()
        << ", id = " << n;
      continue;
    }

    learn_one_pair(inst, worst_good_tree, best_bad_tree, n+ 1);
    if ((n+ 1) % display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  learner->set_timestamp(N);
  learner->flush();

  _INFO << "pipe: nr errors: " << learner->errors() << "/" << N;
  save_model(model_path);
}

void Pipe::learn_one_pair(const Dependency* inst,
    const RerankingTree* good, const RerankingTree* bad,
    int timestamp) {
  int delta_loss = bad->loss - good->loss;
  if (delta_loss > 0 && good->c_score - bad->c_score < delta_loss - 1e-8) {
    learner->set_timestamp(timestamp);
    ScoreContext bad_ctx(inst->size(), inst->forms, inst->postags, good->heads, good->deprels);
    ScoreContext good_ctx(inst->size(), inst->forms, inst->postags, bad->heads, bad->deprels);
    learner->learn(&bad_ctx, &good_ctx, delta_loss, &bad->c_score, &good->c_score);
  }
}

bool Pipe::load_model(const std::string& model_path) {
  weight = new Weight;
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "pipe: cstep model doesn't exist.";
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
  if (!weight->load(mfs)) {
    _WARN << "pipe: failed to load cstep weight.";
    return false;
  }
  mfs.close();
  return true;
}

void Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "pipe: failed to save C-step model.";
  } else {
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "pipe: C-step model saved to " << model_path;
  }
  mfs.close();
}

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
