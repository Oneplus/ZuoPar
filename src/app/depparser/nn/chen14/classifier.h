#ifndef __ZUOPAR_DEPPARSER_NN_CHEN14_CLASSIFIER_H__
#define __ZUOPAR_DEPPARSER_NN_CHEN14_CLASSIFIER_H__

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
#include "app/depparser/nn/chen14/opt.h"

namespace ZuoPar {
namespace DependencyParser {
namespace NeuralNetwork {
namespace Chen2014 {

class NeuralNetworkClassifier {
private:
  struct CostAndGradient {
    arma::mat grad_E;
    arma::mat grad_W1;
    arma::vec grad_b1;
    arma::mat grad_W2;
    arma::mat grad_saved; // Mat: encoder.size() X hidden_layer_size

    floatval_t loss;
    floatval_t accuracy;

    void init(
        int E_n_rows, int E_n_cols,
        int W1_n_rows, int W1_n_cols,
        int b1_n_rows,
        int W2_n_rows, int W2_n_cols,
        int saved_n_rows, int saved_n_cols) {
      grad_E.zeros(E_n_rows, E_n_cols);
      grad_W1.zeros(W1_n_rows, W1_n_cols);
      grad_b1.zeros(b1_n_rows);
      grad_W2.zeros(W2_n_rows, W2_n_cols);
      grad_saved.zeros(saved_n_rows, saved_n_cols);

      loss = 0;
      accuracy = 0;
    }

    void zeros() {
      grad_E.zeros();
      grad_W1.zeros();
      grad_W2.zeros();
      grad_b1.zeros();
      grad_saved.zeros();

      loss = 0;
      accuracy = 0;
    }

    void merge(const CostAndGradient& other) {
      grad_E += other.grad_E;
      grad_W1 += other.grad_W1;
      grad_b1 += other.grad_b1;
      grad_W2 += other.grad_W2;
      grad_saved += other.grad_saved;

      loss += other.loss;
      accuracy += other.accuracy;
    }
  };

private:
  // The weight group.
  arma::mat E;          // Mat: nr_objects X embedding_size
  arma::mat W1;         // Mat: hidden_layer_size X (nr_feature_types * embedding_size)
  arma::vec b1;         // Vec: hidden_layer_size
  arma::mat W2;         // Mat: nr_classes X hidden_layer_size
  arma::mat saved;      // Mat: encoder.size() X hidden_layer_size

  arma::mat eg2E;
  arma::mat eg2W1;
  arma::vec eg2b1;
  arma::mat eg2W2;

  CostAndGradient cost;

  // The configuration
  size_t embedding_size;      //! The size of the embedding.
  size_t hidden_layer_size;   //! The size of the hidden layer
  size_t nr_objects;          //! The sum of forms, postags and deprels
  size_t nr_feature_types;    //! The number of feature types
  size_t nr_classes;          //! The number of classes

  size_t iter;
  size_t nr_threads;
  bool fix_embeddings;

  floatval_t dropout_probability;
  floatval_t lambda;

  // adagrad parameters
  floatval_t ada_eps;
  floatval_t ada_lr;    //! The alpha in adagrad

  std::vector< arma::uvec > dropout_histories;
  std::unordered_map<int, size_t> precomputation_id_encoder;

  bool initialized;
  size_t batch_size;
  std::vector<CostAndGradient> costs;

  struct job_t {
    const Sample* first;
    int second;

    job_t(): first(NULL), second(-1) {}
    job_t(const Sample* _x, int _y): first(_x), second(_y) {}
  };

  boost::lockfree::queue< job_t > queue;
  boost::lockfree::queue< int > queue2;
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
  void info(bool detailed);

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
  //! Initialize the gradient histories
  void initialize_gradient_histories();

  /**
   * 
   */
  void compute_cost_and_gradient_one_thread(CostAndGradient* result);

  void back_propagate_saved_one_thread();

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

} //  namespace chen2014
} //  namespace neuralnetwork
} //  namespace dependencyparser
} //  namespace zuopar

#endif  //  end for __ZUOPAR_DEPPARSER_NN_CLASSIFIER_H__
