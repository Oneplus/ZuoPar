#ifndef __ZUOPAR_APP_DEPPARSER_OPT_H__
#define __ZUOPAR_APP_DEPPARSER_OPT_H__

#include "types/common.h"
#include "frontend/common_opt.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

namespace fe = ZuoPar::FrontEnd;

struct SpecialOption {
  static std::string UNKNOWN;
  static std::string NIL;
  static std::string ROOT;
};

struct BasicOption {
  std::string model_file;     //! The path to the model.
  std::string root;           //! The root.
};

struct AdaOption {
  floatval_t ada_eps; //! Eps used in AdaGrad
  floatval_t ada_lr;  //! Alpha used in AdaGrad
};

struct MomentumASGDOption {
  int        momentum_stepsize; //!
  floatval_t momentum_mu;       //!
  floatval_t momentum_lr;       //! The learning rate.
  floatval_t momentum_gamma;    //! The decay rate
};

struct NetworkOption {
  int hidden_layer_size;    //! Size for hidden layer.
  int embedding_size;       //! Size for embedding.
  floatval_t lambda;        //! regularizer weight
};

struct FeatureOption {
  bool use_distance;            //! Specify to use distance feature.
  bool use_valency;             //! Specify to use valency feature.
  bool use_cluster;             //! Specify to use cluster feature.
  std::string cluster_file;     //! The path to the cluster file, actived in use-cluster.
};

struct LearnOption:
  public BasicOption,
  public AdaOption,
  public NetworkOption,
  public FeatureOption {
  std::string reference_file;   //! The path to the reference file.
  std::string devel_file;       //! The path to the devel file.
  std::string embedding_file;   //! The path to the embedding.
  std::string algorithm;        //! The learning algorithm.
  std::string activation;       //! The activation function.
  int word_cutoff;              //! The frequency of rare word, word lower than that
                                //! will be cut off.
  int max_iter;                 //! The maximum iteration.
  floatval_t init_range;        //!
  floatval_t dropout_probability; //! The probability for dropout.
  int batch_size;               //! The Size of batch.
  int nr_precomputed;           //! The number of precomputed features
  int evaluation_stops;         //!
  std::string oracle;           //! The oracle type.
  std::string evaluation_method;//! The evaluation method.
  bool save_intermediate;       //! Save model whenever see an improved UAS.
  bool fix_embeddings;          //! Not tune the embedding when learning the parameters
  bool debug;
};

struct TestOption:
  public BasicOption {
  std::string input_file;   //! The path to the input file.
  std::string output_file;  //! The path to the output file.
};

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_OPT_H__
