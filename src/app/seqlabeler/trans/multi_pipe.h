#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_MULTI_PIPE_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_MULTI_PIPE_H__

#include <boost/thread/mutex.hpp>
#include <boost/lockfree/queue.hpp>
#include "frontend/common_opt.h"
#include "app/seqlabeler/trans/pipe.h"

namespace ZuoPar {
namespace SequenceLabeler {

namespace eg = ZuoPar::Engine;

class MultiPipe : public Pipe {
public:
  /**
   * The learning mode constructor.
   *
   *  @param[in]  opts  The learning options.
   */
  MultiPipe(const boost::program_options::variables_map& vm);

  //!
  void multi_learn();
private:
  //!
  void decode();

  //!
  void decode_standalone(Decoder* decoder, const SequenceInstance* data);

  //!
  Decoder* get_decoder();

  //! The size of mini batch.
  int batch_size;

  //! The number of threads in.
  int num_threads;

  //!
  int last_free_decoder_id;

  //! The decoders
  std::vector<Decoder *> decoder_pool;

  //! The learner
  MinibatchLearner* minibatch_learner;

  //!
  boost::lockfree::queue<const SequenceInstance*> queue;

  //!
  boost::mutex mtx;
};

} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_MULTI_PIPE_H__
