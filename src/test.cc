#include "system/system.h"
#include <vector>

struct Action {
  int first;
  int second;
};

struct State {
  int top;
};

struct ScoreContext {
  int word;
  int postag;
};

struct Input {
  std::vector<int> words;
};

int main() {
  sys = ZuoPar::TransitionSystem<Action, State, ScoreContext, Input>();
  return 0;
}
