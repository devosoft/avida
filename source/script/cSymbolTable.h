/*
 *  cSymbolTable.h
 *  Avida
 *
 *  Created by David on 2/2/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cSymbolTable_h
#define cSymbolTable_h

#ifndef cASSymbol_h
#include "cASSymbol.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif


class cSymbolTable
{
private:
  tDictionary<cASSymbol> m_symbols;
  
public:
  cSymbolTable() { ; }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
