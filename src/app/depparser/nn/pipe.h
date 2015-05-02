#ifndef __ZUOPAR_DEPPARSER_NN_PIPE_H__
#define __ZUOPAR_DEPPARSER_NN_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/nn/opt.h"
#include "app/depparser/nn/classifier.h"
#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/dataset.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

class Pipe {
public:
  enum PipeMode { kPipeLearn, kPipeTest };
  Pipe(const LearnOption& opts);
  Pipe(const TestOption& opts);

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
  //
  int kNilForm;
  int kNilPostag;
  int kNilDeprel;
  int kFormInFeaturespace;
  int kPostagInFeaturespace;
  int kDeprelInFeaturespace;

  // The dataset
  std::vector<RawCoNLLXDependency> train_dataset;  //! The training dataset.
  std::vector<RawCoNLLXDependency> devel_dataset;  //! The development dataset.
  std::vector<RawCoNLLXDependency> test_dataset;   //! The test dataset.

  // The internal data
  Dataset dataset;
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
  const LearnOption* learn_opt;
  const TestOption*  test_opt;
private:
  void check_dataset(const std::vector<RawCoNLLXDependency>& dataset);
  void display_learning_parameters();
  bool setup();
  void build_alphabet();
  void initialize_classifier();
  void generate_training_samples();
  void get_features(const State& state, std::vector<int>& features);
  void predict(const RawCoNLLXDependency& data, std::vector<int>& heads,
      std::vector<std::string>& deprels);
  void load_model(const std::string& model_path);
  void save_model(const std::string& model_path);
};

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_DEPPARSER_NN_PIPE_H__
