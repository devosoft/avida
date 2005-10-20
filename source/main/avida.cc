//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "avida.h"

#include "cAvidaDriver_Base.h"
#include "cString.h"
#include "defs.h"

#ifdef REVISION_SUPPORT
#include "revision.h"
#endif

#include <signal.h>
#include <stdio.h>

using namespace std;

cString AvidaVersion()
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
#ifdef BREAKPOINTS
  version += " breakp";
#endif
#ifdef EXECUTION_ERRORS
  version += " exec_err";
#endif
#ifdef INSTRUCTION_COSTS
  version += " inst_cost";
#endif
#ifdef INSTRUCTION_COUNT
  version += " inst_cnt";
#endif
#ifdef SMT_FULLY_ASSOCIATIVE
  version += " smt_fa";
#endif
  
  return version;
}

void ExitAvida(int exit_code)
{
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  delete cAvidaDriver_Base::main_driver;
  exit(exit_code);
}
