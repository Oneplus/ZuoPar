#include "engine/word_alphabet.h"

#include "utils/serialization/unordered_map.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace ZGen {

namespace Engine {

WordAlphabet::WordAlphabet()
  : global_id(0) {
  id2name.clear();
  name2id.clear();

  insert("-NONE-");
  insert("-BEGIN-");
  insert("-END-");
}

WordAlphabet::~WordAlphabet() {
}

int
WordAlphabet::insert(const char * name) {
  std::string key(name);
  map_t::const_iterator itx = name2id.find(key);

  if (itx != name2id.end()) {
    return itx->second;
  }

  id2name.push_back(key);
  name2id[key] = global_id;
  return global_id ++;
}


const char *
WordAlphabet::decode(int id) const {
  if (id < 0 || id >= global_id) {
    return 0;
  }
  return id2name.at(id).c_str();
}


int
WordAlphabet::encode(const char* name) const {
  map_t::const_iterator itx = name2id.find(std::string(name));
  if (itx != name2id.end()) {
    return itx->second;
  }
  return -1;
}


bool
WordAlphabet::save(std::ostream& ofs) const {
  if (!ofs.good()) {
    return false;
  }

  boost::archive::text_oarchive oa(ofs);
  oa << id2name;
  oa << name2id;
  oa << global_id;
  return true;
}

bool
WordAlphabet::load(std::istream& ifs) {
  if (!ifs.good()) {
    return false;
  }

  boost::archive::text_iarchive ia(ifs);
  ia >> id2name;
  ia >> name2id;
  ia >> global_id;
  return true;
}


} //  end for namespace Engine

} //  end for namespace ZGen
