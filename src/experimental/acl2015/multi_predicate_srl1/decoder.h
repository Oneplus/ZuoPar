#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "types/internal/packed_scores.h"
#include "experimental/acl2015/multi_predicate_srl1/state.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"
#include "experimental/acl2015/multi_predicate_srl1/score_context.h"
#include "experimental/acl2015/multi_predicate_srl1/action.h"
#include "experimental/acl2015/multi_predicate_srl1/weight.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

namespace eg = ZuoPar::Engine;

typedef TransitionSystem<
  ActionCollection,
  State,
  ScoreContext,
  Weight
> sys;

class Decoder : public sys {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr        The number of dependency relations.
   *  @param[in]  beam_size The size of beam.
   *  @param[in]  weight    The pointer to the weight.
   */
  Decoder(int nr, int beam_size, UpdateStrategy update_strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<ActionCollection>& actions);

  //! Implement the possible actions scoring functions.functions
  void score_possible_actions(const State& source,
      const std::vector<ActionCollection>& actions,
      PackedScores<ActionCollection>& scores);

  //! make the meta packed score cache.
  void make_meta_packed_scores(ScoreContext& ctx, int nr_predicates);

  //! Implement arc standard transition.
  void transit(const State& source, const ActionCollection& act,
      const floatval_t& score, State* target);

private:
  //! number of tags
  int nr_tags;

  struct SlotItem {
    SlotItem()
      : act(),
      score(0),
      previous(NULL) {
    }

    SlotItem(const Action& act_,
        floatval_t score_,
        const SlotItem* previous_)
      : act(act_),
      score(score_),
      previous(previous_) {
    }

    bool operator < (const SlotItem& other) const {
      return score > other.score;
    }

    friend std::ostream& operator << (std::ostream& os, const SlotItem& item) {
      os << item.act << " - " << item.score << " - " << (void *)(item.previous);
      return os;
    }

    Action          act;      //! The action in the table.
    floatval_t      score;    //! The score of the slot.
    const SlotItem* previous; //! The previous slot item.
  };

  int slot_push_back(std::vector<SlotItem>& slot, const SlotItem& item);

  void slot_push_back_B(std::vector<SlotItem>& slot,
      const SlotItem* previous,
      const PackedScores<Action>& meta,
      const tag_t& tag);

  void slot_push_back_I(std::vector<SlotItem>& slot,
      const SlotItem* previous,
      const PackedScores<Action>& meta,
      const tag_t& tag);

  void slot_push_back_O(std::vector<SlotItem>& slot,
      const SlotItem* previous,
      const PackedScores<Action>& meta);

  //! The action beam.
  std::vector< std::vector<SlotItem> > slots;

  //! The packed score.
  std::vector< PackedScores<Action> > meta_packed_scores;
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace lexicalanalyzer
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__
