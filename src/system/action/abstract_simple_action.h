#ifndef __ZUOPAR_SYSTEM_ACTION_ABSTRACT_SIMPLE_ACTION_H__
#define __ZUOPAR_SYSTEM_ACTION_ABSTRACT_SIMPLE_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>
#include "types/common.h"

namespace ZuoPar {

class AbstractSimpleAction {
public:
  // SimpleAction is designed for sequence labeling, in which the action is relative
  // simple.
  AbstractSimpleAction(): seed(0) {}

  /**
   * Constructor for simple action.
   *
   *  @param[in]  name  The name for the action.
   */
  AbstractSimpleAction(int name) : seed(name) {}

  bool operator == (const AbstractSimpleAction& a) const { return (a.seed== seed); }
  bool operator != (const AbstractSimpleAction& a) const { return (a.seed!= seed); }
  bool operator <  (const AbstractSimpleAction& a) const { return (seed < a.seed); }

  inline int name() const { return seed; }
  //! For serialization
  template<class Archive> void serialize(Archive & ar, const unsigned) { ar & seed; }
  //! For hash
  friend std::size_t hash_value(const AbstractSimpleAction& a) { return a.seed; }
protected:
  //! The action name.
  int seed;
  friend class boost::serialization::access;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
