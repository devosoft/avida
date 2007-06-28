/*
 *  cParser.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cParser_h
#define cParser_h

#include <iostream>

#ifndef ASTree_h
#include "ASTree.h"
#endif
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

class cFile;


class cParser
{
private:
  cASLibrary* m_library;
  cLexer* m_lexer;
  cASTNode* m_tree;

  bool m_eof;
  bool m_success;
  
  int m_cur_tok;
  
  cParser();
  
public:
  cParser(cASLibrary* library) : m_library(library), m_eof(false), m_success(true), m_cur_tok(0) { ; }
  
  bool Parse(cFile& input);
  
  void Accept(cASTVisitor& visitor);
  
  
private:
  inline int currentToken() { return m_cur_tok; }
  inline int nextToken();
  
  cASTNode* parseStatementList();
  cASTNode* parseLooseBlock();
};


inline int cParser::nextToken()
{
  m_cur_tok = m_lexer->yylex();
  return m_cur_tok;
}


#endif
