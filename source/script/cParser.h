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

#ifndef AvidaScript_h
#include "AvidaScript.h"
#endif
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
  
  ASToken_t m_cur_tok;
  ASToken_t m_next_tok;
  
  bool m_err_eof;
  
  
  cParser(); // @not_implemented
  cParser(const cParser&); // @not_implemented
  cParser& operator=(const cParser&); // @not_implemented
  
  
public:
  cParser(cASLibrary* library);
  
  bool Parse(cFile& input);
  void Accept(cASTVisitor& visitor);
  
  
private:
  inline ASToken_t currentToken() { return m_cur_tok; }
  ASToken_t nextToken();
  inline ASToken_t peekToken();
  
  cASTNode* parseArgumentList();
  cASTNode* parseArrayUnpack();
  cASTNode* parseAssignment();
  cASTNode* parseCallExpression();
  cASTNode* parseCodeBlock(bool& loose);
  cASTNode* parseExpression();
  cASTNode* parseForeachStatement();
  cASTNode* parseFunctionDefine();
  cASTNode* parseFunctionHeader(bool declare = true);
  cASTNode* parseIDStatement();
  cASTNode* parseIfStatement();
  cASTNode* parseIndexExpression();
  cASTNode* parseLooseBlock();
  cASTNode* parseRefStatement();
  cASTNode* parseReturnStatement();
  cASTNode* parseStatementList();
  cASTNode* parseVarDeclare();
  cASTNode* parseVarDeclareList();
  cASTNode* parseWhileStatement();
  
  bool checkLineTerm(cASTNode* node);
  
  void reportError(ASParseError_t err, const int line);
};


inline ASToken_t cParser::peekToken()
{
  if (m_next_tok == INVALID) m_next_tok = (ASToken_t)m_lexer->yylex();
  return m_next_tok;
}


#endif
