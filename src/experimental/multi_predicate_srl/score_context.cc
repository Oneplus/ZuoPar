#include "experimental/multi_predicate_srl/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

ScoreContext::ScoreContext(const State& state_)
  : state(state_),
  w_2(0), w_1(0), w0(0), w1(0), w2(0),
  p_2(0), p_1(0), p0(0), p1(0), p2(0),
  w_pred(0), p_pred(0), p_pred_1(0), p_pred1(0), vc_pred(0),
  label_set(0),
  predicates_id(state.info->predicates_id),
  predicates_form0(state.info->predicates_form0),
  predicates_postag0(state.info->predicates_postag0),
  predicates_postag1(state.info->predicates_postag1),
  predicates_postag_1(state.info->predicates_postag_1),
  predicates_verbclass(state.info->predicates_verbclass),
  postags_path_to_predicate(state.info->postags_path_to_predicates),
  path(NULL) {
  const SemanticChunks* ref = state.ref;
  const std::vector<form_t>& forms = ref->forms;
  const std::vector<postag_t>& postags = ref->postags;

  id = state.buffer;
  int M = ref->size();
  predicate_rank = 0;

  if (NULL == state.previous) {
    t_1 = eg::TokenAlphabet::BEGIN;
  } else {
    t_1 = state.last_action[predicate_rank].code();
  }

  if (id >= 2) { w_2 = forms[id- 2]; p_2 = postags[id- 2]; }
  else { w_2 = p_2 = eg::TokenAlphabet::BEGIN; }

  if (id >= 1) { w_1 = forms[id- 1]; p_1 = postags[id- 1]; }
  else { w_1 = p_1 = eg::TokenAlphabet::BEGIN; }

  w0 = forms[id];
  p0 = postags[id];

  if (id+ 1 < M) { w1 = forms[id+ 1]; p1 = postags[id+ 1]; }
  else { w1 = p1 = eg::TokenAlphabet::END; }

  if (id+ 2 < M) { w2 = forms[id+ 1]; p2 = postags[id+ 2]; }
  else { w2 = p2 = eg::TokenAlphabet::END; }

  w_pred  = predicates_form0[predicate_rank];
  p_pred  = predicates_postag0[predicate_rank];
  p_pred1 = predicates_postag1[predicate_rank];
  p_pred_1= predicates_postag_1[predicate_rank];
  vc_pred  = predicates_verbclass[predicate_rank];
  label_set = state.label_set[predicate_rank];

  int predicate_id = predicates_id[predicate_rank];
  position = (id < predicate_id ? 1: (id == predicate_id ? 2: 0));
  dist= (id < predicate_id ? predicate_id- id : id- predicate_id);
  path= &postags_path_to_predicate[predicate_rank][id];
}

void
ScoreContext::reset_predicate_rank(int rank) {
  BOOST_ASSERT_MSG(rank < predicates_form0.size(), "Index of rank exceed!");
  if (predicate_rank == rank) {
    return;
  }

  predicate_rank = rank;
  w_pred  = predicates_form0[predicate_rank];
  p_pred  = predicates_postag0[predicate_rank];
  p_pred1 = predicates_postag1[predicate_rank];
  p_pred_1= predicates_postag_1[predicate_rank];
  vc_pred = predicates_verbclass[predicate_rank];
  label_set = state.label_set[predicate_rank];

  int predicate_id = predicates_id[predicate_rank];
  position = (id < predicate_id ? 1: (id == predicate_id ? 2: 0));
  dist= (id > predicate_id ? id- predicate_id : predicate_id- id);
  path= &postags_path_to_predicate[predicate_rank][id];

  if (NULL == state.previous) {
    t_1 = eg::TokenAlphabet::BEGIN;
  } else {
    t_1 = state.last_action[predicate_rank].code();
  }
}

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar
