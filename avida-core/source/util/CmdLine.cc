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

#include "cActionLibrary.h"
#include "cAvidaConfig.h"
#include "cString.h"
#include "cStringIterator.h"
#include "cUserFeedback.h"

#include <iostream>
#include <cstdio>


using namespace std;


static void processArgs(cStringList &argv, cAvidaConfig* cfg, Apto::Map<Apto::String, Apto::String>& defs)
{
  int argc = argv.GetSize();
  int arg_num = 1;              // Argument number being looked at.
  
  // Load all of the args into string objects for ease of access.
  cString* args = new cString[argc];
  //for (int i = 0; i < argc; i++) args[i] = argv[i];
  cStringIterator list_it(argv);
  for (int i = 0; (i < argc) && (list_it.AtEnd() == false); i++) {
    list_it.Next();
    args[i] = list_it.Get();
  }
  
  cString config_filename = "avida.cfg";
  Apto::Map<Apto::String, Apto::String> sets;
  
  bool flag_analyze = false;
  bool flag_interactive = false;
  bool flag_review = false;
  bool flag_verbosity = false;    int val_verbosity = 0;
  bool flag_seed = false;         int val_seed = 0;
  bool flag_warn_default = false;
  
  // Then scan through and process the rest of the args.
  while (arg_num < argc) {
    cString cur_arg = args[arg_num];
    
    // Test against the possible inputs.
    
    // Print out a list of all possibel actions (was events).
    if (cur_arg == "-e" || cur_arg == "-events" || cur_arg == "-actions") {
      cout << endl << "Supported Actions:" << endl;
      cout << cActionLibrary::GetInstance().DescribeAll() << endl;
      exit(0);
    }
    
    // Review configuration options, listing those non-default.
    else if (cur_arg == "-review" || cur_arg == "-r") {
      flag_review = true;
    }
    
    else if (cur_arg == "--help" || cur_arg == "-help" || cur_arg == "-h") {
      cout << "Options:"<<endl
      << "  -a[nalyze]            Process analyze.cfg instead of normal "
      << "run." << endl
      << "  -c[onfig] <filename>  Set config file to be <filename>"<<endl
      << "  -def <name> <value>   Define config include variables" << endl
      << "  -e; -actions          Print a list of all known actions"<< endl
      << "  -h[elp]               Help on options (this listing)"<<endl
      << "  -i[nteractive]        Run analyze mode interactively" << endl
      << "  -l[oad] <filename>    Load a clone file" << endl
      << "  -r[eview]             Review avida.cfg settings." << endl
      << "  -s[eed] <value>       Set random seed to <value>" << endl
      << "  -set <name> <value>   Override values in avida.cfg" << endl
      << "  -v[ersion]            Prints the version number" << endl
      << "  -v0 -v1 -v2 -v3 -v4   Set output verbosity to 0..4" << endl
      << "  -w[arn]               Warn when default config settings are used." << endl
      << "  --generate-config     Generate the default configration files" << endl
      << endl;
      
      exit(0);
    }
    else if (cur_arg == "-seed" || cur_arg == "-s") {
      if (arg_num + 1 == argc) {
        cerr << "Error: Must include a number as the random seed!" << endl;
        exit(0);
      } else {
        arg_num++;
        if (arg_num < argc) cur_arg = args[arg_num];
        val_seed = cur_arg.AsInt();
      }
      flag_seed = true;
    } else if (cur_arg == "-analyze" || cur_arg == "-a") {
      flag_analyze = true;
    } else if (cur_arg == "-interactive" || cur_arg == "-i") {
      flag_interactive = true;
    } else if (cur_arg == "-warn" || cur_arg == "-w") {
      flag_warn_default = true;
    } else if (cur_arg == "-version" || cur_arg == "-v") {
      // We've already showed version info, so just quit.
      exit(0);
    } else if (cur_arg.Substring(0, 2) == "-v") {
      val_verbosity = cur_arg.Substring(2, cur_arg.GetSize() - 2).AsInt();
      flag_verbosity = true;
    } else if (cur_arg == "-set") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
        cerr << "'-set' option must be followed by name and value" << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString value(cur_arg);
      sets.Set((const char*)name, (const char*)value);
    } else if (cur_arg == "-def") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
        cerr << "'-def' option must be followed by name and value" << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString value(cur_arg);
      defs.Set((const char*)name, (const char*)value);
    } else if (cur_arg == "-c" || cur_arg == "-config") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
        cerr << "Error: Filename for configuration must be specified." << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      config_filename = cur_arg;
    } else if (cur_arg == "--generate-config") {
      cerr << "Generating default avida.cfg" << endl;
      cfg->Print(cString(Apto::FileSystem::PathAppend(Apto::FileSystem::GetCWD(), "avida.cfg")));
      exit(0);
    } else {
      cerr << "Error: Unknown Option '" << args[arg_num] << "'" << endl
      << "Type: \"" << args[0] << " -h\" for a full option list." << endl;
      exit(0);
    }
    
    arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
  }
  
  delete [] args;
  
  // Load configuration file
  cUserFeedback feedback;
  cfg->Load(config_filename, cString(Apto::FileSystem::GetCWD()), &feedback, &defs, flag_warn_default);
  for (int i = 0; i < feedback.GetNumMessages(); i++) {
    switch (feedback.GetMessageType(i)) {
      case cUserFeedback::UF_ERROR:    cerr << "error: "; break;
      case cUserFeedback::UF_WARNING:  cerr << "warning: "; break;
      default: break;
    };
    cerr << feedback.GetMessage(i) << endl;
  }
  if (feedback.GetNumErrors()) exit(-1);
  
  
  // Process Command Line Flags
  if (flag_analyze) if (cfg->ANALYZE_MODE.Get() < 1) cfg->ANALYZE_MODE.Set(1);
  if (flag_interactive) if (cfg->ANALYZE_MODE.Get() < 2) cfg->ANALYZE_MODE.Set(2);
  if (flag_seed) cfg->RANDOM_SEED.Set(val_seed);
  if (flag_verbosity) cfg->VERBOSITY.Set(val_verbosity);
  
  cfg->Set(sets); // Process all command line -set statements
  
  if (sets.GetSize()) {
    for (Apto::Map<Apto::String, Apto::String>::KeyIterator kit = sets.Keys(); kit.Next();) {
      cerr << "error: unrecognized command line configuration setting '" << *kit.Get() << "'." << endl;
    }
    exit(1);
  }
  
  
  if (flag_review) {
    cfg->PrintReview();
    exit(0);
  }
}
  
void Avida::Util::ProcessCmdLineArgs(int argc, char* argv[], cAvidaConfig* cfg, Apto::Map<Apto::String, Apto::String>& defs)
{
  cStringList sl;
  for(int i=0; i<argc; i++){
    sl.PushRear(argv[i]);
  }
  processArgs(sl, cfg, defs);
}
