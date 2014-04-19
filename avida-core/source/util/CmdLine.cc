/*
 *  util/CmdLine.cc
 *  avida-core
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/util/CmdLine.h"

#include "apto/core/FileSystem.h"

#include <iostream>
#include <cstdio>


using namespace std;


static void processArgs(Apto::Array<Apto::String>& argv, int& seed, Apto::Map<Apto::String, Apto::String>& defs)
{
  int argc = argv.GetSize();
  int arg_num = 1;              // Argument number being looked at.
  
  
  bool flag_seed = false;         int val_seed = 0;
  
  // Then scan through and process the rest of the args.
  while (arg_num < argc) {
    Apto::String cur_arg = argv[arg_num];
    
    // Test against the possible inputs.
    
    if (cur_arg == "--help" || cur_arg == "-help" || cur_arg == "-h") {
      cout << "Options:"<<endl
      << "  -def <name> <value>   Define config include variables" << endl
      << "  -h[elp]               Help on options (this listing)"<<endl
      << "  -s[eed] <value>       Set random seed to <value>" << endl
      << "  -v[ersion]            Prints the version number" << endl
      << "  -w[arn]               Warn when default config settings are used." << endl
      << endl;
      
      exit(0);
    }
    else if (cur_arg == "-seed" || cur_arg == "-s") {
      if (arg_num + 1 == argc || argv[arg_num + 1][0] == '-') {
        cerr << "Error: Must include a number as the random seed!" << endl;
        exit(0);
      } else {
        arg_num++;  if (arg_num < argc) cur_arg = argv[arg_num];
        val_seed = Apto::StrAs(cur_arg);
      }
      flag_seed = true;
    } else if (cur_arg == "-version" || cur_arg == "-v") {
      // We've already showed version info, so just quit.
      exit(0);
    } else if (cur_arg == "-def") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
        cerr << "'-def' option must be followed by name and value" << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = argv[arg_num];
      Apto::String name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = argv[arg_num];
      Apto::String value(cur_arg);
      defs.Set(name, value);
    } else {
      cerr << "Error: Unknown Option '" << argv[arg_num] << "'" << endl
      << "Type: \"" << argv[0] << " -h\" for a full option list." << endl;
      exit(0);
    }
    
    arg_num++;  if (arg_num < argc) cur_arg = argv[arg_num];
  }
  
  
  // Process Command Line Flags
  if (flag_seed) seed = val_seed;
}
  
void Avida::Util::ProcessCmdLineArgs(int argc, char* argv[], int& seed, Apto::Map<Apto::String, Apto::String>& defs)
{
  Apto::Array<Apto::String> sl(argc);
  for (int i = 0; i < argc; i++) sl[i] = argv[i];
  processArgs(sl, seed, defs);
}
