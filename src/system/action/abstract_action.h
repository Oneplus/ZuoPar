#ifndef __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
#define __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "types/common.h"

namespace ZuoPar {

class AbstractAction {
public:
  AbstractAction()
    : action_name(0), deprel(0), seed(0) {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  AbstractAction(int name, deprel_t rel)
    : action_name(name), deprel(rel), seed(0) {
    boost::hash_combine(seed, name);
    boost::hash_combine(seed, rel);
  }

  AbstractAction& operator = (const AbstractAction& a) {
    seed = a.seed;
    action_name = a.action_name;
    deprel = a.deprel;
    return (*this);
  }

  bool operator == (const AbstractAction& a) const {
    return (a.action_name == action_name && a.deprel == deprel);
  }

  bool operator != (const AbstractAction& a) const {
    return !((*this) == a);
  }

  //! For boost serialization
  friend class boost::serialization::access;

  //! For serialization
  template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
    ar & seed & action_name & deprel;
  }

  //! For boost hash map.
  friend std::size_t hash_value(const AbstractAction& a) {
    return a.seed;
  }

protected:
  //! The action name.
  int action_name;

  //! The dependency relation.
  deprel_t deprel;

  //! The seed for hashing.
  std::size_t seed;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
