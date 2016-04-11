#include "experimental/hc_search/pipe.h"
#include "experimental/hc_search/knowledge.h"
#include "utils/io/stream.h"
#include "utils/io/instance/csv.h"
#include "utils/misc.h"
#include "frontend/common_pipe_utils.h"
#include <set>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace CStep {

using DependencyParser::DependencyParserUtils;

Pipe::Pipe(const boost::program_options::variables_map& vm)
  : weight(new Weight), learner(NULL), conf(vm) {
  language = vm["language"].as<std::string>();

  if (conf.count("model") && load_model(conf["model"].as<std::string>())) {
    _INFO << "[RPT] (cstep.learn) model is loaded.";
  } else {
    _INFO << "[RPT] (cstep.learn) model is not loaded.";
  }
}

Pipe::~Pipe() {
}


void Pipe::evaluate(std::vector<RerankingInstance>& ds, double set_alpha, std::map<double, double>& result) {
  std::string output = FrontEnd::get_output_name("hc_search_cstep", conf);

  std::vector<floatval_t> max_h_scores(ds.size());
  std::vector<floatval_t> min_h_scores(ds.size());
  std::vector<floatval_t> max_c_scores(ds.size());
  std::vector<floatval_t> min_c_scores(ds.size());

  for (unsigned i = 0; i < ds.size(); ++i) {
    RerankingInstance& data = ds[i];
    CoNLLXDependency& inst = data.instance;
    std::vector<RerankingTree>& trees = data.trees;

    floatval_t max_h_score = 0, max_c_score = 0;
    floatval_t min_h_score = 0, min_c_score = 0;

    for (auto j = 0; j < trees.size(); ++j) {
      auto& t = trees[j];
      ScoreContext ctx(inst.size(), inst.forms, inst.postags, t.heads, t.deprels);
      t.c_score = weight->score(ctx, false);
      if (j == 0 || max_h_score < t.h_score) { max_h_score = t.h_score; }
      if (j == 0 || min_h_score > t.h_score) { min_h_score = t.h_score; }
      if (j == 0 || max_c_score < t.c_score) { max_c_score = t.c_score; }
      if (j == 0 || min_c_score > t.c_score) { min_c_score = t.c_score; }
    }
    max_h_scores[i] = max_h_score;
    min_h_scores[i] = min_h_score;
    max_c_scores[i] = max_c_score;
    min_c_scores[i] = min_c_score;
  }
  
  double a_start = 0., a_end = 0.;
  if (set_alpha < 0.) { a_start = 0.; a_end = 1.; }
  else { a_start = set_alpha; a_end = set_alpha; }

  for (double a = a_start; a <= a_end; a += 0.005) {
    floatval_t alpha = a;
    std::ostream* os = IO::get_ostream(output);

    std::vector<int> best_ids(ds.size());
    for (unsigned i = 0; i < ds.size(); ++i) {
      RerankingInstance& data = ds[i];
      std::vector<RerankingTree>& trees = data.trees;
      std::vector<floatval_t> scores(trees.size(), 0.);

      for (auto j = 0; j < trees.size(); ++j) {
        auto& t = trees[j];
        scores[j] =
          alpha * ((t.h_score - min_h_scores[i]) / (max_h_scores[i] - min_h_scores[i])) +
          (1 - alpha) * ((t.c_score - min_c_scores[i]) / (max_c_scores[i] - min_c_scores[i]));
      }

      floatval_t best_score = -1e20; int best_j = -1;
      for (auto j = 0; j < scores.size(); ++j) {
        if (best_j < 0 || best_score < scores[j]) {
          best_j = j; best_score = scores[j];
        }
      }
      best_ids[i] = best_j;
    }

    for (unsigned i = 0; i < ds.size(); ++i) {
      RerankingInstance& data = ds[i];
      CoNLLXDependency& inst = data.instance;
      std::vector<RerankingTree>& trees = data.trees;

      for (auto j = 1; j < inst.size(); ++j) {
        (*os) << j << " "
          << forms_alphabet.decode(inst.forms[j]) << "\t"
          << forms_alphabet.decode(inst.forms[j]) << "\t"
          << postags_alphabet.decode(inst.postags[j]) << "\t"
          << postags_alphabet.decode(inst.postags[j]) << "\t"
          << "_\t"
          << trees[best_ids[i]].heads[j] << "\t"
          << deprels_alphabet.decode(trees[best_ids[i]].deprels[j]) << std::endl;
      }
      (*os) << std::endl;
    }
    if (os != NULL && (os != &(std::cout))) {
      delete os;
      double score = Utility::execute_script(conf["script"].as<std::string>(), output);
      result[alpha] = score;
    }
  }
}

void Pipe::test() {
  if (!load_data(conf["input"].as<std::string>(), dataset, false)) {
    _ERROR << "[PIP] failed to load input data.";
    return;
  }

  build_knowledge();
  std::vector<std::ostream*> ostreams;
  
  std::map<double, double> result;
  if (conf.count("alpha")) {
    evaluate(dataset, conf["alpha"].as<double>(), result);
  } else {
    evaluate(dataset, -1., result);
  }

  for (auto& entry : result) {
    _INFO << "[PIP] test evaluation: " << entry.first << " = " << entry.second;
  }
}


bool Pipe::check_instance(const std::vector<std::string>& info,
    const std::vector< std::vector< std::string> >& mat) const {
  bool bad_format = false;

  auto col = mat[0].size();
  if (col != info.size()) { _WARN << "Unmatched number of columns in header"; return false; }
  if (col <= 4) {  _WARN << "Number of columns less than 4!"; return false; }
  for (auto& row: mat) {
    if (row.size() != col) { _WARN << "Unmatched number of columns"; return false; }
  }
  if (info[0] != "#id" || info[1] != "forms" || info[2] != "postags") {
    _WARN << "Wrong name of header"; return false;
  }
  return true;
}


void Pipe::column_to_reranking_tree(int i, const std::vector< std::string >& header,
    const std::vector< std::vector<std::string> >& mat,
    RerankingTree& tree) {
  auto N = mat.size();

  tree.resize(N+1); // with dummy root
  tree.h_score = boost::lexical_cast<floatval_t>(header[i]);
  tree.heads[0] = -1; tree.deprels[0] = 0;
  for (auto row = 0, j = 1; row < N; ++ row, ++ j) {
    auto delimiter_position = mat[row][i].find_first_of('/');
    tree.heads[j] = atoi(mat[row][i].substr(0, delimiter_position).c_str());
    tree.deprels[j] = deprels_alphabet.insert(mat[row][i].substr(delimiter_position+ 1));
  }
}


bool Pipe::load_data(const std::string& filename, std::vector<RerankingInstance>& ds,
  bool with_oracle) {
  namespace algo = boost::algorithm;

  std::ifstream ifs(filename.c_str());
  if (!ifs.good()) {
    _ERROR << "#: failed to open reference file.";
    _ERROR << "#: training halt.";
    return false;
  }
  _INFO << "[RPT] loading dataset from reference file.";

  ds.clear();
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
    RerankingInstance ri; CoNLLXDependency& inst = ri.instance;
    size_t delimiter_position;
    for (int row = 0; row < N; ++ row) {
      auto form = forms_alphabet.insert(mat[row][1]);
      auto postag = postags_alphabet.insert(mat[row][2]);

      inst.forms.push_back(form);
      inst.postags.push_back(postag);
    }

    // The tree part
    if (with_oracle) {
      column_to_reranking_tree(3, info, mat, ri.oracle);
      ri.oracle.c_score = 0;
      ri.oracle.loss = 0;
    }

    ri.trees.resize(M - 4);
    for (int col = 4, i = 0; col < M; ++ col, ++ i) {
      column_to_reranking_tree(col, info, mat, ri.trees[i]);
      ri.trees[i].c_score = 0;
      if (with_oracle) {
        ri.trees[i].loss = wrong(ri.instance, ri.oracle.heads, ri.oracle.deprels,
            ri.trees[i].heads, ri.trees[i].deprels, true);
      } else {
        ri.trees[i].loss = 0;
      }
    }

    ds.push_back(ri);
    ++ n;
    instance = "";
  }

  _INFO << "[RPT] load #" << ds.size() << " instances.";
  _INFO << "[RPT] " << forms_alphabet.size() << " form(s) is found.";
  _INFO << "[RPT] " << postags_alphabet.size() << " postag(s) is found.";
  _INFO << "[RPT] " << deprels_alphabet.size() << " deprel(s) is found.";
  return true;
}


void Pipe::build_knowledge() {
  std::string language = "";
  if (conf.count("language")) {
    language = conf["language"].as<std::string>();
  }

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
    _INFO << "[RPT] (cstep): postag knowledge for English is built.";
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
    _INFO << "[RPT] (cstep): postag knowledge for Chinese is built.";
  } else {
    _INFO << "[PIP] (cstep): unknown language, no knowledge is built.";
  }
}


int Pipe::wrong(const CoNLLXDependency& instance,
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
    if (conf["evaluation"].as<std::string>() == "punc") {
      eval = DependencyParserUtils::evaluate_inc_punc(forms, postags,
        predict_heads, predict_deprels, heads, deprels, true);
    } else if (conf["evaluation"].as<std::string>() == "conllx") {
      eval = DependencyParserUtils::evaluate_conllx(forms, postags, 
        predict_heads, predict_deprels, heads, deprels, true);
    } else {
      if (conf["language"].as<std::string>() == "ch") {
        eval = DependencyParserUtils::evaluate_chen14ch(forms, postags,
          predict_heads, predict_deprels, heads, deprels, true);
      } else {
        eval = DependencyParserUtils::evaluate_chen14en(forms, postags,
          predict_heads, predict_deprels, heads, deprels, true);
      }
    }
    return std::get<0>(eval) - std::get<2>(eval);
  } else {
    std::tuple<int, int> eval;
    if (conf["evaluation"].as<std::string>() == "punc") { 
      eval = DependencyParserUtils::evaluate_inc_punc(
          forms, postags, predict_heads, heads, true);
    } else if (conf["evaluation"].as<std::string>() == "conllx") {
      eval = DependencyParserUtils::evaluate_conllx(
          forms, postags, predict_heads, heads, true);
    } else {
      if (conf["language"].as<std::string>() == "ch") {
        eval = DependencyParserUtils::evaluate_chen14ch(
          forms, postags, predict_heads, heads, true);
      } else {
        eval = DependencyParserUtils::evaluate_chen14en(
          forms, postags, predict_heads, heads, true);
      }
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

    if (conf["ranker"].as<std::string>() == "gold") {
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
        if (conf["ranker"].as<std::string>() == "coarse") { break; }
      }
    }
  }
  _INFO << "[RPT] (cstep.learn) generate #" << samples.size() << " training samples.";
}


void Pipe::learn() {
  if (!load_data(conf["train"].as<std::string>(), dataset, true)) {
    _ERROR << "[PIP] failed to load training data.";
    return;
  }
  if (!conf.count("devel") || !load_data(conf["devel"].as<std::string>(), devel_dataset, false)) {
    _WARN << "[PIP] development data is not loaded.";
  }

  build_knowledge();
  generate_training_samples();

  learner = new Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

  unsigned n_seen = 0, N = samples.size();
  double best_score = 0., best_mixed_score = 0., best_mixed_alpha;

  std::string model_path = FrontEnd::get_model_name("hc_search_cstep", conf);
  std::string model_path_mixed = model_path + ".mixed";

  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    _INFO << "[PIP] iteration #" << iter << ", start training.";
    std::random_shuffle(samples.begin(), samples.end());

    for (const Sample& sample : samples) {
      ++n_seen;
      const CoNLLXDependency* inst = sample.ref;

      RerankingTree* worst_good_tree = NULL;
      RerankingTree* best_bad_tree = NULL;

      for (int i = 0; i < sample.good.size(); ++i) {
        RerankingTree* t = sample.good[i];
        ScoreContext ctx(inst->size(), inst->forms, inst->postags, t->heads, t->deprels);
        t->c_score = weight->score(ctx, false);
        if (worst_good_tree == NULL || t->c_score < worst_good_tree->c_score) {
          worst_good_tree = t;
        }
      }

      for (int i = 0; i < sample.bad.size(); ++i) {
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
          << ", id = " << n_seen;
        continue;
      }

      learn_one_pair(inst, worst_good_tree, best_bad_tree, n_seen);
      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "[PIP] processed #" << n_seen % N << "/" << n_seen / N << " sample(s).";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        std::map<double, double> result;
        evaluate(devel_dataset, -1, result);

        if (result[0.] > best_score) {
          _INFO << "[PIP] NEW best model (wo/ mix) is achieved, save model to " << model_path;
          save_model(model_path);
          best_score = result[0.];
        }

        auto best = std::max_element(result.begin(), result.end(),
          [](std::pair<double, double> a, std::pair<double, double> b) {
          return a.second > b.second;  }
        );
        if (best->second > best_mixed_score) {
          _INFO << "[PIP] NEW best model (w/ mix) is achieved, save model to " << model_path_mixed;
          save_model(model_path_mixed);
          best_mixed_score = best->second;
          best_mixed_alpha = best->first;
        }
        _INFO << "[PIP] wo/ mix score: " << result[0.];
        _INFO << "[PIP] w/ mix score: " << best->second << " alpha = " << best->first;
      }
    }
    _INFO << "[PIP] processed #" << N << " instances.";
    learner->flush();
    _INFO << "[PIP] n errors: " << learner->errors() << "/" << N;

    std::map<double, double> result;
    evaluate(devel_dataset, -1, result);

    if (result[0.] > best_score) {
      _INFO << "[PIP] NEW best model (wo/ mix) is achieved, save model to " << model_path;
      save_model(model_path);
      best_score = result[0.];
    }

    auto best = std::max_element(result.begin(), result.end(),
      [](std::pair<double, double> a, std::pair<double, double> b) {
      return a.second > b.second;  }
    );
    if (best->second > best_mixed_score) {
      _INFO << "[PIP] NEW best model (w/ mix) is achieved, save model to " << model_path_mixed;
      save_model(model_path_mixed);
      best_mixed_score = best->second;
      best_mixed_alpha = best->first;
    }
    _INFO << "[PIP] wo/ mix score: " << result[0.];
    _INFO << "[PIP] w/ mix score: " << best->second << " alpha = " << best->first;
  }

  _INFO << "[PIP] finish training";
  _INFO << "[PIP] best score: " << best_score;
  _INFO << "[PIP] best mixed score: " << best_mixed_score;
  _INFO << "[PIP] best mixed alpha: " << best_mixed_alpha;
}

void Pipe::learn_one_pair(const CoNLLXDependency* inst,
    const RerankingTree* good, const RerankingTree* bad,
    int timestamp) {
  int delta_loss = bad->loss - good->loss;
  _TRACE << "good loss=" << good->loss << ", bad loss=" << bad->loss
    << ", delta = " << delta_loss << ", good score=" << good->c_score
    << ", bad score=" << bad->c_score;
  if (delta_loss > 0 && good->c_score - bad->c_score < delta_loss - 1e-8) {
    learner->set_timestamp(timestamp);
    ScoreContext good_ctx(inst->size(), inst->forms, inst->postags, good->heads, good->deprels);
    ScoreContext bad_ctx(inst->size(), inst->forms, inst->postags, bad->heads, bad->deprels);
    learner->learn(&bad_ctx, &good_ctx, delta_loss, &bad->c_score, &good->c_score);
  }
}

bool Pipe::load_model(const std::string& model_path) {
  std::ifstream mfs(model_path);

  if (!mfs.good()) {
    _WARN << "[PIP] cstep model doesn't exist.";
    return false;
  }

  boost::archive::text_iarchive ia(mfs);
  ia >> language;

  if (!forms_alphabet.load(mfs)) {
    _WARN << "[PIP] failed to load forms alphabet.";
    return false;
  }
  if (!postags_alphabet.load(mfs)) {
    _WARN << "[PIP] failed to load postags alphabet.";
    return false;
  }
  if (!deprels_alphabet.load(mfs)) {
    _WARN << "[PIP] failed to load deprels alphabet.";
    return false;
  }
  if (!weight->load(mfs)) {
    _WARN << "[PIP] failed to load cstep weight.";
    return false;
  }
  mfs.close();
  return true;
}

void Pipe::save_model(const std::string& model_path) {
  std::ofstream mfs(model_path);
  if (!mfs.good()) {
    _WARN << "[PIP] failed to save C-step model.";
  } else {
    boost::archive::text_oarchive oa(mfs);
    oa << language;
    forms_alphabet.save(mfs);
    postags_alphabet.save(mfs);
    deprels_alphabet.save(mfs);
    weight->save(mfs);
    _INFO << "[PIP] C-step model saved to " << model_path;
  }
  mfs.close();
}

} //  namespace cstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
