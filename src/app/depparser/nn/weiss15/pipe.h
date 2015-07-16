#ifndef __ZUOPAR_NN_WEISS2015_PIPE_H__
#define __ZUOPAR_NN_WEISS2015_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/nn/extractor.h"
#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/dataset.h"
#include "app/depparser/nn/weiss15/opt.h"
#include "app/depparser/nn/weiss15/classifier.h"
#include "app/depparser/nn/weiss15/search.h"
#include "app/depparser/nn/weiss15/model.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Weiss2015 {

class Pipe {
public:
  enum PipeMode { kPipePretrain, kPipePretest, kPipeLearn, kPipeTest };
  Pipe(const PretrainOption& opts);
  Pipe(const PretestOption& opts);
  Pipe(const LearnOption& opts);
  Pipe(const TestOption& opts);

  void pretrain();
  void pretest();
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
  Engine::TokenAlphabet postags_alphabet;
  Engine::TokenAlphabet deprels_alphabet;

  // The functor
  NeuralNetworkClassifier classifier;
  Decoder decoder;
  Extractor extractor;
  Model model;
  Learner * learner;
  StructuredDecoder* search;

  // The options
  const PretrainOption* pretrain_opt;
  const PretestOption* pretest_opt;
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

  void predict(const RawCoNLLXDependency& data, std::vector<int>& heads,
      std::vector<std::string>& deprels);

  void load_model(const std::string& model_path);

  void save_model(const std::string& model_path);

  void load_structured_model(const std::string& model_path);

  void save_structured_model(const std::string& model_path);

  void info();

  std::pair<
    std::vector<Sample>::const_iterator,
    std::vector<Sample>::const_iterator
  > generate_training_samples_one_batch();

  void transduce_instance_to_dependency(const RawCoNLLXDependency& data,
      CoNLLXDependency& output, bool with_reference);
};

} //  namespace weiss2015
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_NN_WEISS2015_PIPE_H__
