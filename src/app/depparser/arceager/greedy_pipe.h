#ifndef __ZUOPAR_APP_DEPPARSER_ARCEAGER_GREEDY_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCEAGER_GREEDY_PIPE_H__

#include "engine/token_alphabet.h"
#include "types/dependency.h"
#include "app/depparser/greedy_opt.h"
#include "app/depparser/arceager/weight.h"
#include "app/depparser/arceager/learner.h"
#include "app/depparser/arceager/decoder.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcEager {

class GreedySearchPipe {
private:
  //! The supported modes.
  enum PipeMode { kPipeLearn, kPipeTest };

private:
  std::string reference_path;
  std::string input_path;
  std::string output_path;
  std::string model_path;
  std::string root;
  int shuffle_times;

  Engine::TokenAlphabet forms_alphabet;
  Engine::TokenAlphabet lemmas_alphabet;
  Engine::TokenAlphabet cpostags_alphabet;
  Engine::TokenAlphabet postags_alphabet;
  Engine::TokenAlphabet deprels_alphabet;
  Engine::TokenAlphabet feat_alphabet;
  std::vector<CoNLLXDependency> dataset;

  PipeMode mode;

  Decoder* decoder;
  Weight* weight;
public:
  GreedySearchPipe(const GreedyLearnOption&);
  GreedySearchPipe(const GreedyTestOption&);
  ~GreedySearchPipe();

  void learn();
  void test();

private:
  bool setup();
  void save_model(const std::string& model_path);
  bool load_model(const std::string& model_path);
  void build_output(const State& state, CoNLLXDependency& output);
};

} //  namespace arceager
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCEAGER_GREEDY_PIPE_H__
