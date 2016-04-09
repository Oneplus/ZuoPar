#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_PIPE_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_PIPE_H__

#include "types/sequence_instance.h"
#include "engine/token_alphabet.h"
#include "frontend/common_pipe_cfg.h"
#include "app/seqlabeler/trans/opt.h"
#include "app/seqlabeler/trans/weight.h"
#include "app/seqlabeler/trans/decoder.h"
#include "app/seqlabeler/trans/learner.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;

class Pipe {
public:
  Pipe(const boost::program_options::variables_map& vm);

  void learn(); //  perform learning
  void test();  //  perform testing

protected:
  double evaluate(const std::vector<SequenceInstance>& dataset);
  bool load_training_data();
  bool load_test_data();
  void load_constrain();

  /**
   * Load model from the specified path.
   *
   *  @param[in]  model_path  The path to the model.
   */
  bool load_model(const std::string& model_path);

  bool save_model(const std::string& model_path);

  /**
   * Build the tag result for the state chain which ends with the source state.
   *
   *  @param[in]  source  The last state in state chain.
   *  @param[out] output  The output sequence instance.
   */
  void build_output(const State& source, SequenceInstance& output);

protected:
  //! Matrix for legal transition.
  std::vector< std::vector<bool> > trans;

  //! The pointer to the weights instances which is pointwise averaged
  //! perceptron model.
  Weight* weight;

  //! The parameter learner.
  Learner* learner;

  //! The pointer to the decoder.
  Decoder* decoder;

  //! The alphabets of attributes.
  eg::TokenAlphabet attributes_alphabet;

  //! The alphabets of postags.
  eg::TokenAlphabet tags_alphabet;

  //! The dataset.
  std::vector<SequenceInstance> dataset;
  std::vector<SequenceInstance> devel_dataset;

  //! The configuration
  const boost::program_options::variables_map& conf;
};


} //  end for namespace sequencelabeler
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_PIPE_H__
