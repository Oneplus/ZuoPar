#ifndef __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
#define __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__

#include <iostream>
#include "experimental/hc_search/evaluate.h"
#include "app/depparser/utils.h"
#include "app/depparser/arceager/pipe.h"

namespace ZuoPar {
namespace Experimental {
namespace HCSearchDependencyParser {
namespace HStep {

class Pipe: public DependencyParser::ArcEager::Pipe {

public:
  //! Build pipe for training first type data.
  Pipe(const boost::program_options::variables_map& vm);

  //! The deallocator
  ~Pipe();

  void learn();
  void prepare();
  void evaluate();

private:
  void evaluate(const std::vector<CoNLLXDependency>& ds, EvaluationStatistics& stat);
  std::string language;

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

#endif  //  end for __ZUOPAR_EXPERIMENTAL_HCSEARCH_ARCEAGER_PIPE_H__
