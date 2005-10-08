//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_COMMAND_DEF_HH
#define ANALYZE_COMMAND_DEF_HH

#ifndef ANALYZE_COMMAND_DEF_BASE_HH
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif

class cAnalyze;
class cString; // aggregate
class cAnalyzeCommand;

class cAnalyzeCommandDef : public cAnalyzeCommandDefBase {
private:
  void (cAnalyze::*CommandFunction)(cString);
public:
  cAnalyzeCommandDef(const cString & _name, void (cAnalyze::*_cf)(cString))
    : cAnalyzeCommandDefBase(_name), CommandFunction(_cf) { ; }
  virtual ~cAnalyzeCommandDef() { ; }

  virtual void Run(cAnalyze * analyze, const cString & args,
		   cAnalyzeCommand & command) const {
    (void) command; // used in other types of command defininitions.
    (analyze->*CommandFunction)(args);
  }
};

#endif
