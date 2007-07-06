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

#define PARSE_ERROR(x) reportError(AS_PARSE_ERR_ ## x, __LINE__)
#define PARSE_UNEXPECT() { if (currentToken()) { PARSE_ERROR(UNEXPECTED_TOKEN); } else { PARSE_ERROR(EOF); } }


cParser::cParser(cASLibrary* library)
: m_library(library)
, m_eof(false)
, m_success(true)
, m_cur_tok(INVALID)
, m_next_tok(INVALID)
, m_err_eof(false)
{
}

bool cParser::Parse(cFile& input)
{
  m_lexer = new cLexer(input.GetFileStream());
  parseStatementList();
  delete m_lexer;
  
  return m_success;
}

void cParser::Accept(cASTVisitor& visitor)
{
  
}

ASToken_t cParser::nextToken()
{
  if (m_next_tok != INVALID) { 
    m_cur_tok = m_next_tok;
    m_next_tok = INVALID;
  } else {
    m_cur_tok = (ASToken_t)m_lexer->yylex();
  }
  return m_cur_tok;
}


cASTNode* cParser::parseArrayUnpack()
{
  cASTNode* au = NULL;
  
  if (nextToken() != ID) {
    PARSE_ERROR(UNEXPECTED_TOKEN);
    return au;
  }
  
  while (nextToken()) {
    if (currentToken() == COMMA) {
      nextToken();
      if (currentToken() == ID) {
        continue;
      } else if (currentToken() == ARR_WILD) {
        break;
      } else {
        PARSE_ERROR(UNEXPECTED_TOKEN);
        break;
      }
    } else if (currentToken() == ARR_WILD) {
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
  cASTNode* al = NULL;
  
  parseExpression();
  while (currentToken() == COMMA) {
    parseExpression();
  }
  
  return al;
}

cASTNode* cParser::parseAssignment()
{
  cASTNode* an = NULL;
  
  nextToken();
  parseExpression();
  
  return an;
}

cASTNode* cParser::parseCallExpression()
{
  cASTNode* ce = NULL;
  
  bool eoe = false;
  while (!eoe) {
    switch (currentToken()) {
      case DOT:
        if (nextToken() != ID) {
          PARSE_UNEXPECT();
          return ce;
        }
        break;
      case PREC_OPEN:
        nextToken();
        parseArgumentList();
        if (nextToken() != PREC_CLOSE) {
          PARSE_UNEXPECT();
          return ce;   
        }
        switch (nextToken()) {
          case IDX_OPEN:
            do {
              parseIndexExpression();
            } while (nextToken() == IDX_OPEN);
            break;
          case DOT:
            continue;

          default:
            eoe = true;
        }
        break;
      case IDX_OPEN:
        do {
          parseIndexExpression();
        } while (nextToken() == IDX_OPEN);

      default:
        PARSE_UNEXPECT();
        return ce;
    }
  }
    
  return ce;
}

cASTNode* cParser::parseCodeBlock(bool& loose)
{
  cASTNode* cb = NULL;

  nextToken();
  if (currentToken() == ARR_OPEN) {
    loose = true;
    cb = parseLooseBlock();
  } else if (currentToken() == SUPPRESS || currentToken() == ENDL) {
    cb = parseStatementList();
  } else {
    PARSE_UNEXPECT();
    return cb;
  }
  
  return cb;  
}

cASTNode* cParser::parseExpression()
{
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
  cASTNode* l = parseExprP1();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case ARR_RANGE:
      case ARR_EXPAN:
        nextToken();
        r = parseExprP1();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP1()
{
  cASTNode* l = parseExprP2();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case OP_LOGIC_AND:
      case OP_LOGIC_OR:
        nextToken();
        r = parseExprP2();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP2()
{
  cASTNode* l = parseExprP3();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case OP_BIT_AND:
      case OP_BIT_OR:
        nextToken();
        r = parseExprP3();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP3()
{
  cASTNode* l = parseExprP4();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case OP_EQ:
      case OP_LE:
      case OP_GE:
      case OP_LT:
      case OP_GT:
      case OP_NEQ:
        nextToken();
        r = parseExprP4();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP4()
{
  cASTNode* l = parseExprP5();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case OP_ADD:
      case OP_SUB:
        nextToken();
        r = parseExprP5();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP5()
{
  cASTNode* l = parseExprP6();
  cASTNode* r = NULL;
  
  while(true) {
    switch (currentToken()) {
      case OP_MUL:
      case OP_DIV:
      case OP_MOD:
        nextToken();
        r = parseExprP6();
        // set l == new expr
        break;
        
      default:
        return l;
    }
  }
  
  return l;
}

cASTNode* cParser::parseExprP6()
{
  cASTNode* expr = NULL;
  
  switch (currentToken()) {
    case FLOAT:
    case INT:
    case CHAR:
      // @todo - expr = ;
      break;
    case STRING:
      // @todo - expr = ;
      break;
    case ID:
      if (peekToken() == PREC_OPEN) {
        nextToken();
        nextToken();
        parseArgumentList();
        if (currentToken() != PREC_CLOSE) {
          PARSE_UNEXPECT();
          return expr;
        }
        // @todo - expr = ;
      } else {
        // @todo - expr = ;
      }
      break;
    case PREC_OPEN:
      nextToken();
      expr = parseExpression();
      if (currentToken() != PREC_CLOSE) {
        PARSE_UNEXPECT();
        return expr;
      }
      break;
    case MAT_MODIFY:
      if (nextToken() != ARR_OPEN) {
        PARSE_UNEXPECT();
        return expr;
      }
    case ARR_OPEN:
      nextToken();
      parseArgumentList();
      if (currentToken() != ARR_CLOSE) {
        PARSE_UNEXPECT();
        return expr;
      }
      break;
      
    case OP_BIT_NOT:
    case OP_LOGIC_NOT:
    case OP_SUB:
      expr = parseExpression();
      nextToken();
      return expr;
      
    default:
      break;
  }

  if (expr) expr = parseExprP6_Index(expr);
  return expr;
}

cASTNode* cParser::parseExprP6_Index(cASTNode* l)
{
  while (currentToken() == DOT || currentToken() == IDX_OPEN) {
    if (currentToken() == DOT) {
      if (nextToken() != ID) {
        PARSE_UNEXPECT();
        return l;
      }
      if (peekToken() == PREC_OPEN) {
        nextToken();
        nextToken();
        parseArgumentList();
        if (currentToken() != PREC_CLOSE) {
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
      if (currentToken() != IDX_CLOSE) {
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
  cASTNode* fs = NULL;
  
  switch (nextToken()) {
    case TYPE_ARRAY:
    case TYPE_CHAR:
    case TYPE_FLOAT:
    case TYPE_INT:
    case TYPE_MATRIX:
    case TYPE_STRING:
      break;
    case ID:
      if (nextToken() != REF) {
        PARSE_UNEXPECT();
        return fs;
      }
      break;
      
    default:
      PARSE_UNEXPECT();
      return fs;
  }
  
  if (nextToken() != PREC_OPEN) {
    PARSE_UNEXPECT();
    return fs;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != PREC_CLOSE) {
    PARSE_UNEXPECT();
    return fs;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (!loose && currentToken() != CMD_ENDFOREACH) PARSE_UNEXPECT();
  
  return fs;
}

cASTNode* cParser::parseFunctionDefine()
{
  cASTNode* fd = parseFunctionHeader(false);
  
  bool loose = false;
  parseCodeBlock(loose);
  if (!loose && currentToken() != CMD_ENDFUNCTION) {
    PARSE_UNEXPECT();
    return fd;
  }
  
  return fd;
}

cASTNode* cParser::parseFunctionHeader(bool declare)
{
  cASTNode* fd = NULL;
  
  switch (nextToken()) {
    case TYPE_ARRAY:
    case TYPE_CHAR:
    case TYPE_FLOAT:
    case TYPE_INT:
    case TYPE_MATRIX:
    case TYPE_STRING:
    case TYPE_VOID:
      break;
    case ID:
      if (peekToken() != REF) {
        nextToken();
        PARSE_UNEXPECT();
        return fd;
      }
      break;
      
    default:
      PARSE_UNEXPECT();
      return fd;
  }
  
  if (nextToken() != ID) {
    PARSE_UNEXPECT();
    return fd;
  }
  
  if (nextToken() != PREC_OPEN) {
    PARSE_UNEXPECT();
    return fd;
  }
  
  nextToken();
  if (declare) {
    parseVarDeclareList();
  } else {
    parseArgumentList();
  }
  
  if (nextToken() != PREC_CLOSE) {
    PARSE_UNEXPECT();
    return fd;    
  }
  
  return fd;
}

cASTNode* cParser::parseIDStatement()
{
  cASTNode* is = NULL;
  
  switch (nextToken()) {
    case ASSIGN:
      parseAssignment();
      break;
    case DOT:
    case IDX_OPEN:
    case PREC_OPEN:
      parseCallExpression();
      break;
    case REF:
      parseVarDeclare();
      break;
      
    default:
      PARSE_UNEXPECT();
      break;
  }      
  
  return is;
}

cASTNode* cParser::parseIfStatement()
{
  cASTNode* is = NULL;
  
  if (nextToken() != PREC_OPEN) {
    PARSE_UNEXPECT();
    return is;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != PREC_CLOSE) {
    PARSE_UNEXPECT();
    return is;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (currentToken() == CMD_ELSE) {
    parseCodeBlock(loose);
    if (!loose && currentToken() != CMD_ENDIF) {
      PARSE_UNEXPECT();
      return is;
    }
  } else if (!loose && currentToken() != CMD_ENDIF) {
    PARSE_UNEXPECT();
    return is;
  }
  
  return is;
}

cASTNode* cParser::parseIndexExpression()
{
  cASTNode* ie = NULL;
  
  nextToken();
  parseExpression();
  if (currentToken() != IDX_CLOSE) {
    PARSE_UNEXPECT();
  }
  
  return ie;
}

cASTNode* cParser::parseLooseBlock()
{
  nextToken();
  cASTNode* sl = parseStatementList();
  if (currentToken() != ARR_CLOSE) {
    PARSE_UNEXPECT();
  }
  return sl;
}

cASTNode* cParser::parseRefStatement()
{
  cASTNode* rs = NULL;

  switch (nextToken()) {
    case ARR_OPEN:
      parseArrayUnpack();
      break;
    case CMD_FUNCTION:
      parseFunctionHeader();
      break;
    default:
      PARSE_UNEXPECT();
  }
  
  return rs;
}

cASTNode* cParser::parseReturnStatement()
{
  cASTNode* rs = NULL;
  
  nextToken();
  parseExpression();
  
  return rs;
}

cASTNode* cParser::parseStatementList()
{
  cASTNode* sl = NULL;

#define CHECK_LINETERM() { if (!checkLineTerm(sl)) return sl; }
  while (nextToken()) {
    switch (currentToken()) {
      case ARR_OPEN:
        parseLooseBlock();
        CHECK_LINETERM();
        break;
      case CMD_IF:
        parseIfStatement();
        CHECK_LINETERM();
        break;
      case CMD_FOREACH:
        parseForeachStatement();
        CHECK_LINETERM();
        break;
      case CMD_FUNCTION:
        parseFunctionDefine();
        CHECK_LINETERM();
        break;
      case CMD_RETURN:
        parseReturnStatement();
        CHECK_LINETERM();
        break;
      case CMD_WHILE:
        parseWhileStatement();
        CHECK_LINETERM();
        break;
      case ENDL:
        break;
      case ID:
        parseIDStatement();
        CHECK_LINETERM();
        break;
      case REF:
        parseRefStatement();
        CHECK_LINETERM();
        break;
      case SUPPRESS:
        break;
      case TYPE_ARRAY:
      case TYPE_CHAR:
      case TYPE_FLOAT:
      case TYPE_INT:
      case TYPE_MATRIX:
      case TYPE_STRING:
        parseVarDeclare();
        CHECK_LINETERM();
        break;
        
      default:
        return sl;
    }
  }
#undef CHECK_LINETERM()
  
  if (!currentToken()) m_eof = true;
  return sl;
}

cASTNode* cParser::parseVarDeclare()
{
  cASTNode* vd = NULL;
  
  switch (currentToken()) {
    case TYPE_ARRAY:
    case TYPE_CHAR:
    case TYPE_FLOAT:
    case TYPE_INT:
    case TYPE_MATRIX:
    case TYPE_STRING:
      break;
    case ID:
      if (nextToken() != REF) {
        PARSE_UNEXPECT();
        return vd;
      }
      break;
      
    default:
      PARSE_UNEXPECT();
      return vd;
  }
  
  if (nextToken() != ID) {
    PARSE_UNEXPECT();
    return vd;
  }
  
  switch (nextToken()) {
    case ASSIGN:
      nextToken();
      parseExpression();
      break;
    case PREC_OPEN:
      nextToken();
      parseArgumentList();
      if (currentToken() != PREC_CLOSE) {
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
  cASTNode* vl = NULL;
  
  parseVarDeclare();
  while (currentToken() == COMMA) {
    parseVarDeclare();
  }
  
  return vl;
}

cASTNode* cParser::parseWhileStatement()
{
  cASTNode* ws = NULL;
  
  if (nextToken() != PREC_OPEN) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  nextToken();
  parseExpression();
  
  if (currentToken() != PREC_CLOSE) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  bool loose = false;
  parseCodeBlock(loose);
  if (!loose && currentToken() != CMD_ENDWHILE) {
    PARSE_UNEXPECT();
    return ws;
  }
  
  return ws;
}


bool cParser::checkLineTerm(cASTNode* node)
{
  nextToken();
  if (currentToken() == SUPPRESS) {
    // @todo - mark output as suppressed
    return true;
  } else if (currentToken() == ENDL) {
    return true;
  }
  
  PARSE_ERROR(UNTERMINATED_EXPR);
  return false;
}


void cParser::reportError(ASParseError_t err, const int line)
{
  m_success = false;

  std::cerr << "error: ";

  switch (err) {
    case AS_PARSE_ERR_UNEXPECTED_TOKEN:
      std::cerr << "unexpected token '" << currentToken() << "'." << std::endl;
      break;
    case AS_PARSE_ERR_UNTERMINATED_EXPR:
      std::cerr << "unterminated expression'" << currentToken() << "'." << std::endl;
      break;
    case AS_PARSE_ERR_NULL_EXPR:
      std::cerr << "expected expression, found '" << currentToken() << "'." << std::endl;
    case AS_PARSE_ERR_EOF:
      if (!m_err_eof) {
        std::cerr << "unexpected end of file" << std::endl;
        m_err_eof = true;
      }
      break;
    case AS_PARSE_ERR_INTERNAL:
      std::cerr << "internal parser error at cParser.cc:" << line << std::endl;
    case AS_PARSE_ERR_UNKNOWN:
    default:
      std::cerr << "parse error" << std::endl;
  }
}
