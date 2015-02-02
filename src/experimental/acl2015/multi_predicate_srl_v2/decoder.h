#ifndef __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__
#define __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__

#include "engine/token_alphabet.h"
#include "system/system.h"
#include "types/internal/packed_scores.h"
#include "experimental/acl2015/multi_predicate_srl_v2/state.h"
#include "experimental/acl2015/multi_predicate_srl_v2/action.h"
#include "experimental/acl2015/multi_predicate_srl_v2/score_context.h"
#include "experimental/acl2015/multi_predicate_srl_v2/weight.h"

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

struct CubeItem {
  //CubeItem(): action(0), score(0), previous(NULL) {}
  CubeItem(int act_, floatval_t score_, const CubeItem* previous_)
    : action(act_), score(score_), previous(previous_) {}

  bool operator < (const CubeItem& other) const { return score > other.score; }
  CubeItem& operator = (const CubeItem& other) {
    action = other.action; score = other.score; previous = other.previous;
    return (*this);
  }
  friend std::ostream& operator << (std::ostream& os, const CubeItem& item) {
    os << item.action << " - " << item.score << " - " << (void *)(item.previous);
    return os;
  }

  int action;               //! The compressed_action;
  floatval_t score;         //! The score of the slot.
  const CubeItem* previous; //! The previous slot item.
};

bool cube_item_greater(const CubeItem* first, const CubeItem* second);

//! push a cube item into the cube.
int cube_push_back(std::vector<CubeItem>& cell, const CubeItem& item, int height);

//!
void cube_make(std::vector<std::vector<std::vector<CubeItem> > >& cube,
    int nr_rows, int nr_columns);

//! Perform a cubic pruning on the get_possible_actions function.
class Decoder : public sys {
public:
  /**
   * The decoder constructor.
   *
   *  @param[in]  nr_tags         The number of semantic rols.
   *  @param[in]  cap_predicates  The maximum number of predicates.
   *  @param[in]  beam_size       The size of beam.
   *  @param[in]  cube_size       The height of cube for cubic pruning.
   *  @param[in]  weight          The pointer to the weight.
   */
  Decoder(int nr_tags, int cap_predicates, int beam_size, int cube_size,
      bool avg, UpdateStrategy strategy, Weight* weight);

  //! Implement arc standard get possible actions.
  void get_possible_actions(const State& source,
      std::vector<ActionCollection>& actions);

  //! Implement the possible actions scoring functions.functions
  void score_possible_actions(const State& source,
      const std::vector<ActionCollection>& actions,
      PackedScores<ActionCollection>& scores);

  //! make the meta packed score cache.
  void calculate_meta_packed_scores(ScoreContext& ctx, int nr_predicates);

  //! Implement arc standard transition.
  void transit(const State& source, const ActionCollection& act,
      const floatval_t& score, State* target);

private:
  //! number of tags
  int nr_tags;

  //! The maximun number of predicates.
  int cap_predicates;

  //! The hight of cube in cube proning.
  int cube_size;

  //! The action beam.
  std::vector<
    std::vector<
      std::vector<CubeItem>
    >
  > cube;

  //! The packed score.
  std::vector< std::vector<floatval_t> > meta_packed_scores;
};

} //  namespace multipredicatesrl
} //  namespace acl2015
} //  namespace lexicalanalyzer
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_ACL2015_MULTI_PREDICATE_SRL_H__
