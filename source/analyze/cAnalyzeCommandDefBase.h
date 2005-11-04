//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_COMMAND_DEF_BASE_HH
#define ANALYZE_COMMAND_DEF_BASE_HH

#ifndef STRING_HH
#include "cString.h"
#endif

class cString; // aggregate
class cAnalyze;
class cAnalyzeCommand;

class cAnalyzeCommandDefBase {
protected:
  cString name;
public:
  cAnalyzeCommandDefBase(const cString & _name) : name(_name) { ; }
  virtual ~cAnalyzeCommandDefBase() { ; }

  virtual void Run(cAnalyze * analyze, const cString & args,
		   cAnalyzeCommand & command) const = 0;
  virtual bool IsFlowCommand() { return false; }

  const cString & GetName() const { return name; }
};

#endif
