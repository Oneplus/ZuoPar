#ifndef __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_PIPE_H__

#include <unordered_map>
#include "types/semchunks.h"
#include "frontend/common_pipe_cfg.h"
#include "engine/token_alphabet.h"
#include "experimental/multi_predicate_srl/opt.h"
#include "experimental/multi_predicate_srl/weight.h"
#include "experimental/multi_predicate_srl/decoder.h"
#include "experimental/multi_predicate_srl/learner.h"

namespace ZuoPar {
namespace Experimental {
namespace MultiPredicateSRL {

namespace eg = ZuoPar::Engine;
namespace fe = ZuoPar::FrontEnd;

class Pipe : fe::CommonPipeConfigure {
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

  ~Pipe();

  /**
   * Perform learning or testing according to the configuration.
   */
  void run();

protected:
  bool setup();

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path);

  /**
   * Save the model to file in specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool save_model(const std::string& model_path);

  /**
   * Build the dependency output for the state chain which ends with the source
   * state.
   *
   *  @param[in]  source  The last state in state chain.
   *  @param[out] output  The output dependency parse.
   */
  void build_output(const State& source, SemanticChunks& output);

  //! Load the word class dictionary.
  bool load_verb_class();

  //! Collect the argument relations.
  void collect_argument_relations();
protected:
  enum OutputFormat {
    kSemanticChunks,
    kCoNLL2005
  };

  OutputFormat output_format;

  int cube_height;

  //! The path to the verb class dictionary.
  std::string verb_class_path;

  //! The tag of the predicate.
  std::string predicate_tag;

  //! The prefix of the argument.
  std::string argument_prefix;

  //! The verb class
  std::unordered_map<form_t, int> verb_classes;

  //! The pointer to the weights instances which is pointwise averaged
  //! perceptron model.
  Weight* weight;

  //! The parameter learner.
  Learner* learner;

  //! The pointer to the decoder.
  Decoder* decoder;

  //! The alphabet of attributes.
  eg::TokenAlphabet forms_alphabet;

  //! The alphabet of postags.
  eg::TokenAlphabet postags_alphabet;

  //! The alphabet of senses.
  eg::TokenAlphabet senses_alphabet;

  //! The alphabet of semantic chunks.
  eg::TokenAlphabet tags_alphabet;

  //! The dataset.
  std::vector<SemanticChunks> dataset;
};

} //  namespace multipredicatesrl
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_MULTI_PREDICATE_SRL_PIPE_H__
