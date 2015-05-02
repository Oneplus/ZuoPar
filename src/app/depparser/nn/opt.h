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

  static int NONEXIST;
};

struct BasicOption {
  std::string model_file;     //! The path to the model.
  std::string embedding_file; //! The path to the embedding.
};

struct AdaOption {
  floatval_t ada_eps;             //! Eps used in AdaGrad
  floatval_t ada_alpha;           //! Alpha used in AdaGrad
  floatval_t lambda;              //! TODO not known.
  floatval_t dropout_probability; //! The probability for dropout.
};

struct NetworkOption {
  int hidden_layer_size;    //! Size for hidden layer.
  int embedding_size;       //! Size for embedding.
};

struct LearnOption:
  public BasicOption,
  public AdaOption,
  public NetworkOption {
  int nr_threads;               //! Number of threads.
  int word_cutoff;              //! The frequency of rare word, word lower than that
                                //! will be cut off.
  int max_iter;                 //! The maximum iteration.
  floatval_t init_range;        //!
  int batch_size;               //! The Size of batch.
  int nr_feature_types;         //! The number of features, including word, POS and label.
  int nr_precomputed;           //! The number of precomputed features
  int evaluation_stops;         //!
  int clear_gradient_per_iter;  //! TODO (?);
  bool save_intermediate;       //! Save model whenever see an improved UAS.
  bool fix_embeddings;          //! Not tune the embedding when learning the parameters
  bool debug;                   //! Perform debug when training.
  std::string reference_file;   //! The path to the reference file.
  std::string devel_file;       //! The path to the devel file.
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
