/*
 *  AvidaScript.h
 *  avida_test_language
 *
 *  Created by David on 1/14/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef AvidaScript_h
#define AvidaScript_h

enum eASTokens {
  SUPRESS = 1,
  ENDL,
  
  OP_BIT_NOT,
  OP_BIT_AND,
  OP_BIT_OR,
  
  OP_LOGIC_NOT,
  OP_LOGIC_AND,
  OP_LOGIC_OR,
  
  OP_ADD,
  OP_SUB,
  OP_MUL,
  OP_DIV,
  OP_MOD,
  
  DOT,
  ASSIGN,
  
  OP_EQ,
  OP_LE,
  OP_GE,
  OP_LT,
  OP_GT,
  OP_NEQ,
  
  PREC_OPEN,
  PREC_CLOSE,
  
  IDX_OPEN,
  IDX_CLOSE,
  
  ARR_OPEN,
  ARR_CLOSE,
  ARR_RANGE,
  ARR_EXPAN,
  
  TYPE_BOOL,
  TYPE_CHAR,
  TYPE_FLOAT,
  TYPE_INT,
  TYPE_VOID,
  
  CMD_IF,
  CMD_ELSE,
  CMD_ENDIF,
  
  CMD_WHILE,
  CMD_ENDWHILE,
  
  CMD_FOREACH,
  CMD_ENDFOREACH,
  
  ID,
  
  FLOAT,
  INT,
  STRING,
  CHAR,
  
  ERR
};

#endif
