#ifndef __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_SCORE_CONTEXT_H__
#define __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "types/semchunks.h"
#include "engine/token_alphabet.h"
#include "experimental/multi_predicate_srl/state.h"
#include "experimental/multi_predicate_srl/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state_);

  void reset_predicate_rank(int rank);

  const State& state;
  form_t    w_2, w_1, w0, w1, w2;
  postag_t  p_2, p_1, p0, p1, p2;
  form_t    w_pred;
  postag_t  p_pred, p_pred_1, p_pred1;
  tag_t     vc_pred;
  tag_t     t_1;
  const std::string* path;
  int position;
  int dist;
  int label_set;
  int id;
  int predicate_rank;

  const std::vector< std::vector<std::string> >& postags_path_to_predicate;
  const std::vector<form_t>& predicates_form0;
  const std::vector<postag_t>& predicates_postag0;
  const std::vector<postag_t>& predicates_postag_1;
  const std::vector<postag_t>& predicates_postag1;
  const std::vector<tag_t>& predicates_verbclass;
  const std::vector<int>& predicates_id;
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_SCORE_CONTEXT_H__
