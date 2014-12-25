#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_OPT_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_OPT_H__

#include <iostream>

namespace ZuoPar {
namespace SequenceLabeler {

struct Option {
  std::string model_path; //! The path to the model file.
  int beam_size;          //! The size of the beam.
  int display_interval;   //! The interval for displaying log.
};

struct LearnOption: public Option {
  std::string reference_path; //! The path to the reference file.
  std::string algorithm;      //! The algorithm.
  bool early_update;
};

struct MultiLearnOption: public LearnOption {
  int batch_size;   //! The size of batch.
  int num_threads;  //! The number of threads.
};

struct TestOption: public Option {
  std::string input_path;     //! The path to the input file.
  std::string output_path;    //! The path to the output file.
};

} //  namespace seqlabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_OPT_H__
