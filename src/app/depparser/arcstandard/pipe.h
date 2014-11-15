#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/arcstandard/opt.h"
#include "app/depparser/arcstandard/weight.h"
#include "app/depparser/arcstandard/decoder.h"

namespace eg = ZuoPar::Engine;

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class Pipe {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts    The learning options.
   */
  Pipe(const LearnOption& opts);

  /**
   *
   *
   */
  Pipe(const TestOption& opts);

  /**
   *
   *
   *
   */
  void run();

private:
  //! Use to specify if perform training.
  bool learning;

  //!
  std::string reference_path;

  //!
  int beam_size;

  //! The pointer to the weights instances which is pointwise averaged
  //! perceptron model.
  Weight* weight;

  //! The pointer to the decoder.
  Decoder* decoder;

  //!
  eg::TokenAlphabet forms_alphabet;
  //!
  eg::TokenAlphabet postags_alphabet;
  //!
  eg::TokenAlphabet deprels_alphabet;

  //!
  std::vector<Dependency> dataset;
};


} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_PIPE_H__
