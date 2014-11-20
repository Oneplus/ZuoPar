#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/arcstandard/opt.h"
#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/decoder.h"
#include "app/depparser/arcstandard/learner.h"
namespace eg = ZuoPar::Engine;

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class Pipe {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  Pipe(const LearnOption& opts);

  /**
   * The testing mode constructor.
   *
   *  @param[in]  opts  The testing options.
   */
  Pipe(const TestOption& opts);

  /**
   * Perform learning or testing according to the configuration.
   */
  void run();

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  void load_model(const std::string& model_path);

  /**
   * Build the dependency output for the state chain which ends with the source
   * state.
   *
   *  @param[in]  source  The last state in state chain.
   *  @param[out] output  The output dependency parse.
   */
  void build_output(const State& source, Dependency& output);
private:
  //! The supported modes.
  enum PipeMode { kPipeLearn, kPipeTest };

  //! Use to specify if perform training.
  PipeMode mode;

  //! The path to the reference file.
  std::string reference_path;

  //! The path to the model file.
  std::string model_path;

  //!
  std::string input_path;

  //! The size of the beam.
  int beam_size;

  //! The display.
  int display_interval;

  //! The pointer to the weights instances which is pointwise averaged
  //! perceptron model.
  Weight* weight;

  //! The parameter learner.
  Learner* learner;

  //! The pointer to the decoder.
  Decoder* decoder;

  //! The alphabets of forms.
  eg::TokenAlphabet forms_alphabet;

  //! The alphabets of postags.
  eg::TokenAlphabet postags_alphabet;

  //! The alphabets of dependency relations.
  eg::TokenAlphabet deprels_alphabet;

  //! The dataset.
  std::vector<Dependency> dataset;
};


} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
