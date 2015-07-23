#include "app/seqlabeler/trans/weight.h"
#include "app/seqlabeler/trans/action.h"
#include "app/seqlabeler/trans/score_context.h"

namespace ZuoPar {
namespace SequenceLabeler {

Weight::Weight() {
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector<ufp_t>& cache) -> void {
        const std::vector<attribute_t>& attributes = ctx.instance->at(ctx.buffer).attributes;
        for (int i = 0; i < attributes.size(); ++ i) {
          cache.push_back( ufp_t(attributes[i]) );
        }
        }
        )
      );
  ufeat_map_repo.push_back( uf_map_t(
        [](const ScoreContext& ctx, std::vector<ufp_t>& cache) -> void {
        cache.push_back( ufp_t(ctx.T_1) );
        }
        )
      );
}

} //  end for namespace sequencelabeler
} //  end for namespace zuopar
