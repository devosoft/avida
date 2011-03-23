/*
 *  cParser.h
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cParser_h
#define cParser_h

#include <iostream>

#include "AvidaScript.h"
#include "ASTree.h"

#include "cASLibrary.h"
#include "cLexer.h"

class cFile;


class cParser
{
private:
  cString m_filename;
  
  cLexer* m_lexer;
  cASTNode* m_tree;

  bool m_eof;
  bool m_success;
  
  ASToken_t m_cur_tok;
  ASToken_t m_next_tok;
  cString* m_cur_text;
  
  bool m_err_eof;
  
  
  cParser(const cParser&); // @not_implemented
  cParser& operator=(const cParser&); // @not_implemented
  
  
public:
  cParser();
  ~cParser();
  
  bool Parse(cFile& input);
  inline cASTNode* ExtractTree() { cASTNode* tree = m_tree; m_tree = NULL; return tree; }
  
  
private:
  inline ASToken_t currentToken() const { return m_cur_tok; }
  ASToken_t nextToken();
  ASToken_t peekToken();
  
  const cString& currentText();
  
  cASTArgumentList* parseArgumentList();
  cASTNode* parseArrayUnpack();
  cASTNode* parseAssignment();
  cASTNode* parseCallExpression(cASTNode* target, bool required = false);
  cASTNode* parseCodeBlock();
  cASTNode* parseExpression();
  cASTNode* parseExprP0();
  cASTNode* parseExprP1();
  cASTNode* parseExprP2();
  cASTNode* parseExprP3();
  cASTNode* parseExprP4();
  cASTNode* parseExprP5();
  cASTNode* parseExprP6();
  cASTNode* parseExprP6_Index(cASTNode* l);
  cASTNode* parseForeachStatement();
  cASTNode* parseFunctionDefine();
  cASTFunctionDefinition* parseFunctionHeader();
  cASTNode* parseIDStatement();
  cASTNode* parseIfStatement();
  cASTNode* parseIndexExpression();
  cASTNode* parseLiteralDict();
  cASTNode* parseLooseBlock();
  cASTNode* parseRefStatement();
  cASTNode* parseReturnStatement();
  cASTNode* parseStatementList();
  cASTVariableDefinition* parseVariableDefinition();
  cASTVariableDefinitionList* parseVariableDefinitionList();
  cASTNode* parseWhileStatement();
  
  void reportError(ASParseError_t err, const int line);
};


#endif
