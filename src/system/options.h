#ifndef __ZUOPAR_SYSTEM_CONFIG_H__
#define __ZUOPAR_SYSTEM_CONFIG_H__

namespace ZuoPar {

enum LearningAlgorithm {
  kPassiveAgressive = 0,
  kAveragePerceptron
};

enum UpdateStrategy {
  kNaive = 0,
  kEarlyUpdate,
  kMaxViolation
};

} //  namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_CONFIG_H__
