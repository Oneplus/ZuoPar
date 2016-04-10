#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_EVALUATE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_EVALUATE_H__

#include <iostream>
#include "types/common.h"
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

struct EvaluationStatistics {
  int n_correct_in_beam;
  int n_correct_in_beam_not_best;
  int n_oracle_positive_recalled_heads;
  int n_oracle_negative_recalled_heads;
  int n_oracle_positive_recalled_deprels;
  int n_oracle_negative_recalled_deprels;
  int n_best_recalled_heads;
  int n_best_recalled_deprels;
  int n_deprels;
  floatval_t avg_uas;
  floatval_t avg_las;

  EvaluationStatistics() :
    n_correct_in_beam(0),
    n_correct_in_beam_not_best(0),
    n_oracle_positive_recalled_heads(0),
    n_oracle_negative_recalled_heads(0),
    n_oracle_positive_recalled_deprels(0),
    n_oracle_negative_recalled_deprels(0),
    n_best_recalled_heads(0),
    n_best_recalled_deprels(0),
    n_deprels(0),
    avg_uas(0),
    avg_las(0) {
  }
};

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_HSTEP_EVALUATE_H__
