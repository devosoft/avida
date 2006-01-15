/*
 *  main.cc
 *  avida_test_language
 *
 *  Created by David on 1/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cLexer.h"
#include <iostream>

int main (int argc, char * const argv[])
{
  cLexer* lexer = new cLexer;
  
  int tok = lexer->yylex();
  while (tok) {
    std::cout << "Token(" << tok << "): '" << lexer->YYText() << "'" << endl;
    tok = lexer->yylex();
  }
  return 0;
}
