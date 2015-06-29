#ifndef __ZUOPAR_NN_WEISS2015_PIPE_H__
#define __ZUOPAR_NN_WEISS2015_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/nn/weiss15/opt.h"
#include "app/depparser/nn/weiss15/classifier.h"
#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/dataset.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

class Pipe {
public:
  enum PipeMode { kPipePretrain, kPipeLearn, kPipeTest };
  Pipe(const PretrainOption& opts);
  Pipe(const LearnOption& opts);
  Pipe(const TestOption& opts);

  void pretrain();
  void learn();
  void test();
private:
  PipeMode mode;  //! The mode for pipe.
  std::string reference_file; //! The path to the reference file.
  std::string devel_file;     //! The path to the development file.
  std::string model_file;     //! The path to the model file.
  std::string embedding_file; //! The path to the embedding file.
  std::string input_file;     //! The path to the input file.
  std::string output_file;    //! The path to the output file.
  std::string root;           //! The root

  struct Context {
    int S0, S1, S2, N0, N1, N2;
    int S0L, S0R, S0L2, S0R2, S0LL, S0RR;
    int S1L, S1R, S1L2, S1R2, S1LL, S1RR;
  };

  //
  int kNilForm;
  int kNilPostag;
  int kNilDeprel;

  int kFormInFeaturespace;
  int kPostagInFeaturespace;
  int kDeprelInFeaturespace;
  int kFeatureSpaceEnd;

  // The dataset
  std::vector<RawCoNLLXDependency> train_dataset;  //! The training dataset.
  std::vector<RawCoNLLXDependency> devel_dataset;  //! The development dataset.
  std::vector<RawCoNLLXDependency> test_dataset;   //! The test dataset.

  // The internal data
  Dataset dataset;
  int cursor;
  int nr_feature_types;
  std::vector<int> precomputed_features;

  // The alphabet
  Engine::TokenAlphabet forms_alphabet;
  Engine::TokenAlphabet lemmas_alphabet;
  Engine::TokenAlphabet cpostags_alphabet;
  Engine::TokenAlphabet postags_alphabet;
  Engine::TokenAlphabet feats_alphabet;
  Engine::TokenAlphabet deprels_alphabet;

  // The functor
  NeuralNetworkClassifier classifier;
  Decoder decoder;

  // The options
  const PretrainOption* pretrain_opt;
  const LearnOption* learn_opt;
  const TestOption*  test_opt;
private:
  void check_dataset(const std::vector<RawCoNLLXDependency>& dataset);

  void display_pretraining_parameters();

  bool setup();

  void build_alphabet();

  void build_feature_space();

  void initialize_classifier();

  void generate_training_samples();

  void get_features(const State& state, std::vector<int>& features);

  void get_context(const State& s, Context& ctx);

  void get_basic_feature(const Context& ctx,
      const std::vector<int>& forms,
      const std::vector<int>& postags,
      const int* deprels,
      std::vector<int>& features);

  void predict(const RawCoNLLXDependency& data, std::vector<int>& heads,
      std::vector<std::string>& deprels);

  void load_model(const std::string& model_path);

  void save_model(const std::string& model_path);

  void info();

  std::pair<
    std::vector<Sample>::const_iterator,
    std::vector<Sample>::const_iterator
  > generate_training_samples_one_batch();

  void transduce_instance_to_dependency(const RawCoNLLXDependency& data,
      Dependency& output, bool with_reference);
};

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_NN_WEISS2015_PIPE_H__
