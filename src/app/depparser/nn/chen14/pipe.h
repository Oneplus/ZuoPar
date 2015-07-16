#ifndef __ZUOPAR_DEPPARSER_NN_PIPE_H__
#define __ZUOPAR_DEPPARSER_NN_PIPE_H__

#include "types/dependency.h"
#include "engine/token_alphabet.h"
#include "app/depparser/nn/decoder.h"
#include "app/depparser/nn/dataset.h"
#include "app/depparser/nn/chen14/opt.h"
#include "app/depparser/nn/chen14/classifier.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Chen2014 {

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
  std::string cluster_file;   //! The path to the cluster file, actived in use_cluster.
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
  int kNilDistance;
  int kNilValency;
  int kNilCluster4;
  int kNilCluster6;
  int kNilCluster;

  int kFormInFeaturespace;
  int kPostagInFeaturespace;
  int kDeprelInFeaturespace;
  int kDistanceInFeaturespace;
  int kValencyInFeaturespace;
  int kCluster4InFeaturespace;
  int kCluster6InFeaturespace;
  int kClusterInFeaturespace;
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
  std::unordered_map<int, int> form_to_cluster4;
  std::unordered_map<int, int> form_to_cluster6;
  std::unordered_map<int, int> form_to_cluster;

  // The alphabet
  Engine::TokenAlphabet forms_alphabet;
  Engine::TokenAlphabet lemmas_alphabet;
  Engine::TokenAlphabet cpostags_alphabet;
  Engine::TokenAlphabet postags_alphabet;
  Engine::TokenAlphabet feats_alphabet;
  Engine::TokenAlphabet deprels_alphabet;
  Engine::TokenAlphabet cluster4_types_alphabet;
  Engine::TokenAlphabet cluster6_types_alphabet;
  Engine::TokenAlphabet cluster_types_alphabet;

  // The functor
  NeuralNetworkClassifier classifier;
  Decoder decoder;

  bool use_distance;
  bool use_valency;
  bool use_cluster;

  // The options
  const LearnOption* learn_opt;
  const TestOption*  test_opt;
private:
  void check_dataset(const std::vector<RawCoNLLXDependency>& dataset);
  void display_learning_parameters();
  bool setup();
  void build_alphabet();
  void build_cluster();
  void build_feature_space();
  void initialize_classifier();
  void generate_training_samples();
  void get_features(const State& state, std::vector<int>& features);
  void get_features(const State& state,
      const std::vector<int>& cluster4,
      const std::vector<int>& cluster6,
      const std::vector<int>& cluster,
      std::vector<int>& features);
  void get_context(const State& s, Context& ctx);
  void get_basic_feature(const Context& ctx,
      const std::vector<int>& forms,
      const std::vector<int>& postags,
      const int* deprels,
      std::vector<int>& features);
  void get_distance_features(const Context& ctx,
      std::vector<int>& features);
  void get_valency_features(const Context& ctx,
      const int* nr_left_children,
      const int* nr_right_children,
      std::vector<int>& features);
  void get_cluster_features(const Context& ctx,
      const std::vector<int>& cluster4,
      const std::vector<int>& cluster6,
      const std::vector<int>& cluster,
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
      CoNLLXDependency& output, bool with_reference);
  void get_cluster_from_dependency(const CoNLLXDependency& data,
      std::vector<int>& cluster4, std::vector<int>& cluster6, std::vector<int>& cluster);
};

}
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_DEPPARSER_NN_PIPE_H__
