/*
 *  cParser.cc
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
#include "tAutoRelease.h"

using namespace AvidaScript;

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
 
 type_def: TYPE_ARRAY | TYPE_BOOL | TYPE_CHAR | TYPE_FLOAT | TYPE_INT | TYPE_MATRIX | TYPE_STRING | ID REF
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
 
 if_block: CMD_IF PREC_OPEN expr PREC_CLOSE loose_block
         | CMD_IF PREC_OPEN expr PREC_CLOSE loose_block CMD_ELSE loose_block
 
 while_block: CMD_WHILE PREC_OPEN expr PREC_CLOSE loose_block
 
 foreach_block: CMD_FOREACH type_def ID PREC_OPEN expr PREC_CLOSE loose_block

 var_declare: type_def ID
            | type_def ID ASSIGN expr
            | type_def ID PREC_OPEN argument_list PREC_CLOSE
 
 var_declare_list: var_declare_list_1
                 |
 
 var_declare_list_1: var_declare var_declare_list_2
 
 var_declare_list_2: COMMA var_declare var_declare_list_2
                   | 
 
 declare_function: REF CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE
 define_function: CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE loose_block
 
 return_stmt: CMD_RETURN expr

 */

#ifndef DEBUG_AS_PARSER
#define DEBUG_AS_PARSER 0
#endif

#if DEBUG_AS_PARSER
# define PARSE_DEBUG(x) { std::cerr << x << std::endl; }
# define PARSE_TRACE(x) { std::cerr << "trace: " << x << std::endl; }
#else
# define PARSE_DEBUG(x)
# define PARSE_TRACE(x)
#endif

#define PARSE_ERROR(x) reportError(AS_PARSE_ERR_ ## x, __LINE__)
#define PARSE_UNEXPECT() { if (currentToken()) { PARSE_ERROR(UNEXPECTED_TOKEN); } else { PARSE_ERROR(EOF); } return NULL; }

#define FILEPOS cASFilePosition(m_filename, m_lexer ? m_lexer->lineno() : 0)

#define TOKEN(x) AS_TOKEN_ ## x


cParser::cParser()
: m_filename("(unknown)")
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

  if (!m_eof && m_success) PARSE_ERROR(UNEXPECTED_TOKEN);
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
  
  if (nextToken() != TOKEN(ID)) PARSE_UNEXPECT();

  tAutoRelease<cASTUnpackTarget> ut(new cASTUnpackTarget(FILEPOS));
  (*ut).AddVar(currentText());
  
  while (nextToken()) {
    if (currentToken() == TOKEN(COMMA)) {
      nextToken();
      if (currentToken() == TOKEN(ID)) {
        (*ut).AddVar(currentText());
        continue;
      } else if (currentToken() == TOKEN(ARR_WILD)) {
        (*ut).SetLastWild();
        break;
      } else {
        PARSE_ERROR(UNEXPECTED_TOKEN);
        break;
      }
    } else if (currentToken() == TOKEN(ARR_WILD)) {
      (*ut).SetLastNamed();
      break;
    } else {
      PARSE_UNEXPECT();
    }
  }
  
  if (nextToken() != TOKEN(ARR_CLOSE)) PARSE_UNEXPECT();
  if (nextToken() != TOKEN(ASSIGN)) PARSE_UNEXPECT();
  nextToken(); // consume '='
  
  (*ut).SetExpression(parseExpression());

  return ut.Release();
}

cASTArgumentList* cParser::parseArgumentList()
{
  PARSE_TRACE("parseArgumentList");
  cASTArgumentList* al = new cASTArgumentList(FILEPOS);

  al->AddNode(parseExpression());
  while (currentToken() == TOKEN(COMMA)) {
    nextToken(); // consume ','
    al->AddNode(parseExpression());
  }
  
  return al;
}

cASTNode* cParser::parseAssignment()
{
  PARSE_TRACE("parseAssignment");
  cASTAssignment* an = new cASTAssignment(FILEPOS, currentText());
  
  nextToken(); // consume id

  nextToken(); // consume '='
  cASTNode* expr = parseExpression();
  an->SetExpression(expr);

  return an;
}

cASTNode* cParser::parseCallExpression(cASTNode* target, bool required)
{
  PARSE_TRACE("parseCallExpression");
  tAutoRelease<cASTNode> ce(target);

  if (currentToken() == TOKEN(DOT) && peekToken() == TOKEN(BUILTIN_METHOD)) {
    nextToken(); // consume '.'
    
    cASTBuiltInCall* bi = new cASTBuiltInCall(FILEPOS, currentText(), ce.Release());
    ce.Set(bi);
    
    if (nextToken() != TOKEN(PREC_OPEN)) PARSE_UNEXPECT();
    if (nextToken() != TOKEN(PREC_CLOSE)) bi->SetArguments(parseArgumentList());
    if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
    nextToken(); // consume ')'
    
    if (currentToken() != TOKEN(DOT) || currentToken() != TOKEN(IDX_OPEN)) return ce.Release();
  }
  
  
  bool eoe = false;
  while (!eoe) {
    if (currentToken() == TOKEN(DOT)) {
      if (nextToken() != TOKEN(ID)) PARSE_UNEXPECT();
      cString name(currentText());
      nextToken(); // consume id

      if (currentToken() == TOKEN(PREC_OPEN)) {
        cASTObjectCall* oc = new cASTObjectCall(FILEPOS, ce.Release(), name);
        ce.Set(oc);
        if (nextToken() != TOKEN(PREC_CLOSE)) oc->SetArguments(parseArgumentList());
        if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
        nextToken(); // consume ')'
        
        // If the next token is not a continued call expression, then set the end-of-expression flag
        if (currentToken() != TOKEN(IDX_OPEN) && currentToken() != TOKEN(DOT)) eoe = true;
      } else {
        ce.Set(new cASTObjectReference(FILEPOS, ce.Release(), name));
        
        if (required && currentToken() == TOKEN(ASSIGN)) {
          cASTObjectAssignment* oa = new cASTObjectAssignment(FILEPOS, ce.Release());
          ce.Set(oa);
          nextToken(); // consume '='
          oa->SetExpression(parseExpression());
          eoe = true;
        }
      }
    } else if (currentToken() == TOKEN(IDX_OPEN)) {
      do {
        nextToken(); // consume '['
        ce.Set(new cASTExpressionBinary(FILEPOS, TOKEN(IDX_OPEN), ce.Release(), parseExpression()));
        if (currentToken() != TOKEN(IDX_CLOSE)) PARSE_UNEXPECT();
      } while (nextToken() == TOKEN(IDX_OPEN));
      
      if (required && currentToken() == TOKEN(ASSIGN)) {
        cASTObjectAssignment* oa = new cASTObjectAssignment(FILEPOS, ce.Release());
        ce.Set(oa);
        nextToken(); // consume '='
        oa->SetExpression(parseExpression());
        eoe = true;
      }      
    } else {
      if (required) { PARSE_UNEXPECT(); }
      else eoe = true;
    }
  }
  
  return ce.Release();
}

cASTNode* cParser::parseCodeBlock()
{
  PARSE_TRACE("parseCodeBlock");

  // Swallow all newlines and suppress tokens
  while (currentToken() == TOKEN(ENDL) || currentToken() == TOKEN(SUPPRESS)) nextToken();
  
  if (currentToken() == TOKEN(ARR_OPEN)) return parseLooseBlock();
  
  PARSE_UNEXPECT();
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
        l = new cASTExpressionBinary(FILEPOS, op, l, r);
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
  tAutoRelease<cASTNode> expr;
  
  bool is_matrix = false;
  
  switch (currentToken()) {
    case TOKEN(FLOAT):
      expr.Set(new cASTLiteral(FILEPOS, AS_TYPE_FLOAT, currentText()));
      break;
    case TOKEN(INT):
      expr.Set(new cASTLiteral(FILEPOS, AS_TYPE_INT, currentText()));
      break;
    case TOKEN(CHAR):
      expr.Set(new cASTLiteral(FILEPOS, AS_TYPE_CHAR,
                               currentText().Substring(1, currentText().GetSize() - 2).ParseEscapeSequences()));
      break;
    case TOKEN(BOOL):
      expr.Set(new cASTLiteral(FILEPOS, AS_TYPE_BOOL, currentText()));
      break;
    case TOKEN(STRING):
      expr.Set(new cASTLiteral(FILEPOS, AS_TYPE_STRING,
                               currentText().Substring(1, currentText().GetSize() - 2).ParseEscapeSequences()));
      break;
    case TOKEN(ID):
      if (peekToken() == TOKEN(PREC_OPEN)) {
        cASTFunctionCall* fc = new cASTFunctionCall(FILEPOS, currentText());
        expr.Set(fc);
        nextToken(); // consume id token
        if (nextToken() != TOKEN(PREC_CLOSE)) fc->SetArguments(parseArgumentList());        
        if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
      } else {
        expr = new cASTVariableReference(FILEPOS, currentText());
      }
      break;
    case TOKEN(BUILTIN_CALL):
      if (peekToken() == TOKEN(PREC_OPEN)) {
        cASTBuiltInCall* bi = new cASTBuiltInCall(FILEPOS, currentText());
        expr.Set(bi);
        nextToken(); // consume builtin methon name token
        if (nextToken() != TOKEN(PREC_CLOSE)) bi->SetArguments(parseArgumentList());        
        if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
      } else {
        PARSE_UNEXPECT();
      }
      break;
    case TOKEN(PREC_OPEN):
      nextToken(); // consume '('
      expr.Set(parseExpression());
      if (expr.IsNull()) PARSE_ERROR(NULL_EXPR);
      if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
      break;
      
    case TOKEN(LITERAL_DICT):
      expr.Set(parseLiteralDict());
      if (currentToken() != TOKEN(ARR_CLOSE)) PARSE_UNEXPECT();
      break;

    case TOKEN(LITERAL_MATRIX):
      is_matrix = true;
    case TOKEN(ARR_OPEN):
      {
        tAutoRelease<cASTArgumentList> al;
        if (nextToken() != TOKEN(ARR_CLOSE)) al.Set(parseArgumentList());
        if (currentToken() != TOKEN(ARR_CLOSE)) PARSE_UNEXPECT();
        expr.Set(new cASTLiteralArray(FILEPOS, al.Release(), is_matrix));
      }
      break;
      
    case TOKEN(OP_BIT_NOT):
    case TOKEN(OP_LOGIC_NOT):
    case TOKEN(OP_SUB):
      ASToken_t op = currentToken();
      nextToken(); // consume operation
      cASTNode* r = parseExprP6();
      if (!r) {
        PARSE_ERROR(NULL_EXPR);
        return NULL;
      }
      expr.Set(new cASTExpressionUnary(FILEPOS, op, r));
      return expr.Release();
      
    default:
      return NULL;
  }

  nextToken();
  if (!expr.IsNull()) return parseCallExpression(expr.Release());
  
  return NULL;
}


cASTNode* cParser::parseForeachStatement()
{
  PARSE_TRACE("parseForeachStatement");
  
  sASTypeInfo type(AS_TYPE_INVALID);
  switch (nextToken()) {
    case TOKEN(TYPE_ARRAY):  type.type = AS_TYPE_ARRAY;  break;
    case TOKEN(TYPE_BOOL):   type.type = AS_TYPE_BOOL;   break;
    case TOKEN(TYPE_CHAR):   type.type = AS_TYPE_CHAR;   break;
    case TOKEN(TYPE_DICT):   type.type = AS_TYPE_DICT;   break;
    case TOKEN(TYPE_FLOAT):  type.type = AS_TYPE_FLOAT;  break;
    case TOKEN(TYPE_INT):    type.type = AS_TYPE_INT;    break;
    case TOKEN(TYPE_MATRIX): type.type = AS_TYPE_MATRIX; break;
    case TOKEN(TYPE_STRING): type.type = AS_TYPE_STRING; break;
    case TOKEN(TYPE_VAR):    type.type = AS_TYPE_VAR;    break;
    case TOKEN(TYPE_VOID):   type.type = AS_TYPE_VOID;   break;
    case TOKEN(ID):
      if (peekToken() != TOKEN(REF)) {
        nextToken();
        PARSE_UNEXPECT();
      }
      
      type.type = AS_TYPE_OBJECT_REF;
      type.info = currentText();
      
      nextToken(); // consume id
      break;
      
    default:
      PARSE_UNEXPECT();
      return NULL;
  }
  
  if (nextToken() != TOKEN(ID)) {
    PARSE_UNEXPECT();
    return NULL;
  }
  
  tAutoRelease<cASTVariableDefinition> var(new cASTVariableDefinition(FILEPOS, type, currentText()));
  
  if (nextToken() != TOKEN(PREC_OPEN)) {
    PARSE_UNEXPECT();
    return NULL;
  }  
  nextToken(); // consume '('
  
  tAutoRelease<cASTNode> expr(parseExpression());
  
  if (currentToken() != TOKEN(PREC_CLOSE)) {
    PARSE_UNEXPECT();
    return NULL;
  }
  nextToken(); // consume ')'
  
  cASTNode* code = parseCodeBlock();
  
  return new cASTForeachBlock(FILEPOS, var.Release(), expr.Release(), code);
}

cASTNode* cParser::parseFunctionDefine()
{
  PARSE_TRACE("parseFunctionDefine");
  cASTFunctionDefinition* fd = parseFunctionHeader();
  
  // If the returned function definition is valid, parse the body
  if (fd) fd->SetCode(parseCodeBlock());

  return fd;
}

cASTFunctionDefinition* cParser::parseFunctionHeader()
{
  PARSE_TRACE("parseFunctionHeader");
  
  sASTypeInfo type(AS_TYPE_INVALID);
  switch (nextToken()) {
    case TOKEN(TYPE_ARRAY):  type.type = AS_TYPE_ARRAY;  break;
    case TOKEN(TYPE_BOOL):   type.type = AS_TYPE_BOOL;   break;
    case TOKEN(TYPE_CHAR):   type.type = AS_TYPE_CHAR;   break;
    case TOKEN(TYPE_DICT):   type.type = AS_TYPE_DICT;   break;
    case TOKEN(TYPE_FLOAT):  type.type = AS_TYPE_FLOAT;  break;
    case TOKEN(TYPE_INT):    type.type = AS_TYPE_INT;    break;
    case TOKEN(TYPE_MATRIX): type.type = AS_TYPE_MATRIX; break;
    case TOKEN(TYPE_STRING): type.type = AS_TYPE_STRING; break;
    case TOKEN(TYPE_VAR):    type.type = AS_TYPE_VAR;    break;
    case TOKEN(TYPE_VOID):   type.type = AS_TYPE_VOID;   break;
    case TOKEN(ID):
      if (peekToken() != TOKEN(REF)) {
        nextToken();
        PARSE_UNEXPECT();
      }
      
      type.type = AS_TYPE_OBJECT_REF;
      type.info = currentText();
      
      nextToken(); // consume id
      break;
      
    default:
      PARSE_UNEXPECT();
  }
  
  if (nextToken() != TOKEN(ID)) {
    PARSE_UNEXPECT();
  }
  cString name(currentText());
  
  if (nextToken() != TOKEN(PREC_OPEN)) PARSE_UNEXPECT();
  
  tAutoRelease<cASTVariableDefinitionList> args;
  if (nextToken() != TOKEN(PREC_CLOSE)) args.Set(parseVariableDefinitionList());
  if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
  nextToken(); // consume ')'
  
  return new cASTFunctionDefinition(FILEPOS, type, name, args.Release());
}

cASTNode* cParser::parseIDStatement()
{
  PARSE_TRACE("parseIDStatement");
  
  switch (peekToken()) {
    case TOKEN(ASSIGN):
      return parseAssignment();
      break;
    case TOKEN(PREC_OPEN):
      {
        cASTFunctionCall* fc = new cASTFunctionCall(FILEPOS, currentText());
        nextToken(); // consume id token
        if (nextToken() != TOKEN(PREC_CLOSE)) fc->SetArguments(parseArgumentList());        
        if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
        nextToken(); // consume ')'
        
        if (currentToken() == TOKEN(DOT) || currentToken() == TOKEN(IDX_OPEN)) {
          return parseCallExpression(fc, true);
        }
        
        return fc;
      }
      break;
    case TOKEN(DOT):
    case TOKEN(IDX_OPEN):
      cASTNode* target = new cASTVariableReference(FILEPOS, currentText());
      nextToken(); // consume id
      return parseCallExpression(target, true);
      break;
    case TOKEN(REF):
      return parseVariableDefinition();
      break;
      
    default:
      PARSE_UNEXPECT();
  }
}

cASTNode* cParser::parseIfStatement()
{
  PARSE_TRACE("parseIfStatement");
  
  if (nextToken() != TOKEN(PREC_OPEN)) PARSE_UNEXPECT();
  
  nextToken();
  tAutoRelease<cASTNode> cond(parseExpression());
  if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
  nextToken();
  
  tAutoRelease<cASTIfBlock> is(new cASTIfBlock(FILEPOS, cond.Release(), parseCodeBlock()));

  while (currentToken() == TOKEN(CMD_ELSEIF)) {
    
    if (nextToken() != TOKEN(PREC_OPEN)) PARSE_UNEXPECT();
    nextToken(); // consume '('
    
    tAutoRelease<cASTNode> elifcond(parseExpression());
    
    if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
    nextToken(); // consume ')'
    
    cASTNode* elifcode = parseCodeBlock();
    (*is).AddElseIf(elifcond.Release(), elifcode);
  }
  
  if (currentToken() == TOKEN(CMD_ELSE)) {
    nextToken(); // consume 'else'
    cASTNode* code = parseCodeBlock();
    (*is).SetElseCode(code);
  }
  
  return is.Release();
}


cASTNode* cParser::parseLiteralDict()
{
  PARSE_TRACE("parseLiteralDict");
  cASTLiteralDict* ld = new cASTLiteralDict(FILEPOS);
  
  if (peekToken() != TOKEN(ARR_CLOSE)) {
    do {
      nextToken(); // consume ',' (or '@{' on first pass)
      cASTNode* idxexpr = parseExpression();
      if (currentToken() != TOKEN(DICT_MAPPING)) PARSE_UNEXPECT();
      nextToken(); // consume '=>'
      cASTNode* valexpr = parseExpression();
      
      ld->AddMapping(idxexpr, valexpr);
    } while (currentToken() == TOKEN(COMMA));
  } else {
    nextToken();
  }
  
  return ld;
}

cASTNode* cParser::parseLooseBlock()
{
  PARSE_TRACE("parseLooseBlock");
  //nextToken();
  tAutoRelease<cASTNode> sl(parseStatementList());
  
  if (currentToken() != TOKEN(ARR_CLOSE)) PARSE_UNEXPECT();
  nextToken(); // consume '}'

  return sl.Release();
}

cASTNode* cParser::parseRefStatement()
{
  PARSE_TRACE("parseRefStatement");

  switch (nextToken()) {
    case TOKEN(ARR_OPEN):
      return parseArrayUnpack();
    case TOKEN(CMD_FUNCTION):
      return parseFunctionHeader();
    default:
      PARSE_UNEXPECT();
  }
}

cASTNode* cParser::parseReturnStatement()
{
  PARSE_TRACE("parseReturnStatement");
  
  nextToken(); // consume 'return'
  cASTNode* rs = new cASTReturnStatement(FILEPOS, parseExpression());
  
  return rs;
}


cASTNode* cParser::parseStatementList()
{
  PARSE_TRACE("parseStatementList");
  tAutoRelease<cASTStatementList> sl(new cASTStatementList(FILEPOS));
  
  tAutoRelease<cASTNode> node;

  while (nextToken()) {
    switch (currentToken()) {
      case TOKEN(ARR_OPEN):
        node.Set(parseLooseBlock());
        break;
      case TOKEN(CMD_IF):
        node.Set(parseIfStatement());
        break;
      case TOKEN(CMD_FOREACH):
        node.Set(parseForeachStatement());
        break;
      case TOKEN(CMD_FUNCTION):
        node.Set(parseFunctionDefine());
        break;
      case TOKEN(CMD_RETURN):
        node.Set(parseReturnStatement());
        break;
      case TOKEN(CMD_WHILE):
        node.Set(parseWhileStatement());
        break;
      case TOKEN(ENDL):
        continue;
      case TOKEN(ID):
        node.Set(parseIDStatement());
        break;
      case TOKEN(REF):
        node.Set(parseRefStatement());
        break;
      case TOKEN(SUPPRESS):
        continue;
      case TOKEN(TYPE_ARRAY):
      case TOKEN(TYPE_BOOL):
      case TOKEN(TYPE_CHAR):
      case TOKEN(TYPE_DICT):
      case TOKEN(TYPE_FLOAT):
      case TOKEN(TYPE_INT):
      case TOKEN(TYPE_MATRIX):
      case TOKEN(TYPE_STRING):
      case TOKEN(TYPE_VAR):
        node.Set(parseVariableDefinition());
        break;
        
      default:
        return sl.Release();
    }
    
    if (node.IsNull() && m_success) PARSE_ERROR(INTERNAL); // Should not receive a null response without an error flag
    
    if (currentToken() == TOKEN(SUPPRESS)) {
      if (!node.IsNull()) (*node).SuppressOutput();
    } else if (currentToken() != TOKEN(ENDL)) {
      PARSE_ERROR(UNTERMINATED_EXPR);
    }

    (*sl).AddNode(node.Release());
  }
  
  if (!currentToken()) m_eof = true;
  return sl.Release();
}


cASTVariableDefinition* cParser::parseVariableDefinition()
{
  PARSE_TRACE("parseVariableDefinition");
  
  sASTypeInfo vtype(AS_TYPE_INVALID);
  switch (currentToken()) {
    case TOKEN(TYPE_ARRAY):  vtype.type = AS_TYPE_ARRAY;  break;
    case TOKEN(TYPE_BOOL):   vtype.type = AS_TYPE_BOOL;   break;
    case TOKEN(TYPE_CHAR):   vtype.type = AS_TYPE_CHAR;   break;
    case TOKEN(TYPE_DICT):   vtype.type = AS_TYPE_DICT;   break;
    case TOKEN(TYPE_FLOAT):  vtype.type = AS_TYPE_FLOAT;  break;
    case TOKEN(TYPE_INT):    vtype.type = AS_TYPE_INT;    break;
    case TOKEN(TYPE_MATRIX): vtype.type = AS_TYPE_MATRIX; break;
    case TOKEN(TYPE_STRING): vtype.type = AS_TYPE_STRING; break;
    case TOKEN(TYPE_VAR):    vtype.type = AS_TYPE_VAR;    break;
    case TOKEN(ID):
      if (peekToken() != TOKEN(REF)) {
        nextToken();
        PARSE_UNEXPECT();
      }
      
      vtype.type = AS_TYPE_OBJECT_REF;
      vtype.info = currentText();
      
      nextToken(); // consume id
      break;
      
    default:
      PARSE_UNEXPECT();
  }
  
  if (nextToken() != TOKEN(ID)) PARSE_UNEXPECT();
  
  tAutoRelease<cASTVariableDefinition> vd(new cASTVariableDefinition(FILEPOS, vtype, currentText()));
  
  switch (nextToken()) {
    case TOKEN(ASSIGN):
      nextToken();
      cASTNode* expr = parseExpression();
      (*vd).SetAssignmentExpression(expr);
      break;
    case TOKEN(PREC_OPEN):
      if (nextToken() != TOKEN(PREC_CLOSE)) (*vd).SetDimensions(parseArgumentList());
      if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
      nextToken(); // consume ')'
      break;
      
    default:
      break;
  }
  
  return vd.Release();
}

cASTVariableDefinitionList* cParser::parseVariableDefinitionList()
{
  PARSE_TRACE("parseVariableDefinitionList");
  tAutoRelease<cASTVariableDefinitionList> vl(new cASTVariableDefinitionList(FILEPOS));
 
  cASTVariableDefinition* vd = parseVariableDefinition();
  if (!vd) return NULL;
  
  (*vl).AddNode(vd);
  while (currentToken() == TOKEN(COMMA)) {
    nextToken(); // consume ','
    vd = parseVariableDefinition();
    if (!vd) return NULL;
    (*vl).AddNode(vd);
  }
  
  return vl.Release();
}

cASTNode* cParser::parseWhileStatement()
{
  PARSE_TRACE("parseWhileStatement");
  
  if (nextToken() != TOKEN(PREC_OPEN)) PARSE_UNEXPECT();
  
  nextToken();
  tAutoRelease<cASTNode> cond(parseExpression());
  if (currentToken() != TOKEN(PREC_CLOSE)) PARSE_UNEXPECT();
  nextToken();
  
  cASTNode* code = parseCodeBlock();
  return new cASTWhileBlock(FILEPOS, cond.Release(), code);
}


void cParser::reportError(ASParseError_t err, const int line)
{
#if DEBUG_AS_PARSER
# define ERR_ENDL "  (cParser.cc:" << line << ")" << std::endl
#else
# define ERR_ENDL std::endl
#endif
  
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
      break;
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
      break;
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

#undef FILEPOS

#undef TOKEN()
