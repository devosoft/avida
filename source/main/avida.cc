/*
 *  avida.cc
 *  Avida
 *
 *  Copyright 2005-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "avida.h"

#include "cString.h"
#include "defs.h"

#ifdef REVISION_SUPPORT
#include "revision.h"
#endif

#include <iostream>
#include <signal.h>
#include <stdio.h>

using namespace std;

cString getAvidaVersion()
{
  cString version("Avida ");
  version += VERSION;
#ifdef REVISION_SUPPORT
  version += " r";
  version += REVISION;
#endif
  version += " (";
  version += VERSION_TAG;
  version += ")";

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
#if SMT_FULLY_ASSOCIATIVE
  version += " smt_fa";
#endif
#ifdef ENABLE_UNIT_TESTS
  version += " ut";
#endif
#if USE_tMemTrack
  version += " memt";
#endif
  
  return version;
}

void printVersionBanner()
{
  // output copyright message
  cout << "----------------------------------------------------------------------" << endl;
  cout << getAvidaVersion() << endl << endl;
  cout << "by Charles Ofria" << endl << endl;
  cout << "Lead Developers: David Bryson (Avida) and Kaben Nanlohy (Avida-ED)" << endl << endl;
  cout << "Portions by Brian Baer, Jeffrey Barrick, Benjamin Beckmann, Jeffrey Clune," << endl
       << "Travis Collier, Art Covert, Evan Dorn, Sherri Goings, George Hagstrom," << endl
       << "Dehua Hang, Wei Huang, David Knoester, Dusan Misevic, Elizabeth Ostrowski," << endl
       << "Matthew Rupp, Jason Stredwick, Bess Walker, Claus Wilke, and Larry Wisne" << endl << endl;

  cout << "With design help from Christoph Adami, C. Titus Brown, Timothy Cooper," << endl
       << "Santiago Elena, Richard Lenski, Philip McKinley, Robert Pennock, Thomas" << endl
       << "Schmidt, Robert Schwartz, Eric Torng, Jialan Wang, Daniel Weise, and Gabriel" << endl
       << "Yedid" << endl;

  cout << endl;

  cout << "Copyright (C) 1999-2007 Michigan State University." << endl;
  cout << "Copyright (C) 1993-2003 California Institute of Technology." << endl << endl;
  
  cout << "Avida comes with ABSOLUTELY NO WARRANTY." << endl;
  cout << "This is free software, and you are welcome to redistribute it" << endl;
  cout << "under certain conditions. See file COPYING for details." << endl << endl;

  cout << "For more information, see: http://devolab.cse.msu.edu/software/avida/" << endl;
  cout << "----------------------------------------------------------------------" << endl << endl;
}

void ExitAvida(int exit_code)
{
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  exit(exit_code);
}
