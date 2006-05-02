/*
 *  cAvidaDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cAvidaDriver_h
#define cAvidaDriver_h

// This class is an abstract base class from which all driver classes
// in Avida descend.  cAvidaDriver objects are friends with cWorld,
// allowing them to register cWorldDriver's with an instance of cWorld.

class cAvidaDriver
{
private:
  cAvidaDriver(const cAvidaDriver&); // @not_implemented
  cAvidaDriver& operator=(const cAvidaDriver&); // @not_implemented

public:
  cAvidaDriver() { ; }
  virtual ~cAvidaDriver() { ; }
  
  virtual void Run() = 0;


#ifdef ENABLE_UNIT_TESTS
public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
#endif  
};

#endif
