#ifndef __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_OPT_H__
#define __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_OPT_H__

#include "frontend/common_opt.h"
#include <boost/program_options.hpp>

namespace ZuoPar {
namespace SequenceLabeler {

namespace po = boost::program_options;
namespace fe = ZuoPar::FrontEnd;

struct ConstrainOption {
  std::string constrain_path;
};

struct LearnOption: public fe::LearnOption, public ConstrainOption {
  std::size_t shuffle;
};

struct MultiLearnOption: public LearnOption {
  int batch_size;
  int num_threads;
};

struct TestOption: public fe::TestOption, public ConstrainOption {};

} //  namespace sequencelabeler
} //  namespace zuopar

#endif  //  end for __ZUOPAR_APP_SEQUENCE_LABELER_TRANS_OPT_H__
