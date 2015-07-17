#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCSTANDARD_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCSTANDARD_PIPE_H__

#include <iostream>
#include "experimental/hc_search/hstep_opt.h"
#include "app/depparser/utils.h"
#include "app/depparser/arcstandard/pipe.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

class Pipe: public DependencyParser::ArcStandard::Pipe {

public:
  //! Build pipe for training first type data.
  Pipe(const LearnOption& opts);

  //! Build pipe for preparing data for phase2.
  Pipe(const PrepareOption& opts);

  //! Build pipe for evaluation, on the UAS/LAS upperbound, averaged UAS/LAS
  Pipe(const EvaluateOption& opts);

  //! The deallocator
  ~Pipe();

  void run2();

private:
  std::string language;

  enum PipeMode { kPipeLearn, kPipePrepare, kPipeEvaluate };
  PipeMode mode;
  NegtiveSampleStrategy neg_sample_strategy;
  DependencyParser::DependencyParserUtils::EvaluationStrategy evaluate_strategy;

private:
  int wrong(const CoNLLXDependency& instance, bool labeled,
      const std::vector<int>& heads,
      const std::vector<int>& deprels,
      int& n_effective_words) const;
};

} //  namespace hstep
} //  namespace hcsearchdependencyparser
} //  namespace experimental
} //  namespace zuopar

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCSTANDARD_PIPE_H__
