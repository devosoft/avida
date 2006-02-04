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

#include <iostream>

#ifndef cASLibrary_h
#include "cASLibrary.h"
#endif
#ifndef cLexer_h
#include "cLexer.h"
#endif
#ifndef cScriptObject_h
#include "cScriptObject.h"
#endif
#ifndef cSymbolTable_h
#include "cSymbolTable.h"
#endif


class cParser
{
private:
  cASLibrary* m_library;
  cLexer* m_lexer;
  cSymbolTable* m_symtbl;
  
  cParser();
  
public:
  cParser(cASLibrary* library) : m_library(library), m_symtbl(NULL) { ; }
  
  cScriptObject* Parse(std::istream* input);
};

#endif
