#include "experimental/acl2015/multi_predicate_srl1/score_context.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
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

  j = state.buffer;
  int M = ref->size();
  predicate_rank = 0;

  tag_t tag;
  if (NULL == state.previous) {
    t_1 = eg::TokenAlphabet::BEGIN;
  } else if (ActionUtils::is_O(state.last_action[predicate_rank])) {
    t_1 = kSemanticChunkOuterTag;
  } else if (ActionUtils::is_B(state.last_action[predicate_rank], tag)) {
    t_1 = kSemanticChunkBeginTag+ tag;
  } else if (ActionUtils::is_I(state.last_action[predicate_rank], tag)) {
    t_1 = kSemanticChunkInterTag+ tag;
  } else {
    BOOST_ASSERT_MSG(false, "Unknown tag!");
  }

  if (j >= 2) { w_2 = forms[j- 2]; p_2 = postags[j- 2]; }
  else { w_2 = p_2 = eg::TokenAlphabet::BEGIN; }

  if (j >= 1) { w_1 = forms[j- 1]; p_1 = postags[j- 1]; }
  else { w_1 = p_1 = eg::TokenAlphabet::BEGIN; }

  w0 = forms[j];
  p0 = postags[j];

  if (j+ 1 < M) { w1 = forms[j+ 1]; p1 = postags[j+ 1]; }
  else { w1 = p1 = eg::TokenAlphabet::END; }

  if (j+ 2 < M) { w2 = forms[j+ 1]; p2 = postags[j+ 2]; }
  else { w2 = p2 = eg::TokenAlphabet::END; }

  w_pred  = predicates_form0[predicate_rank];
  p_pred  = predicates_postag0[predicate_rank];
  p_pred1 = predicates_postag1[predicate_rank];
  p_pred_1= predicates_postag_1[predicate_rank];
  vc_pred  = predicates_verbclass[predicate_rank];

  int predicate_id = predicates_id[predicate_rank];
  position = (j < predicate_id ? 1: (j == predicate_id ? 2: 0));
  dist= (j > predicate_id ? j- predicate_id : predicate_id- j);
  path= &postags_path_to_predicate[predicate_rank][j];
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

  int predicate_id = predicates_id[predicate_rank];
  position = (j < predicate_id ? 1: (j == predicate_id ? 2: 0));
  dist= (j > predicate_id ? j- predicate_id : predicate_id- j);
  path= &postags_path_to_predicate[predicate_rank][j];

  tag_t tag;
  if (NULL == state.previous) {
    t_1 = eg::TokenAlphabet::BEGIN;
  } else if (ActionUtils::is_O(state.last_action[predicate_rank])) {
    t_1 = kSemanticChunkOuterTag;
  } else if (ActionUtils::is_B(state.last_action[predicate_rank], tag)) {
    t_1 = kSemanticChunkBeginTag+ tag;
  } else if (ActionUtils::is_I(state.last_action[predicate_rank], tag)) {
    t_1 = kSemanticChunkInterTag+ tag;
  } else {
    BOOST_ASSERT_MSG(false, "Unknown tag!");
  }
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
