#ifndef __ZUOPAR_SYSTEM_ACTION_ABSTRACT_INEXACT_ACTION_H__
#define __ZUOPAR_SYSTEM_ACTION_ABSTRACT_INEXACT_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <boost/serialization/access.hpp>
#include "types/common.h"

namespace ZuoPar {

class AbstractInexactAction {
public:
  AbstractInexactAction(): seed(0) {}

  /**
   * Constructor for inexact action. Empirically, the number of name
   * is less than 32. So such inexact action type compile the action
   * name and action type into a single integer.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  AbstractInexactAction(int name, tag_t rel): seed(rel << 6 | name) {}

  bool operator == (const AbstractInexactAction& a) const { return (a.seed == seed); }
  bool operator != (const AbstractInexactAction& a) const { return (a.seed != seed); }
  bool operator <  (const AbstractInexactAction& a) const { return (seed < a.seed);  }

  inline int name() const { return (seed & 0x3f);   }
  inline int rel()  const { return (seed >> 6); }

  //! For boost serialization
  friend class boost::serialization::access;

  //! For serialization
  template<class Archive> void serialize(Archive & ar, const unsigned) { ar & seed; }

  //! For boost hash map.
  friend std::size_t hash_value(const AbstractInexactAction& a) {  return a.seed; }

protected:
  //! The seed for hashing.
  std::size_t seed;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_ACTION_ABSTRACT_INEXACT_ACTION_H__
