#ifndef __ZUOPAR_SYSTEM_ACTION_ABSTRACT_SIMPLE_ACTION_H__
#define __ZUOPAR_SYSTEM_ACTION_ABSTRACT_SIMPLE_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include "types/common.h"

namespace ZuoPar {

class AbstractSimpleAction {
public:
  AbstractSimpleAction(): action_name(0) {}

  /**
   * Constructor for simple action.
   *
   *  @param[in]  name  The name for the action.
   */
  AbstractSimpleAction(int name) : action_name(name) {}

  AbstractSimpleAction& operator = (const AbstractSimpleAction& a) {
    action_name = a.action_name;
    return (*this);
  }

  bool operator == (const AbstractSimpleAction& a) const {
    return (a.action_name== action_name);
  }

  bool operator != (const AbstractSimpleAction& a) const {
    return !((*this) == a);
  }

  //! For boost serialization
  friend class boost::serialization::access;

  //! For serialization
  template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
    ar & action_name;
  }

  //! For boost hash map.
  friend std::size_t hash_value(const AbstractSimpleAction& a) {
    return a.action_name;
  }

protected:
  //! The action name.
  int action_name;
};

} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_SYSTEM_ACTION_ABSTRACT_ACTION_H__
