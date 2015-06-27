#ifndef __ZUOPAR_DEPPARSER_NN_CLASSIFIER_H__
#define __ZUOPAR_DEPPARSER_NN_CLASSIFIER_H__

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <armadillo>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lockfree/queue.hpp>
#include "types/common.h"
#include "app/depparser/nn/dataset.h"
#include "app/depparser/nn/opt.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {

class NeuralNetworkClassifier {
public:
  enum ActivationType {
    kCube,
    kReLU,
  };

private:
  // The weight group.
  arma::mat E;          // Mat: nr_objects X embedding_size
  arma::mat W1;         // Mat: hidden_layer_size X (nr_feature_types * embedding_size)
  arma::mat W10;        // Mat: hidden_layer_size X hidden_layer_size (for ReLu)
  arma::vec b1;         // Vec: hidden_layer_size
  arma::vec b10;        // Vec: hidden_layer_size
  arma::mat W2;         // Mat: nr_classes X hidden_layer_size

  arma::mat grad_E;
  arma::mat grad_W1;
  arma::mat grad_W10;
  arma::vec grad_b1;
  arma::vec grad_b10;
  arma::mat grad_W2;

  arma::mat eg2E;
  arma::mat eg2W1;
  arma::mat eg2W10;
  arma::vec eg2b1;
  arma::vec eg2b10;
  arma::mat eg2W2;

  floatval_t loss;
  floatval_t accuracy;

  // Precomputed matrix
  arma::mat saved;      // Mat: encoder.size() X hidden_layer_size
  arma::mat grad_saved; // Mat: encoder.size() X hidden_layer_size

  // The configuration
  size_t embedding_size;      //! The size of the embedding.
  size_t hidden_layer_size;   //! The size of the hidden layer
  size_t nr_objects;          //! The sum of forms, postags and deprels
  size_t nr_feature_types;    //! The number of feature types
  size_t nr_classes;          //! The number of classes

  size_t batch_size;
  size_t nr_threads;
  size_t iter;
  bool fix_embeddings;

  floatval_t dropout_probability;
  floatval_t lambda;

  // adagrad parameters
  floatval_t ada_eps;
  floatval_t ada_lr;    //! The alpha in adagrad

  // momentum parameters
  size_t     momentum_stepsize;
  floatval_t momentum_mu;
  floatval_t momentum_lr;
  floatval_t momentum_gamma;

  std::vector< arma::uvec > dropout_histories;
  std::unordered_map<int, size_t> precomputation_id_encoder;

  bool initialized;
  ActivationType activation;
public:
  NeuralNetworkClassifier();

  /**
   * Initialize the neural network
   *
   *  @param[in]  nr_forms        The size of vocabulary.
   *  @param[in]  nr_postags      The size of postags.
   *  @param[in]  nr_labels       The size of label set.
   *  @param[in]  nr_tokens       (?)
   *  @param[in]  embedding_size  The size of embeddings
   *  @param[in]  hidden_size     The size of hidden layer
   */
  void initialize(int nr_objects,
      int nr_classes,
      int nr_feature_types,
      const LearnOption& opt,
      const std::vector< std::vector<floatval_t> >& embeddings,
      const std::vector<int>& precomputed_features
      );

  /**
   * Calculate scores for the given features.
   *
   *  @param[in]  features  The features
   *  @param[out] retval    The calculated score
   */
  void score(const std::vector<int>& attributes, std::vector<floatval_t>& retval);

  /**
   * Show the network structure.
   */
  void info();

  /**
   * Compute the cost and gradient
   *
   *  @param[in]  begin The beginning pointer to the vector<Sample>
   *  @param[in]  end   The ending pointer to the vector<Sample>
   *  @param[in]  check If true, perform gradient check
   */
  void compute_cost_and_gradient(
      std::vector<Sample>::const_iterator begin,
      std::vector<Sample>::const_iterator end,
      bool debug = false
      );

  //! Take the gradient from with adagrad.
  void take_adagrad_step();

  //! Take the gradient from momentum sgd.
  void take_momentum_asgd_step();

  /**
   * Save the classifier to the stream.
   *
   *  @param[out] mfs The output stream.
   */
  void save(std::ofstream& mfs);

  /**
   * Load the classifier from the stream.
   *
   *  @param[in]  mfs The input stream.
   */
  void load(std::ifstream& mfs);

  void precomputing();

  floatval_t get_cost() const;      //! shorthand for cost
  floatval_t get_accuracy() const;  //! shorthand for accuracy
private:
  //!
  void initialize_gradient_histories();

  /**
   * Collect the indices from samples and put them into a set.
   *
   *  @param[in]  samples The 
   *  @param[out] retval  The set of indicies
   */
  void get_precomputed_features(std::vector<Sample>::const_iterator& begin,
      std::vector<Sample>::const_iterator& end,
      std::unordered_set<int>& retval);

  void precomputing(const std::unordered_set<int>& precomputed_indices);

  void compute_cost_and_gradient(std::vector<Sample>::const_iterator& begin,
      std::vector<Sample>::const_iterator& end,
      const std::unordered_set<int>& precomputed_indices,
      bool debug);

  floatval_t compute_cost(std::vector<Sample>::const_iterator& begin,
      std::vector<Sample>::const_iterator& end,
      const std::vector< arma::uvec >& dropout_histories);

  void gradient_check(std::vector<Sample>::const_iterator& begin,
      std::vector<Sample>::const_iterator& end);

  void gradient_check_one_dimension(const floatval_t& num_grad,
      const floatval_t& grad, const std::string& name, int x, int y);
};

} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_DEPPARSER_NN_CLASSIFIER_H__
