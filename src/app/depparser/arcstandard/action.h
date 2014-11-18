#ifndef __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__
#define __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__

#include <iostream>
#include <boost/assert.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/functional/hash.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/singleton.hpp>
#include "types/common.h"

namespace ZuoPar {
namespace DependencyParser {
namespace ArcStandard {

class Action {
public:
  enum {
    kNone = 0,  //! Placeholder for illegal action.
    kShift,     //! The index of shift action.
    kLeftArc,   //! The index of arc left action.
    kRightArc   //! The index of arc right action.
  };

  Action()
    : action_name(kNone), deprel(0), seed(0) {}

  /**
   * Constructor for action.
   *
   *  @param[in]  name  The name for the action.
   *  @param[in]  rel   The dependency relation.
   */
  Action(int name, deprel_t rel)
    : action_name(name),
    deprel(rel) {
    seed = 0;
    boost::hash_combine(seed, name);
    boost::hash_combine(seed, rel);
  }

  Action& operator = (const Action& a) {
    seed = a.seed;
    action_name = a.action_name;
    deprel = a.deprel;
    return (*this);
  }

  bool operator == (const Action& a) const {
    return (a.seed == seed);
  }

  bool operator != (const Action& a) const {
    return !((*this) == a);
  }

  //! For boost serialization
  friend class boost::serialization::access;

  template<class Archive>
    void serialize(Archive & ar, const unsigned int /* file_version */) {
    ar & seed & action_name & deprel;
  }

  //! For boost hash map.
  friend std::size_t hash_value(const Action& a) {
    return a.seed;
  }

  //! Overload the ostream function.
  friend std::ostream& operator<<(std::ostream& os, const Action& act) {
    if (act.action_name == kShift) {
      os << "SH(" << act.seed << ")";
    } else if (act.action_name == kLeftArc) {
      os << "LA-" << act.deprel << "(" << act.seed << ")";
    } else if (act.action_name == kRightArc) {
      os << "RA-" << act.deprel << "(" << act.seed << ")";
    } else {
      BOOST_ASSERT_MSG(false, "unknown actions.");
    }

    return os;
  }

  //! For is_shift, is_left_arc, is_right_arc;
  friend class ActionUtils;
private:
  //! The action name.
  int action_name;

  //! The dependency relation.
  deprel_t deprel;

  //! The seed for hashing.
  size_t seed;
};

class ActionFactory {
public:
  /**
   * Make a shift action.
   *
   *  @return Action  A shift action.
   */
  static Action make_shift();

  /**
   * Make a arc left action.
   *
   *  @param[in]  rel     The dependency relation.
   *  @return     Action  The arc left action.
   */
  static Action make_left_arc(const deprel_t& rel);

  /**
   * Make a arc right action.
   *
   *  @param[in]  rel     The dependency relation.
   *  @return     Action  The arc right action.
   */
  static Action make_right_arc(const deprel_t& rel);
};

} //  end for namespace arcstandard
} //  end for namespace dependencyparser
} //  end for namespace zuopar

#endif  //  end for __ZUOPAR_APP_DEPPARSER_ARCSTANDARD_ACTION_H__
