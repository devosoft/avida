/*
 *  cASSymbol.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cASSymbol_h
#define cASSymbol_h

#ifndef cString_h
#include "cString.h"
#endif

class cASSymbol
{
private:
  cString m_name;
  
  cASSymbol();
  
public:
  cASSymbol(cString name) : m_name(name) { ; }
};

#endif
