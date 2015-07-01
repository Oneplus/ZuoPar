#include "app/semchunker/mono/score_context.h"

namespace ZuoPar {
namespace SemanticChunker {
namespace MonoPredicate {

ScoreContext::ScoreContext(const State& state)
  : w_2(0), w_1(0), w0(0), w1(0), w2(0), dist(0), position(0),
  p_2(0), p_1(0), p0(0), p1(0), p2(0),
  w_pred(state.info->predicate_form0),
  p_pred(state.info->predicate_postag0),
  p_pred_1(state.info->predicate_postag_1),
  p_pred1(state.info->predicate_postag1),
  vc_pred(state.info->predicate_verbclass),
  label_set(state.label_set),
  path(state.info->postag_paths[state.buffer]) {

  const MonoSemanticChunks* ref = state.ref;
  const std::vector<form_t>& forms = ref->forms;
  const std::vector<postag_t>& postags = ref->postags;

  int id= state.buffer;
  int predicate_id = state.info->predicate_id;
  int M = ref->size();

  if (NULL == state.previous) { t_1 = eg::TokenAlphabet::BEGIN; }
  else { t_1 = boost::hash<Action>()(state.last_action); }

  if (id >= 2) { w_2 = forms[id- 2]; p_2 = postags[id- 2]; }
  else { w_2 = p_2 = eg::TokenAlphabet::BEGIN; }

  if (id >= 1) { w_1 = forms[id- 1]; p_1 = postags[id- 1]; }
  else { w_1 = p_1 = eg::TokenAlphabet::BEGIN; }

  w0 = forms[id];
  p0 = postags[id];

  if (id+ 1 < M) { w1 = forms[id+ 1]; p1 = postags[id+ 1]; }
  else { w1 = p1 = eg::TokenAlphabet::END; }

  if (id+ 2 < M) { w2 = forms[id+ 2]; p2 = postags[id+ 2];}
  else { w2 = p2 = eg::TokenAlphabet::END; }

  // position type should not equals to 0 because of the dense_hash problem.
  position = (id < predicate_id ? 2: (id == predicate_id ? 3: 1));
  dist = (id < predicate_id?
      (predicate_id - id):
      (id == predicate_id ? -1: (id - predicate_id)));
};

} //  namespace monopredicate
} //  namespace semanticchunker
} //  namespace zuopar
