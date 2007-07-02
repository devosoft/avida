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
 
 p0_expr: p1_expr p6_expr_1
 
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
          | OP_NEQ p4_expr p4_expr_1
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
          |
  
 value: FLOAT
      | INT
      | STRING
      | CHAR
      | ID id_expr
      | PREC_OPEN expr PREC_CLOSE
      | ARR_OPEN argument_list ARR_CLOSE
      | MAT_MODIFY ARR_OPEN argument_list ARR_CLOSE
 
 id_expr: IDX_OPEN expr IDX_CLOSE
        | PREC_OPEN argument_list PREC_CLOSE
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
         | CMD_IF PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ELSE statement_list CMD_ENDIF
 
 while_block: CMD_WHILE PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ENDWHILE
            | CMD_WHILE PREC_OPEN expr PREC_CLOSE loose_block
 
 foreach_block: CMD_FOREACH type_def ID PREC_OPEN expr PREC_CLOSE lineterm statement_list CMD_ENDFOREACH
              | CMD_FOREACH type_def ID PREC_OPEN expr PREC_CLOSE loose_block

 var_declare: type_def ID
            | type_def ID ASSIGN expr
            | type_def ID PREC_OPEN expr PREC_CLOSE
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

#define PARSE_ERROR(x) reportError(AS_PARSE_ERR_ ## x, __LINE__);
#define PARSE_UNEXPECT() { if (currentToken()) { PARSE_ERROR(UNEXPECTED_TOKEN); } else { PARSE_ERROR(EOF); } }

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

cASTNode* cParser::parseAssignment()
{
  cASTNode* an = NULL;
  
  nextToken();
  parseExpression();
  
  return an;
}

cASTNode* cParser::parseCallExpression()
{
  // @todo
  return NULL;
}

cASTNode* cParser::parseExpression()
{
  // @todo
  return NULL;
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
  
  nextToken();
  if (currentToken() == ARR_OPEN) {
    parseLooseBlock();
  } else if (currentToken() == SUPPRESS || currentToken() == ENDL) {
    parseStatementList();
    if (currentToken() != CMD_ENDFOREACH) {
      PARSE_UNEXPECT();
      return fs;
    }
  } else {
    PARSE_UNEXPECT();
    return fs;
  }
  
  return fs;
}

cASTNode* cParser::parseFunctionDeclare()
{
  // @todo
  return NULL;
}

cASTNode* cParser::parseFunctionDefine()
{
  // @todo
  return NULL;
}

cASTNode* cParser::parseIfStatement()
{
  // @todo
  return NULL;
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
      parseFunctionDeclare();
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
  // @todo
  return NULL;
}

cASTNode* cParser::parseWhileStatement()
{
  // @todo
  return NULL;
}


bool cParser::checkLineTerm(cASTNode* node)
{
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
