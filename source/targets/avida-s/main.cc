/*
 *  main.cc
 *  Avida
 *
 *  Created by David on 1/13/06.
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

#include "Avida.h"
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
