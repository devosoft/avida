/*
 *  cParser.cc
 *  Avida
 *
 *  Created by David on 1/16/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cParser.h"

#include "AvidaScript.h"

/*
 script: statement_list
 
 statement_list:	statement |	statement_list statement |
 statement: expression ENDL
          | var_declare ENDL
          | flow_command
          | declare_function
          | define_function
          | ENDL
  
 type_any: TYPE_BOOL | TYPE_CHAR | TYPE_FLOAT | TYPE_INT | TYPE_VOID
 
 expression: OP_BIT_NOT expression
           | expression OP_BIT_AND expression
           | expression OP_BIT_OR expression

           | OP_LOGIC_NOT expression
           | expression OP_LOGIC_AND expression
           | expression OP_LOGIC_OR expression

           | expression OP_ADD expression
           | expression OP_SUB expression
           | expression OP_MUL expression
           | expression OP_DIV expression
           | expression OP_MOD expression
 
           | ID ASSIGN expression

           | expression OP_EQ expression
           | expression OP_LE expression
           | expression OP_GE expression
           | expression OP_LT expression
           | expression OP_GT expression
           | expression OP_NEQ expression

           | OP_SUB expression
 
           | PREC_OPEN expression PREC_CLOSE

           | FLOAT
           | INT
           | STRING
           | CHAR
 
           | ID
           | ID IDX_OPEN expression IDX_CLOSE
           | ID PREC_OPEN argument_list PREC_CLOSE
           | expression DOT ID PREC_OPEN argument_list PREC_CLOSE
  
 argument_list: argument_list2 | 
 argument_list2: argument_list2 ',' expression |	expression 

 if_block: CMD_IF PREC_OPEN expression PREC_CLOSE ENDL statement_list CMD_ENDIF ENDL
         | CMD_IF PREC_OPEN expression PREC_CLOSE ENDL statement_list CMD_ELSE statement_list CMD_ENDIF ENDL
 
 while_block: CMD_WHILE PREC_OPEN expression PREC_CLOSE ENDL statement_list CMD_ENDWHILE ENDL
 
 foreach_block: CMD_FOREACH REF ID PREC_OPEN expression PREC_CLOSE ENDL statement_list CMD_ENDFOREACH ENDL
              | CMD_FOREACH type_any ID PREC_OPEN expression PREC_CLOSE ENDL statement_list CMD_ENDFOREACH ENDL

 var_declare: type_any ID | type_any ID ASSIGN expression
 var_declare_list: var_declare_list2 |  
 var_declare_list2: var_declare_list2 ',' var_declare | var_declare
 
 declare_function: REF CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE ENDL
 define_function: CMD_FUNCTION type_any ID PREC_OPEN var_declare_list PREC_CLOSE ENDL statement_list CMD_ENDFUNCTION
 
 */

cScriptObject* cParser::Parse(std::istream* input)
{
  m_lexer = new cLexer(input);
  
  int tok = m_lexer->yylex();
  while (tok) {
    std::cout << "Token(" << tok << "): '" << m_lexer->YYText() << "'" << endl;
    tok = m_lexer->yylex();
  }
  
  delete m_lexer;
  
  return NULL;
}
