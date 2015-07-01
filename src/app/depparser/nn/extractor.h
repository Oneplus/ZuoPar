#ifndef __ZUOPAR_APP_DEPPARSER_NN_EXTRACTOR_H__
#define __ZUOPAR_APP_DEPPARSER_NN_EXTRACTOR_H__

#include <vector>
#include "app/depparser/nn/state.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

struct Context {
  int S0, S1, S2, N0, N1, N2;
  int S0L, S0R, S0L2, S0R2, S0LL, S0RR;
  int S1L, S1R, S1L2, S1R2, S1LL, S1RR;
};

class Extractor {
public:
  //
  int kNilForm;
  int kNilPostag;
  int kNilDeprel;

  int kFormInFeaturespace;
  int kPostagInFeaturespace;
  int kDeprelInFeaturespace;
  int kFeatureSpaceEnd;

  bool initialized;
public:
  Extractor();

  void initialize(
      int nil_form, int n_forms,
      int nil_postag, int n_postags,
      int nil_deprel, int n_deprels);

  void display();

  void get_features(const State& state, std::vector<int>& features);

  void get_context(const State& state, Context& ctx);

  void get_basic_feature(const Context& ctx,
      const std::vector<int>& forms,
      const std::vector<int>& postags,
      const int* deprels,
      std::vector<int>& features);

  int n_dim() const;
};

} //  namespace neuralnetwork
} //  namespace depparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_NN_EXTRACTOR_H__
