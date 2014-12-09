#include <iostream>
#include <cstring>  // strcmp
#include "opt.h"
#include "app/depparser/arcstandard/learn.h"
#include "app/depparser/arcstandard/test.h"
#include "utils/logging.h"

/**
 * Perform the learning process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running learning.
 */
int learn(int argc, char** argv);

/**
 * Perform the testing process of ZuoPar::arcstandard dependency parser.
 *
 *  @param[in]  argc  The number of arguments.
 *  @param[in]  argv  The arguments.
 *  @return     int   The status of running test.
 */
int test(int argc, char** argv);

int main(int argc, char** argv) {
  std::string usage = "ZuoPar::arcstandard dependency parser.\n";
  usage += "Author: Yijia Liu (oneplus.lau@gmail.com).\n\n";
  usage += "Usage: arcstandard_depparser [learn|test] [options]";

  if (argc == 1) {
    std::cerr << usage << std::endl;
    return 1;
  } else if (strcmp(argv[1], "learn") == 0) {
    learn(argc- 1, argv+ 1);
  } else if (strcmp(argv[1], "test") == 0) {
    test(argc- 1, argv+ 1);
  } else {
    std::cerr << "Unknown mode: " << argv[1] << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }

  return 0;
}
