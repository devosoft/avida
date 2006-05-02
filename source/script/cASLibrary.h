/*
 *  cASLibrary.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cASLibrary_h
#define cASLibrary_h

#ifndef cASSymbol_h
#include "cASSymbol.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif


class cASLibrary
{
private:
  tDictionary<cASSymbol> m_symtbl;

public:
  cASLibrary() { ; }


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
