/*
 *  cParser.cc
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

#include "cParser.h"

#include "AvidaScript.h"
#include "cFile.h"

/*
 The following represents the grammar for AvidaScript in BNF, adjusted so that it is compatible with recursive descent
 parsing (to be) implemented by cParser.
 
 
 script: statement_list
 
 statement_list: statement statement_list
               |
 
 statement: assign_expr lineterm
          | var_declare lineterm
          | loose_block
          | if_block lineterm
          | while_block lineterm
          | foreach_block lineterm
          | declare_function lineterm
          | define_function lineterm
          | call_expr lineterm
          | return_stmt lineterm
          | lineterm
  
 lineterm: SUPPRESS | ENDL
 
 type_def: TYPE_ARRAY | TYPE_CHAR | TYPE_FLOAT | TYPE_INT | TYPE_MATRIX | TYPE_STRING | ID REF
 type_any: type_def | TYPE_VOID
 
 assign_expr: assign_dest ASSIGN expr
 
 assign_dest: ID
            | REF ARR_OPEN id_list ARR_CLOSE

 id_list: ID id_list_1
 
 id_list_1: COMMA ID id_list_1
          | COMMA ID ARR_WILD
          | COMMA ARR_WILD
          |
 
 
 expr: p0_expr 
 
 p0_expr: p1_expr p0_expr_1
 
 p0_expr_1: ARR_RANGE p1_expr p0_expr_1
          | ARR_EXPAN p1_expr p0_expr_1
          |
 
 p1_expr: p2_expr p1_expr_1
 
 p1_expr_1: OP_LOGIC_AND p2_expr p1_expr_1
          | OP_LOGIC_OR p2_expr p1_expr_1
          |

 p2_expr: p3_expr p2_expr_1
 
 p2_expr_1: OP_BIT_AND p3_expr p2_expr_1
          | OP_BIT_OR p3_expr p2_expr_1
          |

 
 p3_expr: p4_expr p3_expr_1
 
 p3_expr_1: OP_EQ p4_expr p3_expr_1
          | OP_LE p4_expr p3_expr_1
          | OP_GE p4_expr p3_expr_1
          | OP_LT p4_expr p3_expr_1
          | OP_GT p4_expr p3_expr_1
          | OP_NEQ p4_expr p3_expr_1
          |
 
 
 p4_expr: p5_expr p4_expr_1
 
 p4_expr_1: OP_ADD p5_expr p4_expr_1
          | OP_SUB p5_expr p4_expr_1
          |

 
 p5_expr: p6_expr p5_expr_1
 
 p5_expr_1: OP_MUL p6_expr p5_expr_1
          | OP_DIV p6_expr p5_expr_1
          | OP_MOD p6_expr p5_expr_1
          |
 
 p6_expr: value p6_expr_1
        | OP_BIT_NOT expr
        | OP_LOGIC_NOT expr
        | OP_SUB expr
 
 p6_expr_1: DOT ID id_expr p6_expr_1
          | IDX_OPEN expr IDX_CLOSE
          |
  
 value: FLOAT
      | INT
      | STRING
      | CHAR
      | ID id_expr
      | PREC_OPEN expr PREC_CLOSE
      | ARR_OPEN argument_list ARR_CLOSE
      | MAT_MODIFY ARR_OPEN argument_list ARR_CLOSE
 
 id_expr: PREC_OPEN argument_list PREC_CLOSE
        |
 

 call_expr: call_trgt PREC_OPEN argument_list PREC_CLOSE
          
 call_trgt: ID call_value

 call_value: DOT ID call_value
           | PREC_OPEN argument_list PREC_CLOSE call_sub_idx DOT ID call_value
           | IDX_OPEN expr IDX_CLOSE call_sub_idx DOT ID call_value
           | 
 
 call_sub_idx: IDX_OPEN expr IDX_CLOSE call_sub_idx
             |
 
 
 argument_list: argument_list_1
              |
 
 argument_list_1: expr argument_list_2
 
 argument_list_2: COMMA expr argument_list_2
                |

 loose_block: ARR_OPEN statement_list ARR_CLOSE
 
 if_block: CMD_IF PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ENDIF
         | CMD_IF PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ELSE lineterm statement_list CMD_ENDIF
         | CMD_IF PREC_OPEN expr PREC_CLOSE loose_block CMD_ENDIF
         | CMD_IF PREC_OPEN expr PREC_CLOSE loose_block CMD_ELSE loose_block CMD_ENDIF
 
 while_block: CMD_WHILE PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ENDWHILE
            | CMD_WHILE PREC_OPEN expr PREC_CLOSE loose_block
 
 foreach_block: CMD_FOREACH type_def ID PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ENDFOREACH
              | CMD_FOREACH type_def ID PREC_OPEN expr PREC_CLOSE loose_block

 var_declare: type_def ID
            | type_def ID ASSIGN expr
            | type_def ID PREC_OPEN argument_list PREC_CLOSE
 
 var_declare_list: var_declare_list_1
                 |
 
 var_declare_list_1: var_declare var_declare_list_2
 
 var_declare_list_2: COMMA var_declare var_declare_list_2
                   | 
 
 declare_function: REF CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE
 define_function: CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE lineterm statement_list CMD_ENDFUNCTION
                | CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE loose_block
 
 return_stmt: CMD_RETURN expr

 */


#define PARSE_DEBUG(x) { std::cerr << x << std::endl; }
#define PARSE_TRACE(x) { std::cerr << "trace: " << x << std::endl; }

#define PARSE_ERROR(x) reportError(AS_PARSE_ERR_ ## x, __LINE__)
#define PARSE_UNEXPECT() { if (currentToken()) { PARSE_ERROR(UNEXPECTED_TOKEN); } else { PARSE_ERROR(EOF); } }

#define TOKEN(x) AS_TOKEN_ ## x


cParser::cParser(cASLibrary* library)
: m_library(library)
, m_filename("(unknown)")
, m_eof(false)
, m_success(true)
, m_cur_tok(TOKEN(INVALID))
, m_next_tok(TOKEN(INVALID))
, m_cur_text(NULL)
, m_err_eof(false)
{
}

bool cParser::Parse(cFile& input)
{
  m_filename = input.GetFilename();
  m_lexer = new cLexer(input.GetFileStream());
  
  m_tree = parseStatementList();

  if (!m_eof) PARSE_UNEXPECT();
  if (!m_tree) PARSE_ERROR(EMPTY);

  delete m_lexer;
  m_lexer = NULL;
  
  return m_success;
}

cParser::~cParser()
{
  delete m_tree;
  delete m_lexer;
}



ASToken_t cParser::nextToken()
{
  if (m_next_tok != TOKEN(INVALID)) { 
    m_cur_tok = m_next_tok;
    m_next_tok = TOKEN(INVALID);
  } else {
    m_cur_tok = (ASToken_t)m_lexer->yylex();
  }
  delete m_cur_text;
  m_cur_text = NULL;
  PARSE_DEBUG("nextToken: " << m_cur_tok);
  return m_cur_tok;
}

ASToken_t cParser::peekToken()
{
  if (m_next_tok == TOKEN(INVALID)) {
    delete m_cur_text;
    m_cur_text = new cString(m_lexer->YYText());
    m_next_tok = (ASToken_t)m_lexer->yylex();
  }
  return m_next_tok;
}

const cString& cParser::currentText()
{
  if (!m_cur_text) m_cur_text = new cString(m_lexer->YYText());
  return *m_cur_text;
}


cASTNode* cParser::parseArrayUnpack()
{
  PARSE_TRACE("parseArrayUnpack");
  cASTNode* au = NULL;
  
  if (nextToken() != TOKEN(ID)) {
    PARSE_ERROR(UNEXPECTED_TOKEN);
    return au;
  }
  
  while (nextToken()) {
    if (currentToken() == TOKEN(COMMA)) {
      nextToken();
      if (currentToken() == TOKEN(ID)) {
        continue;
      } else if (currentToken() == TOKEN(ARR_WILD)) {
        break;
      } else {
        PARSE_ERROR(UNEXPECTED_TOKEN);
        break;
      }
    } else if (currentToken() == TOKEN(ARR_WILD)) {
      break;
    } else {
      PARSE_UNEXPECT();
      break;      
    }
  }

  return au;
}

cASTNode* cParser::parseArgumentList()
{
  PARSE_TRACE("parseArgumentList");
  cASTNode* al = NULL;
  
  parseExpression();
  while (currentToken() == TOKEN(COMMA)) {
    parseExpression();
  }
  
  return al;
}

cASTNode* cParser::parseAssignment()
{
  PARSE_TRACE("parseAssignment");
  cASTAssignment* an = new cASTAssignment(currentText());
  
  nextToken();
  cASTNode* expr = parseExpression();
  an->SetExpression(expr);

  return an;
}

cASTNode* cParser::parseCallExpression()
{
  PARSE_TRACE("parseCallExpression");
  cASTNode* ce = NULL;
  
  nextToken();
  
  bool eoe = false;
  while (!eoe) {
    switch (currentToken()) {
      case TOKEN(DOT):
        if (nextToken() != TOKEN(ID)) {
          PARSE_UNEXPECT();
          return ce;
        }
        break;
      case TOKEN(PREC_OPEN):
        if (nextToken() != TOKEN(PREC_CLOSE)) parseArgumentList();
        if (currentToken() != TOKEN(PREC_CLOSE)) {
          PARSE_UNEXPECT();
          return ce;   
        }
        switch (nextToken()) {
          case TOKEN(IDX_OPEN):
            do {
              parseIndexExpression();
            } while (nextToken() == TOKEN(IDX_OPEN));
            break;
          case TOKEN(DOT):
            continue;

          default:
            eoe = true;
        }
        break;
      case TOKEN(IDX_OPEN):
        do {
          parseIndexExpression();
        } while (nextToken() == TOKEN(IDX_OPEN));

      default:
        PARSE_UNEXPECT();
        return ce;
    }
  }
    
  return ce;
}

cASTNode* cParser::parseCodeBlock(bool& loose)
{
  PARSE_TRACE("parseCodeBlock");
  cASTNode* cb = NULL;

  if (currentToken() == TOKEN(ARR_OPEN)) {
    loose = true;
    cb = parseLooseBlock();
  } else if (currentToken() == TOKEN(SUPPRESS) || currentToken() == TOKEN(ENDL)) {
    cb = parseStatementList();
  } else {
    PARSE_UNEXPECT();
    return cb;
  }
  
  return cb;  
}

cASTNode* cParser::parseExpression()
{
  PARSE_TRACE("parseExpression");
  cASTNode* expr = NULL;
  
  expr = parseExprP0();
  if (!expr) {
    if (currentToken())
      PARSE_ERROR(NULL_EXPR);
    else
      PARSE_ERROR(EOF);
  }
  
  return expr;
}

cASTNode* cParser::parseExprP0()
{
  PARSE_TRACE("parseExprP0");
  cASTNode* l = parseExprP1();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(ARR_RANGE):
      case TOKEN(ARR_EXPAN):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP1();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP1()
{
  PARSE_TRACE("parseExprP1");
  cASTNode* l = parseExprP2();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(OP_LOGIC_AND):
      case TOKEN(OP_LOGIC_OR):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP2();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP2()
{
  PARSE_TRACE("parseExprP2");
  cASTNode* l = parseExprP3();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(OP_BIT_AND):
      case TOKEN(OP_BIT_OR):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP3();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP3()
{
  PARSE_TRACE("parseExprP3");
  cASTNode* l = parseExprP4();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(OP_EQ):
      case TOKEN(OP_LE):
      case TOKEN(OP_GE):
      case TOKEN(OP_LT):
      case TOKEN(OP_GT):
      case TOKEN(OP_NEQ):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP4();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP4()
{
  PARSE_TRACE("parseExprP4");
  cASTNode* l = parseExprP5();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(OP_ADD):
      case TOKEN(OP_SUB):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP5();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP5()
{
  PARSE_TRACE("parseExprP5");
  cASTNode* l = parseExprP6();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case TOKEN(OP_MUL):
      case TOKEN(OP_DIV):
      case TOKEN(OP_MOD):
        ASToken_t op = currentToken();
        nextToken();
        r = parseExprP6();
        if (!r) PARSE_ERROR(NULL_EXPR);
        l = new cASTExpressionBinary(op, l, r);
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP6()
{
  PARSE_TRACE("parseExprP6");
  cASTNode* expr = NULL;
  
  switch (currentToken()) {
    case TOKEN(FLOAT):
      expr = new cASTLiteral(AS_TYPE_FLOAT, currentText());
      break;
    case TOKEN(INT):
      expr = new cASTLiteral(AS_TYPE_INT, currentText());
      break;
    case TOKEN(CHAR):
      expr = new cASTLiteral(AS_TYPE_CHAR, currentText());
      break;
    case TOKEN(STRING):
      expr = new cASTLiteral(AS_TYPE_STRING, currentText());
      break;
    case TOKEN(ID):
      if (peekToken() == TOKEN(PREC_OPEN)) {
        nextToken();
        if (nextToken() != TOKEN(PREC_CLOSE)) parseArgumentList();
        if (currentToken() != TOKEN(PREC_CLOSE)) {
          PARSE_UNEXPECT();
          return expr;
        }
        expr = new cASTFunctionCall(); // @todo
      } else {
        expr = new cASTVariableReference(currentText());
      }
      break;
    case TOKEN(PREC_OPEN):
      nextToken();
      expr = parseExpression();
      if (!expr) PARSE_ERROR(NULL_EXPR);
      if (currentToken() != TOKEN(PREC_CLOSE)) {
        PARSE_UNEXPECT();
        return expr;
      }
      break;
    case TOKEN(MAT_MODIFY):
      if (nextToken() != TOKEN(ARR_OPEN)) {
        PARSE_UNEXPECT();
        return expr;
      }
    case TOKEN(ARR_OPEN):
      if (nextToken() != TOKEN(ARR_CLOSE)) parseArgumentList();
      if (currentToken() != TOKEN(ARR_CLOSE)) {
        PARSE_UNEXPECT();
        return expr;
      }
      // @todo - return literal array
      break;
      
    case TOKEN(OP_BIT_NOT):
    case TOKEN(OP_LOGIC_NOT):
    case TOKEN(OP_SUB):
      ASToken_t op = currentToken();
      expr = new cASTExpressionUnary(op, parseExpression());
      if (!expr) PARSE_ERROR(NULL_EXPR);
      nextToken();
      return expr;
      
    default:
      break;
  }

  nextToken();
  if (expr) expr = parseExprP6_Index(expr);
  return expr;
}

cASTNode* cParser::parseExprP6_Index(cASTNode* l)
{
  PARSE_TRACE("parseExprP6_Index");
  while (currentToken() == TOKEN(DOT) || currentToken() == TOKEN(IDX_OPEN)) {
    if (currentToken() == TOKEN(DOT)) {
      if (nextToken() != TOKEN(ID)) {
        PARSE_UNEXPECT();
        return l;
      }
      if (peekToken() == TOKEN(PREC_OPEN)) {
        nextToken();
        if (nextToken() != TOKEN(PREC_CLOSE)) parseArgumentList();
        if (currentToken() != TOKEN(PREC_CLOSE)) {
          PARSE_UNEXPECT();
          return l;
        }
        // @todo
      } else {
        // @todo
      }
    } else { // IDX_OPEN:
      nextToken();
      parseExpression();
      if (currentToken() != TOKEN(IDX_CLOSE)) {
        PARSE_UNEXPECT();
        return l;
      }
      // @todo
    }
  }
  
  return l;
}


cASTNode* cParser::parseForeachStatement()
{
  PARSE_TRACE("parseForeachStatement");
  cASTNode* fs = NULL;
  
  switch (nextToken()) {
    case TOKEN(TYPE_ARRAY):
    case TOKEN(TYPE_CHAR):
    case TOKEN(TYPE_FLOAT):
    case TOKEN(TYPE_INT):
    case TOKEN(TYPE_MATRIX):
    case TOKEN(TYPE_STRING):
      break;
    case TOKEN(ID):
      if (nextToken() != TOKEN(REF)) {
        PARSE_UNEXPECT();
        return fs;
      }
      break;
      
    default:
      PARSE_UNEXPECT();
      return fs;
  }
  
  if (nextToken() != TOKEN(PREC_OPEN)) {
    PARSE_UNEXPECT();
    return fs;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != TOKEN(PREC_CLOSE)) {
    PARSE_UNEXPECT();
    return fs;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (!loose && currentToken() != TOKEN(CMD_ENDFOREACH)) PARSE_UNEXPECT();
  
  return fs;
}

cASTNode* cParser::parseFunctionDefine()
{
  PARSE_TRACE("parseFunctionDefine");
  cASTFunctionDefinition* fd = parseFunctionHeader(false);
  
  bool loose = false;
  fd->SetCode(parseCodeBlock(loose));
  if (!loose && currentToken() != TOKEN(CMD_ENDFUNCTION)) {
    PARSE_UNEXPECT();
    return fd;
  }

  nextToken();
  return fd;
}

cASTFunctionDefinition* cParser::parseFunctionHeader(bool declare)
{
  PARSE_TRACE("parseFunctionHeader");
  
  ASType_t type = AS_TYPE_INVALID;
  switch (nextToken()) {
    case TOKEN(TYPE_ARRAY):  type = AS_TYPE_ARRAY;  break;
    case TOKEN(TYPE_CHAR):   type = AS_TYPE_CHAR;   break;
    case TOKEN(TYPE_FLOAT):  type = AS_TYPE_FLOAT;  break;
    case TOKEN(TYPE_INT):    type = AS_TYPE_INT;    break;
    case TOKEN(TYPE_MATRIX): type = AS_TYPE_MATRIX; break;
    case TOKEN(TYPE_STRING): type = AS_TYPE_STRING; break;
    case TOKEN(TYPE_VOID):   type = AS_TYPE_VOID;   break;
    case TOKEN(ID):
      if (peekToken() != TOKEN(REF)) {
        nextToken();
        PARSE_UNEXPECT();
        return NULL;
      }
      type = AS_TYPE_OBJECT_REF;
      break;
      
    default:
      PARSE_UNEXPECT();
      return NULL;
  }
  
  if (nextToken() != TOKEN(ID)) {
    PARSE_UNEXPECT();
    return NULL;
  }
  cString name(currentText());
  
  if (nextToken() != TOKEN(PREC_OPEN)) {
    PARSE_UNEXPECT();
    return NULL;
  }
  
  cASTNode* args = NULL;
  if (nextToken() != TOKEN(PREC_CLOSE)) {
    if (declare) {
      args = parseVarDeclareList();
    } else {
      args = parseArgumentList();
    }
  }
  
  if (currentToken() != TOKEN(PREC_CLOSE)) {
    PARSE_UNEXPECT();
    return NULL;    
  }
  
  nextToken();
  
  return new cASTFunctionDefinition(type, name, args);
}

cASTNode* cParser::parseIDStatement()
{
  PARSE_TRACE("parseIDStatement");
  cASTNode* is = NULL;
  
  switch (peekToken()) {
    case TOKEN(ASSIGN):
      is = parseAssignment();
      break;
    case TOKEN(DOT):
    case TOKEN(IDX_OPEN):
    case TOKEN(PREC_OPEN):
      is = parseCallExpression();
      break;
    case TOKEN(REF):
      is = parseVarDeclare();
      break;
      
    default:
      PARSE_UNEXPECT();
      break;
  }      
  
  return is;
}

cASTNode* cParser::parseIfStatement()
{
  PARSE_TRACE("parseIfStatement");
  cASTNode* is = NULL;
  
  if (nextToken() != TOKEN(PREC_OPEN)) {
    PARSE_UNEXPECT();
    return is;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != TOKEN(PREC_CLOSE)) {
    PARSE_UNEXPECT();
    return is;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (currentToken() == TOKEN(CMD_ELSE)) {
    parseCodeBlock(loose);
    if (!loose && currentToken() != TOKEN(CMD_ENDIF)) {
      PARSE_UNEXPECT();
      return is;
    }
  } else if (!loose && currentToken() != TOKEN(CMD_ENDIF)) {
    PARSE_UNEXPECT();
    return is;
  }
  
  return is;
}

cASTNode* cParser::parseIndexExpression()
{
  PARSE_TRACE("parseIndexExpression");
  cASTNode* ie = NULL;
  
  nextToken();
  parseExpression();
  if (currentToken() != TOKEN(IDX_CLOSE)) {
    PARSE_UNEXPECT();
  }
  
  return ie;
}

cASTNode* cParser::parseLooseBlock()
{
  PARSE_TRACE("parseLooseBlock");
  nextToken();
  cASTNode* sl = parseStatementList();
  if (currentToken() != TOKEN(ARR_CLOSE)) {
    PARSE_UNEXPECT();
  }
  return sl;
}

cASTNode* cParser::parseRefStatement()
{
  PARSE_TRACE("parseRefStatement");
  cASTNode* rs = NULL;

  switch (nextToken()) {
    case TOKEN(ARR_OPEN):
      rs = parseArrayUnpack();
      break;
    case TOKEN(CMD_FUNCTION):
      rs = parseFunctionHeader();
      break;
    default:
      PARSE_UNEXPECT();
  }
  
  return rs;
}

cASTNode* cParser::parseReturnStatement()
{
  PARSE_TRACE("parseReturnStatement");
  
  nextToken();
  cASTNode* rs = new cASTReturnStatement(parseExpression());
  
  return rs;
}


#define CHECK_LINETERM() { if (!checkLineTerm(sl)) return sl; }
cASTNode* cParser::parseStatementList()
{
  PARSE_TRACE("parseStatementList");
  cASTStatementList* sl = new cASTStatementList();
  
  cASTNode* node = NULL;

  while (nextToken()) {
    switch (currentToken()) {
      case TOKEN(ARR_OPEN):
        node = parseLooseBlock();
        CHECK_LINETERM();
        break;
      case TOKEN(CMD_IF):
        node = parseIfStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(CMD_FOREACH):
        node = parseForeachStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(CMD_FUNCTION):
        node = parseFunctionDefine();
        CHECK_LINETERM();
        break;
      case TOKEN(CMD_RETURN):
        node = parseReturnStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(CMD_WHILE):
        node = parseWhileStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(ENDL):
        continue;
      case TOKEN(ID):
        node = parseIDStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(REF):
        node = parseRefStatement();
        CHECK_LINETERM();
        break;
      case TOKEN(SUPPRESS):
        continue;
      case TOKEN(TYPE_ARRAY):
      case TOKEN(TYPE_CHAR):
      case TOKEN(TYPE_FLOAT):
      case TOKEN(TYPE_INT):
      case TOKEN(TYPE_MATRIX):
      case TOKEN(TYPE_STRING):
        node = parseVarDeclare();
        CHECK_LINETERM();
        break;
        
      default:
        return sl;
    }
    
    if (node == NULL) {
      // Some error has occured, so terminate early
      if (m_success) PARSE_ERROR(INTERNAL); // Should not receive a null response without an error flag
      break;
    }
    sl->AddNode(node);
  }
  
  if (!currentToken()) m_eof = true;
  return sl;
}
#undef CHECK_LINETERM()


cASTNode* cParser::parseVarDeclare()
{
  PARSE_TRACE("parseVarDeclare");
  cASTVariableDefinition* vd = NULL;
  
  ASType_t vtype = AS_TYPE_INVALID;
  switch (currentToken()) {
    case TOKEN(TYPE_ARRAY):  vtype = AS_TYPE_ARRAY;  break;
    case TOKEN(TYPE_CHAR):   vtype = AS_TYPE_CHAR;   break;
    case TOKEN(TYPE_FLOAT):  vtype = AS_TYPE_FLOAT;  break;
    case TOKEN(TYPE_INT):    vtype = AS_TYPE_INT;    break;
    case TOKEN(TYPE_MATRIX): vtype = AS_TYPE_MATRIX; break;
    case TOKEN(TYPE_STRING): vtype = AS_TYPE_STRING; break;
    case TOKEN(ID):
      if (nextToken() != TOKEN(REF)) {
        PARSE_UNEXPECT();
        return vd;
      }
      vtype = AS_TYPE_OBJECT_REF;
      break;
      
    default:
      PARSE_UNEXPECT();
      return vd;
  }
  
  if (nextToken() != TOKEN(ID)) {
    PARSE_UNEXPECT();
    return vd;
  }
  
  vd = new cASTVariableDefinition(vtype, currentText());
  
  switch (nextToken()) {
    case TOKEN(ASSIGN):
      nextToken();
      cASTNode* expr = parseExpression();
      vd->SetAssignmentExpression(expr);
      break;
    case TOKEN(PREC_OPEN):
      // @todo - array/matrix size declaration
      if (nextToken() != TOKEN(PREC_CLOSE)) parseArgumentList();
      if (currentToken() != TOKEN(PREC_CLOSE)) {
        PARSE_UNEXPECT();
        return vd;
      }
      break;
      
    default:
      break;
  }
  
  return vd;
}

cASTNode* cParser::parseVarDeclareList()
{
  PARSE_TRACE("parseVarDeclareList");
  cASTNode* vl = NULL;
  
  parseVarDeclare();
  while (currentToken() == TOKEN(COMMA)) {
    parseVarDeclare();
  }
  
  return vl;
}

cASTNode* cParser::parseWhileStatement()
{
  PARSE_TRACE("parseWhileStatement");
  cASTNode* ws = NULL;
  
  if (nextToken() != TOKEN(PREC_OPEN)) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != TOKEN(PREC_CLOSE)) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (!loose && currentToken() != TOKEN(CMD_ENDWHILE)) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  return ws;
}


bool cParser::checkLineTerm(cASTNode* node)
{
  PARSE_TRACE("checkLineTerm");
  if (currentToken() == TOKEN(SUPPRESS)) {
    // @todo - mark output as suppressed
    return true;
  } else if (currentToken() == TOKEN(ENDL)) {
    return true;
  }
  
  PARSE_ERROR(UNTERMINATED_EXPR);
  return false;
}


void cParser::reportError(ASParseError_t err, const int line)
{
#define ERR_ENDL "  (cParser.cc:" << line << ")" << std::endl
  
  m_success = false;

  std::cerr << m_filename << ":";
  
  int lineno = m_lexer ? m_lexer->lineno() : 0;
  if (lineno) std::cerr << lineno;
  else std::cerr << "?";
  
  std::cerr << ": error: ";

  switch (err) {
    case AS_PARSE_ERR_UNEXPECTED_TOKEN:
      std::cerr << "unexpected token '" << currentText() << "'" << ERR_ENDL;
      break;
    case AS_PARSE_ERR_UNTERMINATED_EXPR:
      std::cerr << "unterminated expression" << ERR_ENDL;
      break;
    case AS_PARSE_ERR_NULL_EXPR:
      std::cerr << "expected expression, found '" << currentText() << "'" << ERR_ENDL;
    case AS_PARSE_ERR_EOF:
      if (!m_err_eof) {
        std::cerr << "unexpected end of file" << ERR_ENDL;
        m_err_eof = true;
      }
      break;
    case AS_PARSE_ERR_EMPTY:
      std::cerr << "empty script, no valid statements found" << ERR_ENDL;
      break;
    case AS_PARSE_ERR_INTERNAL:
      std::cerr << "internal parser error at cParser.cc:" << line << std::endl;
    case AS_PARSE_ERR_UNKNOWN:
    default:
      std::cerr << "parse error" << std::endl;
  }

#undef ERR_ENDL
}

#undef PARSE_DEBUG()
#undef PARSE_TRACE()

#undef PARSE_ERROR()
#undef PARSE_UNEXPECT()

#undef TOKEN()
