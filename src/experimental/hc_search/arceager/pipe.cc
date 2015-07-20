#include "experimental/hc_search/arceager/pipe.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

namespace ae = DependencyParser::ArcEager;
using DependencyParser::DependencyParserUtils;

Pipe::Pipe(const LearnOption& opts): mode(kPipeLearn),
  ae::Pipe(static_cast<const DependencyParser::LearnOption&>(opts)) {

  if (opts.neg_sample == "best") {
    neg_sample_strategy = kNegativeSamplingBest;
  } else if (opts.neg_sample == "worst") {
    neg_sample_strategy = kNegativeSamplingWorst;
  } else if (opts.neg_sample == "random") {
    neg_sample_strategy = kNegativeSamplingRandom;
  } else if (opts.neg_sample == "baseline") {
    neg_sample_strategy = kNegativeSamplingBaseline;
  }
  _INFO << "report: (hstep.learn) learning method = " << opts.neg_sample;
}

Pipe::Pipe(const PrepareOption& opts): mode(kPipePrepare),
  ae::Pipe(static_cast<const DependencyParser::TestOption&>(opts)){

  language = opts.language;
  _INFO << "report: (hstep.prepare) language = " << language;

  if (load_model(opts.model_path)) {
    _INFO << "report: (hstep.prepare) model is loaded.";
  } else {
    _INFO << "report: (hstep.prepare) model is not loaded.";
  }
}

Pipe::Pipe(const EvaluateOption& opts): mode(kPipeEvaluate),
  ae::Pipe(static_cast<const DependencyParser::TestOption&>(opts)) {

  language = opts.language;
  _INFO << "report: (hstep.evaluate) language = " << language;

  if (language == "en") { evaluate_strategy = DependencyParserUtils::kChen14en; }
  else if (language == "ch") { evaluate_strategy = DependencyParserUtils::kChen14ch; }

  if (load_model(opts.model_path)) {
    _INFO << "report: (hstep.evaluate) model is loaded.";
  } else {
    _INFO << "report: (hstep.evaluate) model is not loaded.";
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

void Pipe::run2() {
  if (mode == kPipeLearn) {
    if (!setup(reference_path, true)) { return; }
  } else if (mode == kPipePrepare || mode == kPipeEvaluate) {
    if (!setup(input_path, false)) { return; }
  }

  deprel_t root_tag = deprels_alphabet.encode(root);
  if (mode == kPipeLearn) {
    decoder = new ae::Decoder(deprels_alphabet.size(), root_tag, ae::Decoder::kLeft,
        beam_size, false, update_strategy, weight);
    learner = new ae::Learner(weight, this->algorithm);
  } else {
    decoder = new ae::Decoder(deprels_alphabet.size(), root_tag, ae::Decoder::kLeft,
        beam_size, true, UpdateStrategy::kNaive, weight);
  }

  size_t N = dataset.size();
  std::ostream* os = (mode != kPipeLearn? IO::get_ostream(output_path.c_str()): NULL);

  std::vector<std::size_t> ranks;
  Utility::shuffle(N, shuffle_times, ranks);

  int nr_correct_in_beam = 0;
  int nr_correct_in_beam_not_best = 0;
  int nr_oracle_positive_recalled_heads = 0;
  int nr_oracle_negative_recalled_heads = 0;
  int nr_oracle_positive_recalled_deprels = 0;
  int nr_oracle_negative_recalled_deprels = 0;
  int nr_best_recalled_heads = 0;
  int nr_best_recalled_deprels = 0;
  int nr_deprels = 0;
  floatval_t avg_uas = 0.;
  floatval_t avg_las = 0.;

  for (size_t n = 0; n < N; ++ n) {
    const CoNLLXDependency& instance = dataset[ranks[n]];

    std::vector<ae::Action> actions;
    ae::ActionUtils::get_oracle_actions(instance, actions);

    int max_nr_actions = instance.size() * 2 - 1;
    ae::State init_state(&instance);
    ae::Decoder::const_decode_result_t result = decoder->decode(init_state, actions,
        max_nr_actions);

    if (mode == kPipeLearn) {
      if (neg_sample_strategy == kNegativeSamplingBaseline) {
        learner->set_timestamp(n+ 1);
        learner->learn(result.first, result.second);
      } else {
        int round = decoder->get_ending_round();
        std::vector<ae::State*> final_results;
        decoder->get_results_in_beam(final_results, round);

        bool oracle_in_beam = false;
        for (const ae::State* candidate_result: final_results) {
          if (candidate_result == result.second) { oracle_in_beam = true; break; }
        }

        if (!oracle_in_beam) {
          learner->set_timestamp(n+ 1);

          if (neg_sample_strategy == kNegativeSamplingBest) {
            learner->learn(result.first, result.second);

          } else if (neg_sample_strategy == kNegativeSamplingRandom) {
            const ae::State* random_state = final_results[rand() % final_results.size()];
            learner->learn(random_state, result.second);

          } else if (neg_sample_strategy == kNegativeSamplingWorst) {
            const ae::State* worst_state = NULL;
            floatval_t worst_score = 1e20;
            for (const ae::State* candidate_result: final_results) {
              if (worst_state == NULL || worst_score > candidate_result->score) {
                worst_state = candidate_result;
                worst_score = candidate_result->score;
              }
            }
            learner->learn(worst_state, result.second);
          }
        }
      }
    } else if (mode == kPipePrepare) {
      std::vector<ae::State*> final_results;
      decoder->get_results_in_beam(final_results, max_nr_actions);
      std::sort(final_results.begin(), final_results.end(),
          [](const ae::State* x,  const ae::State* y) -> bool { return x->score > y->score; });

      (*os) << "#id\tforms\tpostags\t" << result.second->score;
      for (const ae::State* candidate_result: final_results) {
        (*os) << "\t" << candidate_result->score;
      }
      (*os) << std::endl;
      for (size_t i = 1; i < instance.size(); ++ i) {
        (*os) << i << " "
          << forms_alphabet.decode(instance.forms[i]) << " "
          << postags_alphabet.decode(instance.postags[i]) << " "
          << instance.heads[i] << "/"
          << (instance.heads[i] == -1? root: deprels_alphabet.decode(instance.deprels[i]));
        for (const ae::State* candidate_result: final_results) {
          (*os) << " " << candidate_result->heads[i] << "/"
            << (candidate_result->heads[i] == -1? root:
                deprels_alphabet.decode(candidate_result->deprels[i]));
        }
        (*os) << std::endl;
      }
      (*os) << std::endl;
    } else if (mode == kPipeEvaluate) {
      std::vector<ae::State*> final_results;
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

      CoNLLXDependency output;
      build_output((*result.first), output, deprels_alphabet.encode(root));
      loss1 = wrong(output, true, instance.heads, instance.deprels, nr_effective_tokens);
      loss2 = wrong(output, false, instance.heads, instance.deprels, nr_effective_tokens);
      nr_best_recalled_heads += nr_effective_tokens - loss2;
      nr_best_recalled_deprels += nr_effective_tokens - loss1;

      for (const ae::State* candidate_result: final_results) {
        CoNLLXDependency output;
        build_output((*candidate_result), output, deprels_alphabet.encode(root));
        loss1 = wrong(output, true, instance.heads, instance.deprels, nr_effective_tokens);
        loss2 = wrong(output, false, instance.heads, instance.deprels, nr_effective_tokens);

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
          oracle_positive_recalled_heads = recalled_heads; }

        if (oracle_negative_recalled_heads > recalled_heads) {
          oracle_negative_recalled_heads = recalled_heads; }

        if (oracle_positive_recalled_deprels < recalled_deprels) {
          oracle_positive_recalled_deprels = recalled_deprels; }

        if (oracle_negative_recalled_deprels > recalled_deprels) {
          oracle_negative_recalled_deprels = recalled_deprels; }
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
    }

    if ((n+ 1)% display_interval == 0) {
      _INFO << "pipe: processed #" << (n+ 1) << " instances.";
    }
  }
  _INFO << "pipe: processed #" << N << " instances.";

  if (mode == kPipeLearn) {
    learner->set_timestamp(N);
    learner->flush();
    _INFO << "pipe: nr errors: " << learner->errors();
    save_model(model_path);

  } else if (mode == kPipeEvaluate) {
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
    _INFO << "report: best UAS: " << floatval_t(nr_best_recalled_heads) / nr_deprels;
    _INFO << "report: best LAS: " << floatval_t(nr_best_recalled_deprels) / nr_deprels;
  }

  if (os != NULL && os != &std::cout) { delete os; }
}

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar
