#ifndef __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
#define __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>
#include "types/common.h"

namespace ZuoPar {

class AbstractAction {
public:
  AbstractAction(): name(0), rel(0), seed(0) {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  AbstractAction(int _name, int _rel): name(_name), rel(_rel), seed(0) {
    boost::hash_combine(seed, _name);
    boost::hash_combine(seed, _rel);
  }

  bool operator == (const AbstractAction& a) const { return (rel == a.rel) && (name == a.name); }
  bool operator != (const AbstractAction& a) const { return (!(*this) == a); }
  bool operator <  (const AbstractAction& a) const {
    return (name == a.name ? rel < a.rel: name < a.name); }

  //! For boost serialization
  friend class boost::serialization::access;

  //! For serialization
  template<class Archive> void serialize(Archive & ar, const unsigned) { ar & seed & name & rel; }

  //! For boost hash map.
  friend std::size_t hash_value(const AbstractAction& a) {  return a.seed;  }

protected:
  //! The action name.
  int name;
  //! The dependency relation.
  int rel;
  //! The seed for hashing.
  std::size_t seed;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
