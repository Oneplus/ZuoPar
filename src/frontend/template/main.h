#ifndef __ZUOPAR_FRONTEND_MAIN_H__
#define __ZUOPAR_FRONTEND_MAIN_H__

#define MAIN(USAGE, EXE)                                          \
int main(int argc, char** argv) {                                 \
  std::string usage = "ZuoPar::";                                 \
  usage += USAGE;                                                 \
  usage += ".\n";                                                 \
  usage += "Usage: ";                                             \
  usage += EXE;                                                   \
  usage += " [learn|multi-learn|test] [options]";                 \
                                                                  \
  if (argc == 1) {                                                \
    std::cerr << usage << std::endl;                              \
    return 1;                                                     \
  } else if (strcmp(argv[1], "learn") == 0) {                     \
    learn(argc- 1, argv+ 1);                                      \
  } else if (strcmp(argv[1], "multi-learn") == 0) {               \
    multi_learn(argc- 1, argv+ 1);                                \
  } else if (strcmp(argv[1], "test") == 0) {                      \
    test(argc- 1, argv+ 1);                                       \
  } else {                                                        \
    std::cerr << "Unknown mode: " << argv[1] << std::endl;        \
    std::cerr << usage << std::endl;                              \
    return 1;                                                     \
  }                                                               \
                                                                  \
  return 0;                                                       \
}

#endif  //  end for __ZUOPAR_FRONTEND_MAIN_H__
