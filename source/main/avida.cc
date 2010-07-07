/*
 *  avida.cc
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "defs.h"
#include "cActionLibrary.h"
#include "cAnalyzeGenotype.h"
#include "cAvidaConfig.h"
#include "cDriverManager.h"
#include "cModularityAnalysis.h"
#include "cString.h"
#include "cStringIterator.h"
#include "tDictionary.h"


#ifdef REVISION_SUPPORT
#include "revision.h"
#endif

#include <iostream>
#include <csignal>
#include <cstdio>


using namespace std;


namespace Avida {
  

const char* const BioUnitSourceMap[] = {
  "deme:compete", // SRC_DEME_COMPETE
  "deme:copy", // SRC_DEME_COPY
  "deme:germline", // SRC_DEME_GERMLINE
  "deme:replicate", // SRC_DEME_REPLICATE
  "deme:spawn", // SRC_DEME_SPAWN
  "org:compete", // SRC_ORGANISM_COMPETE
  "org:divide", // SRC_ORGANISM_DIVIDE
  "org:file_load", // SRC_ORGANISM_FILE_LOAD
  "org:random", // SRC_ORGANISM_RANDOM
  "para:file_load", // SRC_PARASITE_FILE_LOAD
  "para:inject", // SRC_PARASITE_INJECT
  "testcpu", // SRC_TEST_CPU
};
  
void Initialize()
{
  cActionLibrary::Initialize();
  cAnalyzeGenotype::Initialize();
  cModularityAnalysis::Initialize();
}
  
  

cString GetVersion()
{
  cString version("Avida ");
  version += VERSION;
#ifdef REVISION_SUPPORT
  version += " r";
  version += REVISION;
#endif
  version += " (";
  version += VERSION_TAG;
  version += ") ";
  
#ifdef COMPILE_ARCH
  version += COMPILE_ARCH " ";
#endif

#ifdef DEBUG
  version += " debug";
#endif
#if BREAKPOINTS
  version += " breakp";
#endif
#ifdef EXECUTION_ERRORS
  version += " exec_err";
#endif
#if INSTRUCTION_COSTS
  version += " inst_cost";
#endif
#if INSTRUCTION_COUNT
  version += " inst_cnt";
#endif
#ifdef ENABLE_UNIT_TESTS
  version += " ut";
#endif
#if USE_tMemTrack
  version += " memt";
#endif
  
  return version;
}

void PrintVersionBanner()
{
  // output copyright message

  cout << GetVersion() << endl << endl;
  cout << "--------------------------------------------------------------------------------" << endl;
  cout << "by Charles Ofria" << endl << endl;

  cout << "Lead Developers: David M. Bryson (Avida) and Kaben Nanlohy (Avida-ED)" << endl << endl;

  cout << "Active contributors include:  Christoph Adami, Brian Baer, Jeffrey Barrick," << endl
       << "Benjamin Beckmann, Jeffrey Clune, Brian Connelly, Art Covert, Santiago Elena," << endl
       << "Sherri Goings, Heather Goldsby, David Knoester, Richard Lenski, Philip McKinley," << endl
       << "Dusan Misevic, Elizabeth Ostrowski, Robert Pennock, Matthew Rupp, Eric Torng," << endl
       << "Michael Vo, Bess Walker, and Gabriel Yedid" << endl << endl;

  cout << "For a more complete list of contributors, see the AUTHORS file." << endl;

  cout << endl;

  cout << "Copyright (C) 1999-2009 Michigan State University." << endl;
  cout << "Copyright (C) 1993-2003 California Institute of Technology." << endl << endl;
  
  cout << "Avida comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions. See file COPYING for details." << endl << endl;

  cout << "For more information, see: http://avida.devosoft.org/" << endl;
  cout << "--------------------------------------------------------------------------------" << endl << endl;
}

void ProcessArgs(cStringList &argv, cAvidaConfig* cfg)
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
  bool crash_if_not_found = false;
  tDictionary<cString> sets;
  tDictionary<cString> defs;
  
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
      << endl;
      
      exit(0);
    }
    else if (cur_arg == "-seed" || cur_arg == "-s") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
        cerr << "Error: Must include a number as the random seed!" << endl;
        exit(0);
      } else {
        arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
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
      sets.Add(name, value);
    } else if (cur_arg == "-def") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
        cerr << "'-def' option must be followed by name and value" << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString value(cur_arg);
      defs.Add(name, value);
    } else if (cur_arg == "-c" || cur_arg == "-config") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
        cerr << "Error: Filename for configuration must be specified." << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      config_filename = cur_arg;
      crash_if_not_found = true;
    } else {
      cerr << "Error: Unknown Option '" << args[arg_num] << "'" << endl
      << "Type: \"" << args[0] << " -h\" for a full option list." << endl;
      exit(0);
    }
    
    arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
  }
  
  delete [] args;

  // Load configuration file
  cfg->Load(config_filename, defs, crash_if_not_found, flag_warn_default);
  

  // Process Command Line Flags
  if (flag_analyze) if (cfg->ANALYZE_MODE.Get() < 1) cfg->ANALYZE_MODE.Set(1);
  if (flag_interactive) if (cfg->ANALYZE_MODE.Get() < 2) cfg->ANALYZE_MODE.Set(2);
  if (flag_seed) cfg->RANDOM_SEED.Set(val_seed);
  if (flag_verbosity) cfg->VERBOSITY.Set(val_verbosity);
  
  cfg->Set(sets); // Process all command line -set statements
  
  if (sets.GetSize()) {
    tList<cString> keys;
    sets.GetKeys(keys);
    cString* keystr = NULL;
    while ((keystr = keys.Pop())) {
      cerr << "Error: Unrecognized command line configuration setting '" << *keystr << "'." << endl;
    }
    exit(1);
  }
  
  
  if (flag_review) {
    cfg->PrintReview();
    exit(0);
  }  
  
}

void ProcessCmdLineArgs(int argc, char* argv[], cAvidaConfig* cfg)
{
  cStringList sl;
  for(int i=0; i<argc; i++){
    sl.PushRear(argv[i]);
  }
  ProcessArgs(sl, cfg);
}

void Exit(int exit_code)
{
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  exit(exit_code);
}

};
