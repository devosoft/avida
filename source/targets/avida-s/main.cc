/*
 *  main.cc
 *  avida_test_language
 *
 *  Created by David on 1/13/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cASLibrary.h"
#include "cParser.h"

#include <iostream>


int main (int argc, char * const argv[])
{
  cASLibrary* lib = new cASLibrary;
  cParser* parser = new cParser(lib);
  parser->Parse(&std::cin);
  
  return 0;
}
