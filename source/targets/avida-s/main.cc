/*
 *  main.cc
 *  Avida
 *
 *  Created by David on 1/13/06.
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

#include "avida.h"
#include "cASLibrary.h"
#include "cASTDumpVisitor.h"
#include "cFile.h"
#include "cParser.h"
#include "PlatformExpert.h"

#include <iostream>


int main (int argc, char * const argv[])
{
  PlatformExpert::Initialize();

  cASLibrary* lib = new cASLibrary;
  cParser* parser = new cParser(lib);
  
  cFile file;
  if (file.Open("main.asl")) {
    if (parser->Parse(file)) {
      std::cout << "Parse Successful\n" << std::endl;

      cASTDumpVisitor visitor;
      parser->Accept(visitor);
      
      std::cout << std::endl;
      Avida::Exit(0);
    } else {
      std::cout << "Parse Failed" << std::endl;
      Avida::Exit(1);
    }
  } else {
    std::cerr << "error: unable to open script" << std::endl;
  }
  
  return -1;
}
