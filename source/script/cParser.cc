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
 
 statement: expr ENDL
          | var_declare ENDL
          | if_block
          | while_block
          | foreach_block
          | declare_function
          | define_function
          | ENDL
  
 type_any: TYPE_BOOL | TYPE_CHAR | TYPE_FLOAT | TYPE_INT | TYPE_VOID
 
 expr: p0_expr expr_1

 expr_1: ASSIGN p0_expr expr_1
      |
 
 
 p0_expr: p1_expr p0_expr_1
 
 p0_expr_1: OP_LOGIC_AND p1_expr p0_expr_1
          | OP_LOGIC_OR p1_expr p0_expr_1
          |

 p1_expr: p2_expr p1_expr_1
 
 p1_expr_1: OP_BIT_AND p2_expr p1_expr_1
          | OP_BIT_OR p2_expr p1_expr_1
          |

 
 p2_expr: p3_expr p2_expr_1
 
 p2_expr_1: OP_EQ p3_expr p2_expr_1
          | OP_LE p3_expr p2_expr_1
          | OP_GE p3_expr p2_expr_1
          | OP_LT p3_expr p2_expr_1
          | OP_GT p3_expr p2_expr_1
          | OP_NEQ p3_expr p2_expr_1
          |
 
 
 p3_expr: p4_expr p3_expr_1
 
 p3_expr_1: OP_ADD p4_expr p3_expr_1
          | OP_SUB p4_expr p3_expr_1
          |

 
 p4_expr: p5_expr prec_4_expr_1
 
 p4_expr_1: OP_MUL p5_expr p4_expr_1
          | OP_DIV p5_expr p4_expr_1
          | OP_MOD p5_expr p4_expr_1
          |
 
 
 p5_expr: value p5_expr_1
        | OP_BIT_NOT expr
        | OP_LOGIC_NOT expr
        | OP_SUB expr
 
 p5_expr_1: DOT ID id_expr
          |
 
 
 value: FLOAT
      | INT
      | STRING
      | CHAR
      | ID id_expr
      | PREC_OPEN expr PREC_CLOSE
 
 id_expr: IDX_OPEN expr IDX_CLOSE
        | PREC_OPEN argument_list PREC_CLOSE
        |
 
 argument_list: argument_list_1
              |
 
 argument_list_1: expr argument_list_2
 
 argument_list_2: COMMA expr argument_list_2
                |

 if_block: CMD_IF PREC_OPEN expr PREC_CLOSE ENDL statement_list CMD_ENDIF ENDL
         | CMD_IF PREC_OPEN expr PREC_CLOSE ENDL statement_list CMD_ELSE statement_list CMD_ENDIF ENDL
 
 while_block: CMD_WHILE PREC_OPEN expr PREC_CLOSE ENDL statement_list CMD_ENDWHILE ENDL
 
 foreach_block: CMD_FOREACH REF ID PREC_OPEN expr PREC_CLOSE ENDL statement_list CMD_ENDFOREACH ENDL
              | CMD_FOREACH type_any ID PREC_OPEN expr PREC_CLOSE ENDL statement_list CMD_ENDFOREACH ENDL

 var_declare: type_any ID
            | type_any ID ASSIGN expr
 
 var_declare_list: var_declare_list_1
                 |
 
 var_declare_list_1: var_declare var_declare_list_2
 
 var_declare_list_2: COMMA var_declare var_declare_list_2
                   | 
 
 declare_function: REF CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE ENDL
 define_function: CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE ENDL statement_list CMD_ENDFUNCTION
 
 
 @TODO - return
 @TODO - array definitions
 @TODO - suppress
 @TODO - ARR_OPEN ARR_CLOSE as block open/close.  
 @TODO - function variant with block rather than endfunction.
 */

bool cParser::Parse(cFile& input)
{
  m_lexer = new cLexer(input.GetFileStream());
  
  int tok = m_lexer->yylex();
  while (tok) {
    std::cout << "Token(" << tok << "): '" << m_lexer->YYText() << "'" << endl;
    tok = m_lexer->yylex();
  }
  
  delete m_lexer;
  
  return m_success;
}


void cParser::Accept(cASTVisitor& visitor)
{
  
}
