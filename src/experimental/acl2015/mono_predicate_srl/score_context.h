#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_SCORE_CONTEXT_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_SCORE_CONTEXT_H__

#include <vector>
#include "types/common.h"
#include "types/semchunks.h"
#include "engine/token_alphabet.h"
#include "experimental/acl2015/mono_predicate_srl/state.h"
#include "experimental/acl2015/mono_predicate_srl/action_utils.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MonoPredicateSRL {

namespace eg = ZuoPar::Engine;

class ScoreContext {
public:
  /**
   * The ScoreContext constructor
   *
   *  @param[in]  state   The State
   */
  ScoreContext(const State& state)
    : w_2(0), w_1(0), w0(0), w1(0), w2(0),
    p_2(0), p_1(0), p0(0), p1(0), p2(0), w_pred(0),
    p_pred(0), p_pred_1(0), p_pred1(0), vc_pred(0),
    path(state.paths->payload[state.buffer]) {
    const MonoSemanticChunks* ref = state.ref;
    const std::vector<form_t>& forms = ref->forms;
    const std::vector<postag_t>& postags= ref->postags;
    const std::vector<tag_t>& senses = ref->senses;

    int j = state.buffer;
    int M = ref->size();

    tag_t tag;
    if (NULL == state.previous) {
      t_1 = 0;
    } else if (ActionUtils::is_O(state.last_action)) {
      t_1 = kSemanticChunkOuterTag;
    } else if (ActionUtils::is_B(state.last_action, tag)) {
      t_1 = kSemanticChunkBeginTag+ tag;
    } else if (ActionUtils::is_I(state.last_action, tag)) {
      t_1 = kSemanticChunkInterTag+ tag;
    }

    if (j- 2 >= 0) {
      w_2 = forms[j- 2];
      p_2 = postags[j- 2];
    } else {
      w_2 = 1;
      p_2 = 1;
    }

    if (j- 1 >= 0) {
      w_1 = forms[j- 1];
      p_1 = postags[j- 1];
    } else {
      w_1 = 1;
      p_1 = 1;
    }

    w0 = forms[j];
    p0 = postags[j];

    if (j+ 1 < M) {
      w1 = forms[j+ 1];
      p1 = postags[j+ 1];
    } else {
      w1 = 2;
      p1 = 2;
    }

    if (j+ 2 < M) {
      w2 = forms[j+ 2];
      p2 = postags[j+ 2];
    } else {
      w2 = 2;
      p2 = 2;
    }

    w_pred = forms[ref->predicate.first];
    p_pred = postags[ref->predicate.first];
    //vc_pred = senses[ref->predicate.first];
    vc_pred  = state.verb_class;
    p_pred_1 = (ref->predicate.first >= 1 ? postags[ref->predicate.first- 1]: 1);
    p_pred1 = (ref->predicate.first+ 1 < M ? postags[ref->predicate.first+ 1]: 2);

    position = j < ref->predicate.first ? 1: 0;
    dist= std::abs(j - ref->predicate.first);
  };

  form_t    w_2, w_1, w0, w1, w2;
  postag_t  p_2, p_1, p0, p1, p2;
  form_t    w_pred;
  postag_t  p_pred, p_pred_1, p_pred1;
  tag_t     vc_pred;
  tag_t     t_1;
  int position;
  int dist;
  const std::string& path;
};

} //  namespace monopredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MONO_PREDICATE_SRL_SCORE_CONTEXT_H__
