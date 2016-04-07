#ifndef __ZUOPAR_SYSTEM_CONFIG_H__
#define __ZUOPAR_SYSTEM_CONFIG_H__

namespace ZuoPar {

enum LearningAlgorithm {
  kPassiveAggressive = 0,
  kAveragePerceptron
};

enum UpdateStrategy {
  kNaive = 0,
  kEarlyUpdate,
  kMaxViolation
};

static LearningAlgorithm get_algorithm(const std::string& flag) {
  if (flag == "ap") {
    return LearningAlgorithm::kAveragePerceptron; 
  } else {
    return LearningAlgorithm::kPassiveAggressive;
  }
}

static UpdateStrategy get_update_strategy(const std::string& flag) {
  if (flag == "early") {
    return UpdateStrategy::kEarlyUpdate;
  } else if (flag == "max") {
    return UpdateStrategy::kMaxViolation;
  } else {
    return UpdateStrategy::kNaive;
  }
}

} //  namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_CONFIG_H__
