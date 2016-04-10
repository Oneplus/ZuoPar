#include "experimental/hc_search/arceager/pipe.h"
#include "frontend/common_pipe_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

namespace ae = DependencyParser::ArcEager;
using DependencyParser::DependencyParserUtils;

Pipe::Pipe(const boost::program_options::variables_map& vm): ae::Pipe(vm) {
  if (vm.count("neg-sample")) {
    _INFO << "[RPT] (hstep.learn)";
    _INFO << "[RPT] (hstep.learn) learning method = " << vm["neg-sample"].as<std::string>();
  } else if (vm.count("language")) {
    _INFO << "[RPT] (hstep.prepare|evaluate)";
    _INFO << "[RPT] (hstep.prepare|evaluate) language = " << vm["language"].as<std::string>();
  }
}

Pipe::~Pipe() {
}

int Pipe::wrong(const CoNLLXDependency& instance, bool labeled,
    const std::vector<int>& heads,
    const std::vector<deprel_t>& deprels,
    int& n_effective_words) const {
  std::vector<std::string> forms;
  std::vector<std::string> postags;
  for (auto i = 0; i < instance.size(); ++ i) {
    forms.push_back( forms_alphabet.decode(instance.forms[i]) );
    if (instance.feats[i].size() > 0) {
      postags.push_back( feat_alphabet.decode(instance.feats[i][0]) );
    } else {
      postags.push_back( postags_alphabet.decode(instance.postags[i]) );
    }
  }

  if (labeled) {
    std::tuple<int, int, int> eval;
    if (evaluate_strategy == DependencyParserUtils::kChen14en) {
      eval = DependencyParserUtils::evaluate_chen14en(forms, postags,
          instance.heads, instance.deprels, heads, deprels, true);
    } else {
      eval = DependencyParserUtils::evaluate_chen14ch(forms, postags,
          instance.heads, instance.deprels, heads, deprels, true);
    }
    n_effective_words = std::get<0>(eval);
    return std::get<0>(eval) - std::get<2>(eval);
  } else {
    std::tuple<int, int> eval;
    if (evaluate_strategy == DependencyParserUtils::kChen14en) {
      eval = DependencyParserUtils::evaluate_chen14en(
          forms, postags, instance.heads, heads, true);
    } else {
      eval = DependencyParserUtils::evaluate_chen14ch(
          forms, postags, instance.heads, heads, true);
    }
    n_effective_words = std::get<0>(eval);
    return std::get<0>(eval) - std::get<1>(eval);
  }
  n_effective_words = instance.size();
  return instance.size();
}

void Pipe::learn() {
  if (!setup(conf["train"].as<std::string>(), dataset, true)) {
    _ERROR << "[PIP] failed to load training data.";
    return; 
  }
  if (conf.count("devel") && setup(conf["devel"].as<std::string>(), devel_dataset, false)) {
    _WARN << "[PIP] failed to load development data.";
  }
  deprel_t root_tag = deprels_alphabet.encode(root);
  decoder = new ae::Decoder(deprels_alphabet.size(), root_tag, ae::Decoder::kLeft,
    conf["beam"].as<unsigned>(), false, 
    get_update_strategy(conf["update"].as<std::string>()), weight);
  learner = new ae::Learner(weight, get_algorithm(conf["algorithm"].as<std::string>()));

  std::string model_path = FrontEnd::get_model_name(signature, conf);
  unsigned n_seen = 0, N = dataset.size();
  double best_score = 0.;
  for (unsigned iter = 0; iter < conf["maxiter"].as<unsigned>(); ++iter) {
    _INFO << "[PIP] iteration #" << iter + 1 << ", start training.";
    std::random_shuffle(dataset.begin(), dataset.end());
    for (const CoNLLXDependency& instance : dataset) {
      ++n_seen;

      std::vector<ae::Action> actions;
      ae::ActionUtils::get_oracle_actions(instance, actions);
      int max_actions = instance.size() * 2 - 1;
      ae::State init_state(&instance);
      ae::Decoder::const_decode_result_t result = decoder->decode(init_state, actions, 
        max_actions);

      if (conf["neg-sample"].as<std::string>() == "baseline") {
        learner->set_timestamp(n_seen);
        learner->learn(result.first, result.second);
      } else {
        int round = decoder->get_ending_round();
        std::vector<ae::State*> final_results;
        decoder->get_results_in_beam(final_results, round);

        bool oracle_in_beam = false;
        for (const ae::State* candidate_result : final_results) {
          if (candidate_result == result.second) { oracle_in_beam = true; break; }
        }

        if (!oracle_in_beam) {
          learner->set_timestamp(n_seen);
          if (conf["neg-sample"].as<std::string>() == "best") {
            learner->learn(result.first, result.second);
          } else if (conf["neg-sample"].as<std::string>() == "random") {
            const ae::State* random_state = final_results[rand() % final_results.size()];
            learner->learn(random_state, result.second);
          } else if (conf["neg-sample"].as<std::string>() == "worst") {
            const ae::State* worst_state = NULL;
            floatval_t worst_score = 1e20;
            for (const ae::State* candidate_result : final_results) {
              if (worst_state == NULL || worst_score > candidate_result->score) {
                worst_state = candidate_result;
                worst_score = candidate_result->score;
              }
            }
            learner->learn(worst_state, result.second);
          }
        }
      }

      if (n_seen % conf["report_stops"].as<unsigned>() == 0) {
        _INFO << "[PIP] processed #" << n_seen % N << "/" << n_seen / N << " instance(s).";
      }
      if (n_seen % conf["evaluate_stops"].as<unsigned>() == 0) {
        learner->flush();
        EvaluationStatistics stat;
        evaluate(devel_dataset, stat);
        decoder->reset_use_avg();
        _INFO << "[PIP] evaluate score: " << stat.avg_uas;
        if (stat.n_correct_in_beam > best_score) {
          _INFO << "[PIP] NEW best model is achieved, save to" << model_path;
          save_model(model_path);
          best_score = stat.n_correct_in_beam;
        }
      }
    }
  }
}

void Pipe::prepare() {
  if (!setup(conf["input"].as<std::string>(), dataset, false)) {
    _ERROR << "[PIP] failed to open file.";
    return;
  }

  deprel_t root_tag = deprels_alphabet.encode(root);
  decoder = new ae::Decoder(deprels_alphabet.size(), root_tag, ae::Decoder::kLeft,
    conf["beam"].as<unsigned>(), true, UpdateStrategy::kNaive, weight);

  std::string output = FrontEnd::get_output_name(signature, conf);
  std::ostream* os = IO::get_ostream(output);

  for (const CoNLLXDependency& instance : dataset) {
    int max_actions = instance.size() * 2 - 1;
    ae::State init_state(&instance);
    ae::Decoder::const_decode_result_t result = decoder->decode(init_state,
      std::vector<ae::Action>(), max_actions);

    std::vector<ae::State*> final_results;
    decoder->get_results_in_beam(final_results, max_actions);
    std::sort(final_results.begin(), final_results.end(),
      [](const ae::State* x, const ae::State* y) -> bool { return x->score > y->score; });

    (*os) << "#id\tforms\tpostags\t" << result.second->score;
    for (const ae::State* candidate_result : final_results) {
      (*os) << "\t" << candidate_result->score;
    }
    (*os) << std::endl;
    for (size_t i = 1; i < instance.size(); ++i) {
      (*os) << i << " "
        << forms_alphabet.decode(instance.forms[i]) << " "
        << postags_alphabet.decode(instance.postags[i]) << " "
        << instance.heads[i] << "/"
        << (instance.heads[i] == -1 ? root : deprels_alphabet.decode(instance.deprels[i]));
      for (const ae::State* candidate_result : final_results) {
        (*os) << " " << candidate_result->heads[i] << "/"
          << (candidate_result->heads[i] == -1 ? root :
          deprels_alphabet.decode(candidate_result->deprels[i]));
      }
      (*os) << std::endl;
    }
    (*os) << std::endl;
  }

  if (os != NULL && os != &std::cout) { delete os; }
}

void Pipe::evaluate(const std::vector<CoNLLXDependency>& ds, EvaluationStatistics& stat) {
  for (const CoNLLXDependency& instance : ds) {
    int max_actions = instance.size() * 2 - 1;
    ae::State init_state(&instance);
    ae::Decoder::const_decode_result_t result = decoder->decode(init_state,
      std::vector<ae::Action>(), max_actions);

    std::vector<ae::State*> final_results;
    decoder->get_results_in_beam(final_results, max_actions);

    bool correct_in_beam = false;
    bool correct_in_beam_not_best = true;
    int oracle_positive_recalled_heads = 0;
    int oracle_positive_recalled_deprels = 0;
    int oracle_negative_recalled_heads = instance.size();
    int oracle_negative_recalled_deprels = instance.size();
    int n_effective_tokens = 0;
    int loss1, loss2;
    floatval_t one_avg_uas = 0, one_avg_las = 0;

    CoNLLXDependency output;
    build_output((*result.first), output, deprels_alphabet.encode(root));
    loss1 = wrong(output, true, instance.heads, instance.deprels, n_effective_tokens);
    loss2 = wrong(output, false, instance.heads, instance.deprels, n_effective_tokens);

    stat.n_best_recalled_heads += n_effective_tokens - loss2;
    stat.n_best_recalled_deprels += n_effective_tokens - loss1;

    for (const ae::State* candidate_result : final_results) {
      CoNLLXDependency output;
      build_output((*candidate_result), output, deprels_alphabet.encode(root));
      loss1 = wrong(output, true, instance.heads, instance.deprels, n_effective_tokens);
      loss2 = wrong(output, false, instance.heads, instance.deprels, n_effective_tokens);

      if (0 == loss1) {
        correct_in_beam = true;
        if (candidate_result == result.first) {
          // Once the correct is found in the beam, this flag should be
          // set false.
          correct_in_beam_not_best = false;
        }
      }

      int recalled_heads = n_effective_tokens - loss2;
      int recalled_deprels = n_effective_tokens - loss1;
      one_avg_las += floatval_t(recalled_deprels) / n_effective_tokens;
      one_avg_uas += floatval_t(recalled_heads) / n_effective_tokens;

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

    stat.n_oracle_positive_recalled_heads += oracle_positive_recalled_heads;
    stat.n_oracle_negative_recalled_heads += oracle_negative_recalled_heads;
    stat.n_oracle_positive_recalled_deprels += oracle_positive_recalled_deprels;
    stat.n_oracle_negative_recalled_deprels += oracle_negative_recalled_deprels;
    stat.n_deprels += n_effective_tokens;
    if (one_avg_las > 0 && one_avg_uas > 0) {
      stat.avg_uas += one_avg_uas / final_results.size();
      stat.avg_las += one_avg_las / final_results.size();
    } else {
      stat.avg_uas += 1.;
      stat.avg_las += 1.;
    }

    if (correct_in_beam) {
      ++stat.n_correct_in_beam;
      if (correct_in_beam_not_best) { ++stat.n_correct_in_beam_not_best; }
    }
  }
}

void Pipe::evaluate() {
  if (!setup(conf["input"].as<std::string>(), dataset, false)) {
    _ERROR << "[PIP] failed to open file.";
    return;
  }
  deprel_t root_tag = deprels_alphabet.encode(root);
  decoder = new ae::Decoder(deprels_alphabet.size(), root_tag, ae::Decoder::kLeft,
    conf["beam"].as<unsigned>(), true, UpdateStrategy::kNaive, weight);

  unsigned N = dataset.size();
  EvaluationStatistics stat;
  evaluate(dataset, stat);

  _INFO << "[RPT] correct in beam: " << stat.n_correct_in_beam
    << "/" << N << "=" << floatval_t(stat.n_correct_in_beam) / N;
  _INFO << "[RPT] correct in beam (not best): " << stat.n_correct_in_beam_not_best
    << "/" << stat.n_correct_in_beam
    << "=" << floatval_t(stat.n_correct_in_beam_not_best) / stat.n_correct_in_beam;
  _INFO << "[RPT] UAS upperbound: " << stat.n_oracle_positive_recalled_heads
    << "/" << stat.n_deprels << "=" << floatval_t(stat.n_oracle_positive_recalled_heads) / stat.n_deprels;
  _INFO << "[RPT] LAS upperbound: " << stat.n_oracle_positive_recalled_deprels
    << "/" << stat.n_deprels << "=" << floatval_t(stat.n_oracle_positive_recalled_deprels) / stat.n_deprels;
  _INFO << "[RPT] UAS lowerbound: " << stat.n_oracle_negative_recalled_heads
    << "/" << stat.n_deprels << "=" << floatval_t(stat.n_oracle_negative_recalled_heads) / stat.n_deprels;
  _INFO << "[RPT] LAS lowerbound: " << stat.n_oracle_negative_recalled_deprels
    << "/" << stat.n_deprels << "=" << floatval_t(stat.n_oracle_negative_recalled_deprels) / stat.n_deprels;
  _INFO << "[RPT] averaged UAS: " << stat.avg_uas / N;
  _INFO << "[RPT] averaged LAS: " << stat.avg_las / N;
  _INFO << "[RPT] best UAS: " << floatval_t(stat.n_best_recalled_heads) / stat.n_deprels;
  _INFO << "[RPT] best LAS: " << floatval_t(stat.n_best_recalled_deprels) / stat.n_deprels;
}

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
