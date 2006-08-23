/*
 *  cTools.h
 *  Avida
 *
 *  Called "tools.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cTools_h
#define cTools_h

/*
 *   Filesystem tools...
 */

class cString;

class cTools
{
private:
  cTools(); // @not_implemented

public:
  static bool MkDir(const cString& dirname, bool verbose=false);
};


#ifdef ENABLE_UNIT_TESTS
namespace nTools {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
