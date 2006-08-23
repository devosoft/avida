/*
 *  cCycleCheck.h
 *  Avida
 *
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
 *
 */

#ifndef cCycleCheck_h
#define cCycleCheck_h

class cID;

/*!
\brief Internal class; used to prevent cycles in recursive operations on collections of classes or objects.

\remarks
This class is used internally to prevent cycles in recursive operations on collections of classes or objects; it is probably
not well-suited for any other use.

This class is (currently only) used in recursive unit-testing of classes and recursive verification of objects, to detect and
prevent cycles in the recursion.  Such classes should each have a unique identifying string, and such objects should each have
a cID member variable. An instance of this class is carried through the process of recursive unit-testing, and as each class
is encountered, the ckStr() member function is called with that class' identifying string; if that string has not been seen in
the recursion then ckStr() returns \e true (and the class is unit-tested), and otherwise returns \e false.  Similarly in
recursive object-verification, each object's cID is checked by the ckID() function.

This class also indicates to the testing code whether a unit-test or object-verification should be recursive; in that case
recurse() returns \e true, and otherwise returns \e false.

For more information about defining recursive unit-test suites and object-verification suites, see \ref recursiveTesting

\ingroup recursiveTesting
*/
class cCycleCheck {
private:
  struct cCycleCheckPrv;
private:
  cCycleCheck(); // disabled.
private:
  cCycleCheckPrv *m_cyck_prv;
  bool m_recurse;
public:
  /*!\brief Returns \e true if ckStr() has not been called with string \e str as an argument. */
  bool ckStr(const char *str);
  /*!\brief Returns \e true if ckId() has not been called with identifier object \e id as an argument. */
  bool ckID(const cID &id);
  /*!\brief Returns \e true if this unit-test or object-verification should be recursive. */
  bool recurse(){ return m_recurse; }
public:
  /*!
  \brief Constructs an object to detect cycles in recursive unit-testing and object verification.

  If \e recurse is \e true then the unit-test of a class will recurse into unit-tests of classes on which it depends, or the
  object verification of an object will recurse into verification of its sub-objects.
  */
  cCycleCheck(bool recurse);
  /*!
  \brief Destroys the cycle-checking object.

  Frees the list of classes encountered in the recursive unit-test, or the ids of objects encountered in the recursive
  verification.
  */
  ~cCycleCheck();
};

  
#ifdef ENABLE_UNIT_TESTS
namespace nCycleCheck {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
