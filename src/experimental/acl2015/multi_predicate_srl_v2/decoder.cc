#include "experimental/acl2015/multi_predicate_srl_v2/decoder.h"
#include "experimental/acl2015/multi_predicate_srl_v2/action.h"
#include "experimental/acl2015/multi_predicate_srl_v2/action_utils.h"
#include "utils/logging.h"

namespace ZuoPar {
namespace Experimental {
namespace ACL2015 {
namespace MultiPredicateSRL {

void cube_make(std::vector<std::vector<std::vector<CubeItem> > >& cube,
    int nr_rows, int nr_columns) {
  cube.resize(nr_rows);
  for (int i = 0; i < nr_rows; ++ i) {
    cube[i].resize(nr_columns);
  }
}

int cube_push_back(std::vector<CubeItem>& cell,
    const CubeItem& item,
    int beam_size) {
  if (beam_size== cell.size()) {
    if (item.score > cell[0].score) {
      std::pop_heap(cell.begin(), cell.end());
      cell[beam_size- 1] = item;
      std::push_heap(cell.begin(), cell.end());
    }
    return 0;
  }

  cell.push_back(item);
  std::push_heap(cell.begin(), cell.end());
  return 1;
}

bool cube_item_greater(const CubeItem* first, const CubeItem* second) {
  return first->score > second->score;
}

Decoder::Decoder(int nr_tags_, int cap_predicates_, int beam_size, int cube_size_,
    UpdateStrategy strategy,
    Weight* weight)
  : nr_tags(nr_tags_),
  cap_predicates(cap_predicates_),
  cube_size(cube_size_),
  TransitionSystem<
    ActionCollection,
    State,
    ScoreContext,
    Weight>(beam_size, strategy, weight) {
  // init the cube.
  int cap_tags = ActionUtils::max(nr_tags);
  cube_make(cube, cap_predicates, cap_tags);
}

void
Decoder::score_possible_actions(const State& source,
    const std::vector<ActionCollection>& actions,
    PackedScores<ActionCollection>& scores) {
  scores.clear();
  for (const ActionCollection& collection: actions) {
    floatval_t val = 0.;
    for (int rank = 0; rank < collection.size(); ++ rank) {
      const Action& act = collection[rank];
      val += meta_packed_scores[rank][ActionUtils::compress(act, nr_tags)];
    }
    scores[collection] = val;
    //_TRACE << "decoder: score possible actions: " << collection << " -> " << val;
  }
}

void
Decoder::calculate_meta_packed_scores(ScoreContext& ctx, int nr_predicates) {
  meta_packed_scores.resize(nr_predicates);
  Action act;
  int M = ActionUtils::max(nr_tags);
  for (int i = 0; i < nr_predicates; ++ i) {
    ctx.reset_predicate_rank(i);
    std::vector<floatval_t>& meta = meta_packed_scores[i];

    meta.resize(M);

    act = ActionFactory::make_O();
    meta[ActionUtils::compress(act, nr_tags)] = this->model->score(ctx, act, this->use_avg);

    for (int j = eg::TokenAlphabet::END+ 1; j < nr_tags; ++ j) {
      act = ActionFactory::make_B(j);
      meta[ActionUtils::compress(act, nr_tags)] = this->model->score(ctx, act, this->use_avg);

      act = ActionFactory::make_I(j);
      meta[ActionUtils::compress(act, nr_tags)] = this->model->score(ctx, act, this->use_avg);
    }
  }
}

void
Decoder::get_possible_actions(const State& source,
    std::vector<ActionCollection>& actions) {
  int buffer = source.buffer;
  int nr_predicates = source.ref->nr_predicates();
  int total_tags = ActionUtils::max(nr_tags);

  ScoreContext ctx(source);
  calculate_meta_packed_scores(ctx, nr_predicates);

  int first_i_tag = ActionUtils::compress(ActionFactory::make_I(3), nr_tags);
  if (buffer == 0) {
    for (int rank = 0; rank < nr_predicates; ++ rank) {
      const std::vector<floatval_t>& SCORES = meta_packed_scores[rank];
      for (int tag = 0; tag < first_i_tag; ++ tag) {
        cube[rank][tag].clear();

        if (rank == 0) {
          cube_push_back(cube[rank][tag], CubeItem(tag, SCORES[tag], NULL), cube_size);
        } else {
          for (int prev_tag = 0; prev_tag < first_i_tag; ++ prev_tag) {
            for (int i = 0; i < cube[rank- 1][prev_tag].size(); ++ i) {
              const CubeItem* previous = &cube[rank- 1][prev_tag][i];
              cube_push_back(cube[rank][tag],
                  CubeItem(tag, previous->score + SCORES[tag], previous), cube_size);
            }
          }
        }
      }
    }
  } else {
    BOOST_ASSERT_MSG(source.last_action.size() == nr_predicates,
        "Number of predicate unmatch!");
    for (int rank = 0; rank < nr_predicates; ++ rank) {
      const std::vector<floatval_t>& SCORES = meta_packed_scores[rank];
      for (int tag = 0; tag < first_i_tag; ++ tag) {
        cube[rank][tag].clear();

        if (rank == 0) {
          cube_push_back(cube[rank][tag], CubeItem(tag, SCORES[tag], NULL), cube_size);
        } else {
          for (int prev_tag = 0; prev_tag < total_tags; ++ prev_tag) {
            for (int i = 0; i < cube[rank- 1][prev_tag].size(); ++ i) {
              const CubeItem* previous = &cube[rank- 1][prev_tag][i];
              cube_push_back(cube[rank][tag],
                  CubeItem(tag, previous->score + SCORES[tag], previous), cube_size);
            }
          }
        }
      }

      tag_t mask= -1, tag = -1;
      if (ActionUtils::is_B(source.last_action[rank], mask) ||
          ActionUtils::is_I(source.last_action[rank], mask)) {
        tag = ActionUtils::compress(ActionFactory::make_I(mask), nr_tags);
        cube[rank][tag].clear();
        //_TRACE << "mask " << mask << ", tag " << tag;
        if (rank == 0) {
          cube_push_back(cube[rank][tag], CubeItem(tag, SCORES[tag], NULL), cube_size);
        } else {
          for (int prev_tag = 0; prev_tag < first_i_tag; ++ prev_tag) {
            for (int i = 0; i < cube[rank- 1][prev_tag].size(); ++ i) {
              const CubeItem* previous = &cube[rank- 1][prev_tag][i];
              cube_push_back(cube[rank][tag],
                  CubeItem(tag, previous->score + SCORES[tag], previous), cube_size);
            }
          }
        }
      }
    }
  }

  actions.clear();
  std::vector<const CubeItem*> last_resort;

  for (int i = 0; i < total_tags; ++ i) {
    for (int j = 0; j < cube[nr_predicates- 1][i].size(); ++ j) {
      last_resort.push_back( &cube[nr_predicates- 1][i][j] );
    }
  }

  std::sort(last_resort.begin(), last_resort.end(), cube_item_greater);

  for (int i = 0; i < std::min((int)last_resort.size(), this->beam_size); ++ i) {
    ActionCollection collection;

    const CubeItem* item = last_resort[i];
    while (item) {
      collection.push_back(ActionUtils::decompress(item->action, nr_tags));
      item = item->previous;
    }
    std::reverse(collection.begin(), collection.end());
    actions.push_back(collection);
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
