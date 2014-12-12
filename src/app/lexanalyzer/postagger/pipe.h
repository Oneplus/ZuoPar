#ifndef __ZUOPAR_APP_LEXANALYZER_POSTAGGER_PIPE_H__
#define __ZUOPAR_APP_LEXANALYZER_POSTAGGER_PIPE_H__

#include "types/postag.h"
#include "engine/token_alphabet.h"
#include "app/lexanalyzer/postagger/opt.h"
#include "app/lexanalyzer/postagger/weight.h"
#include "app/lexanalyzer/postagger/decoder.h"
#include "app/lexanalyzer/postagger/learner.h"

namespace ZuoPar {
namespace LexicalAnalyzer {
namespace Postagger {

namespace eg = ZuoPar::Engine;

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
  bool load_model(const std::string& model_path);

  /**
   * Build the dependency output for the state chain which ends with the source
   * state.
   *
   *  @param[in]  source  The last state in state chain.
   *  @param[out] output  The output dependency parse.
   */
  void build_output(const State& source, PostagWithLiteralCache& output);
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

  //!
  std::string output_path;

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

  //! The dataset.
  std::vector<PostagWithLiteralCache> dataset;
};


} //  end for namespace postagger
} //  end for namespace lexicalanalyzer
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_LEXANALYZER_POSTAGGER_PIPE_H__
