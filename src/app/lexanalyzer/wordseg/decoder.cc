#include "app/lexanalyzer/wordseg/decoder.h"
#include "app/lexanalyzer/wordseg/action_utils.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace ChineseWordSegmentor {

Decoder::Decoder(int beam_size, bool avg, Weight* weight)
  : TransitionSystem<Action, State, Weight>(
      beam_size, avg, UpdateStrategy::kEarlyUpdate, weight) {
}

void Decoder::get_possible_actions(const State& source,
    std::vector<Action>& actions) {
  actions.clear();
  actions.push_back(ActionFactory::make_append());
  actions.push_back(ActionFactory::make_split());
}

void Decoder::transit(const State& source, const Action& act, const floatval_t& score,
    State* target) {
  if (ActionUtils::is_split(act))       { target->split(source); }
  else if (ActionUtils::is_append(act)) { target->append(source); }
  else { std::cerr << "unknown action"; }
  target->score = score;
}

bool Decoder::terminated() { return false; }

} //  end for namespace chinesewordsegmentor
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar
