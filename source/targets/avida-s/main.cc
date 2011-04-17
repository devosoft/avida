/*
 *  main.cc
 *  Avida
 *
 *  Created by David on 1/13/06.
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

#include "avida/Avida.h"
#include "Platform.h"

#include "ASCoreLib.h"
#include "ASAvidaLib.h"
#include "ASAnalyzeLib.h"

#include "cASLibrary.h"
#include "cDirectInterpretASTVisitor.h"
#include "cDumpASTVisitor.h"
#include "cFile.h"
#include "cParser.h"
#include "cSemanticASTVisitor.h"
#include "cSymbolTable.h"

#include <iostream>


int main (int argc, char * const argv[])
{
  Avida::Initialize();

  Avida::PrintVersionBanner();

  cASLibrary* lib = new cASLibrary;  
  RegisterASCoreLib(lib);
  RegisterASAvidaLib(lib);
  RegisterASAnalyzeLib(lib);
  
  cParser* parser = new cParser;
  
  cFile file;
  if (file.Open("main.asl")) {
    if (parser->Parse(file)) {
      cASTNode* tree = parser->ExtractTree();

      cDumpASTVisitor dump;
      tree->Accept(dump);
      std::cout << std::endl;
      
      cSymbolTable global_symtbl;
      cSemanticASTVisitor semantic_check(lib, &global_symtbl, tree);
      tree->Accept(semantic_check);
      semantic_check.PostCheck();
      
      if (!semantic_check.WasSuccessful()) {
        std::cerr << "error: semantics check failed" << std::endl;
        exit(AS_EXIT_FAIL_SEMANTIC);
      }
      
      cDirectInterpretASTVisitor interpeter(&global_symtbl);
      int exit_code = interpeter.Interpret(tree);
      
      exit(exit_code);
    } else {
      std::cerr << "error: parse failed" << std::endl;
      exit(AS_EXIT_FAIL_PARSE);
    }
  } else {
    std::cerr << "error: unable to open script" << std::endl;
    exit(AS_EXIT_FILE_NOT_FOUND);
  }
  
  return AS_EXIT_UNKNOWN;
}
