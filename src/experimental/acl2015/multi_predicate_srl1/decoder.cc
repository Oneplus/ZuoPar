#include "experimental/acl2015/multi_predicate_srl1/decoder.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"
#include "experimental/acl2015/multi_predicate_srl1/action_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

Decoder::Decoder(int nr, int beam_size, UpdateStrategy strategy,
    Weight* weight)
  : nr_tags(nr),
  TransitionSystem<
    ActionCollection,
    State,
    ScoreContext,
    Weight>(beam_size, strategy, weight) {

  slots.resize( beam_size );
}

int
Decoder::slot_push_back(std::vector<SlotItem>& slot,
    const SlotItem& item) {
  if (this->beam_size == slot.size()) {
    if (item.score > slot[0].score) {
      std::pop_heap(slot.begin(), slot.end());
      slot[this->beam_size- 1] = item;
      std::push_heap(slot.begin(), slot.end());
    }
    return 0;
  }

  slot.push_back(item);
  std::push_heap(slot.begin(), slot.end());
  return 1;
}

void
Decoder::slot_push_back_O(std::vector<SlotItem>& slot,
    const SlotItem* previous,
    const PackedScores<Action>& meta) {
  Action act = ActionFactory::make_O();
  PackedScores<Action>::const_iterator entry = meta.find(act);
  floatval_t score = 0.;
  if (previous) { score += previous->score; }
  if (entry != meta.end()) { score += entry->second;}

  slot_push_back(slot, SlotItem(act, score, previous));
}

void
Decoder::slot_push_back_B(std::vector<SlotItem>& slot,
    const SlotItem* previous,
    const PackedScores<Action>& meta,
    const tag_t& tag) {
  Action act = ActionFactory::make_B(tag);
  PackedScores<Action>::const_iterator entry = meta.find(act);
  floatval_t score = 0.;
  if (previous) { score += previous->score; }
  if (entry != meta.end()) { score += entry->second;}

  slot_push_back(slot, SlotItem(act, score, previous));
}

void
Decoder::slot_push_back_I(std::vector<SlotItem>& slot,
    const SlotItem* previous,
    const PackedScores<Action>& meta,
    const tag_t& tag) {
  Action act = ActionFactory::make_I(tag);
  PackedScores<Action>::const_iterator entry = meta.find(act);
  floatval_t score = 0.;
  if (previous) { score += previous->score; }
  if (entry != meta.end()) { score += entry->second;}

  slot_push_back(slot, SlotItem(act, score, previous));
}

void
Decoder::score_possible_actions(const State& source,
    const std::vector<ActionCollection>& actions,
    PackedScores<ActionCollection>& scores) {
  scores.clear();
  for (const ActionCollection& collection: actions) {
    floatval_t val = 0.;
    for (int j = 0; j < collection.size(); ++ j) {
      const Action& act = collection[j];
      val += meta_packed_scores[j][act];
    }
    scores[collection] = val;
    _TRACE << "decoder: score possible actions: " << collection << " -> " << val;
  }
}

void
Decoder::make_meta_packed_scores(ScoreContext& ctx, int nr_predicates) {
  meta_packed_scores.resize(nr_predicates);
  Action act;
  for (int i = 0; i < nr_predicates; ++ i) {
    ctx.reset_predicate_rank(i);
    PackedScores<Action>& meta = meta_packed_scores[i];
    meta.clear();

    act = ActionFactory::make_O();
    meta[act] = this->model->score(ctx, act, this->use_avg);
    for (int j = 0; j < nr_tags; ++ j) {
      act = ActionFactory::make_B(j);
      meta[act] = this->model->score(ctx, act, this->use_avg);

      act = ActionFactory::make_I(j);
      meta[act] = this->model->score(ctx, act, this->use_avg);
    }
  }
}

//! Problem, the avg is invisiable to this function.
void
Decoder::get_possible_actions(const State& source,
    std::vector<ActionCollection>& actions) {
  int buffer = source.buffer;
  int nr_predicates = source.ref->nr_predicates();

  ScoreContext ctx(source);
  make_meta_packed_scores(ctx, nr_predicates);

  if (buffer == 0) {
    for (int i = 0; i < nr_predicates; ++ i) {
      slots[i].clear();
      if (i == 0) {
        slot_push_back_O(slots[i], NULL, meta_packed_scores[i] );
        for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
          slot_push_back_B(slots[i], NULL, meta_packed_scores[i], p);
        }
      } else {
        for (int j = 0; j < slots[i- 1].size(); ++ j) {
          const SlotItem* previous = &slots[i- 1][j];
          slot_push_back_O(slots[i], previous, meta_packed_scores[i]);

          for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
            slot_push_back_B(slots[i], previous, meta_packed_scores[i], p);
          }
        }
      }
    }
  } else {
    BOOST_ASSERT_MSG(state.last_action.size() == nr_predicates,
        "Number of predicate unmatch!");
    for (int i = 0; i < nr_predicates; ++ i) {
      slots[i].clear();

      if (i == 0) {
        slot_push_back_O(slots[i], NULL, meta_packed_scores[i] );
        for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
          slot_push_back_B(slots[i], NULL, meta_packed_scores[i], p);
        }
      } else {
        tag_t tag = -1;
        ActionUtils::is_B(source.last_action[i], tag);
        if (tag == -1) { ActionUtils::is_I(source.last_action[i], tag); }

        for (int j = 0; j < slots[i- 1].size(); ++ j) {
          const SlotItem* previous = &slots[i- 1][j];
          slot_push_back_O(slots[i], previous, meta_packed_scores[i]);

          for (tag_t p = eg::TokenAlphabet::END+ 1; p < nr_tags; ++ p) {
            slot_push_back_B(slots[i], previous, meta_packed_scores[i], p);
          }

          if (tag != -1) {
            slot_push_back_I(slots[i], previous, meta_packed_scores[i], tag);
          }
        }
      }
    }
  }

  actions.clear();
  for (int i = 0; i < slots[nr_predicates- 1].size(); ++ i) {
    ActionCollection collection;

    const SlotItem* item = &slots[nr_predicates- 1][i];
    while (item) {
      collection.push_back(item->act);
      item = item->previous;
    }
    std::reverse(collection.begin(), collection.end());
    actions.push_back(collection);
  }

  for (size_t i = 0; i < actions.size(); ++ i) {
    _TRACE << "decoder: get possible actions #" << i << " " << actions[i];
  }
}

void
Decoder::transit(const State& source, const ActionCollection& act, const floatval_t& score,
    State* target) {
  target->take(source, act);
  target->score = score;
}

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace experimental
} //  namespace zuopar
