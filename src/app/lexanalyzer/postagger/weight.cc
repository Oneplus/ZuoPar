#include "app/lexanalyzer/postagger/weight.h"
#include "app/lexanalyzer/postagger/action.h"
#include "app/lexanalyzer/postagger/score_context.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

Weight::Weight() {
  repo.push_back(
      map_t(
        [](const ScoreContext& ctx, std::vector<std::string>& cache) -> void { 
        cache.push_back( ctx.W0 ); }
        )
      );
}

} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
