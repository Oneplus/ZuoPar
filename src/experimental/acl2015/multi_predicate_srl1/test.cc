#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

class ActionCollection: public std::vector<int> {
public:
  std::size_t seed;
  ActionCollection(): seed(0) {}

  void push_back(const int& val) {
    boost::hash_combine(seed, val);
    std::vector<int>::push_back(val);
  }
};

int main() {
  ActionCollection collection;
  collection.push_back(1);
  collection.push_back(2);
  collection.push_back(3);
  std::cout << collection.seed << std::endl;
  for (int i = 0; i < collection.size(); ++ i) { std::cout << collection[i] << std::endl; }

  ActionCollection collection2(collection);
  std::cout << collection2.seed << std::endl;
  for (int i = 0; i < collection2.size(); ++ i) { std::cout << collection2[i] << std::endl; }

  ActionCollection collection3 = collection;
  std::cout << collection3.seed << std::endl;
  for (int i = 0; i < collection3.size(); ++ i) { std::cout << collection3[i] << std::endl; }

  return 0;
}
