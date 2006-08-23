/*
 *  cID.h
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology
 *
 */

#ifndef cID_h
#define cID_h

/*!
\brief Provides a unique identifier for instances of a class.

This class provides unique identifier objects that can be ordered for use as unique keys for elements of heterogeneous
collections, where the types of the elements are unimportant.

Instances of cID are used, for example, to detect cycles in recursive object verification, by keeping a list of cIDs of
objects that have been verified, and which should not be later verified if they are encountered again in the recursion. In
recursive object verification, objects of many types may be encountered, but for the purpose of preventing cycles in the
recursion, the cID of each object is important, not its type.

\note XXX : In the current implementation this is a dataless class; instances are distinguished by their addresses in memory.
I don't know how portable this will be, but at least we have some abstraction to help find another way to order instances if
we need to.
*/
class cID {
public:
  /*!
  \brief Comparison operator, providing well-ordering for any collection of cID instances.

  \note XXX : Don't expect any particular order (although there is \e some well-defined order). The ordering might change if
  the current use of pointers to cID objects for their identification turns out to not work.
  */
  cID() { ; }
  bool operator<(const cID &rhs) const { return this < &rhs; }
  bool operator>(const cID &rhs) const { return this > &rhs; }
  bool operator<=(const cID &rhs) const { return this <= &rhs; }
  bool operator>=(const cID &rhs) const { return this >= &rhs; }
  bool operator==(const cID &rhs) const { return this == &rhs; }
  bool operator!=(const cID &rhs) const { return this != &rhs; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nID {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
