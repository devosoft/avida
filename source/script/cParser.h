/*
 *  cParser.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cParser_h
#define cParser_h

#ifndef cASLibrary_h
#include "cASLibrary.h"
#endif
#ifndef cASSymbol_h
#include "cASSymbol.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif


class cParser
{
private:
  cASLibrary* m_library;
  tDictionary<cASSymbol> m_symtbl;
  
  cParser();
  
public:
  cParser(cASLibrary* library) : m_library(library) { ; }
  
};

#endif
