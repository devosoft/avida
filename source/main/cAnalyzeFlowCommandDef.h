//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZE_FLOW_COMMAND_DEF_HH
#define ANALYZE_FLOW_COMMAND_DEF_HH

#ifndef ANALYZE_COMMAND_HH
#include "cAnalyzeCommand.h"
#endif
#ifndef ANALYZE_COMMAND_DEF_BASE_HH
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef STRING_HH
#include "cString.h"
#endif
#ifndef TLIST_HH
#include "tList.hh"
#endif

class cAnalyze;
template <class T> class tList;
class cAnalyzeCommand; // access
class cString; // aggregate

class cAnalyzeFlowCommandDef : public cAnalyzeCommandDefBase {
private:
  void (cAnalyze::*CommandFunction)(cString, tList<cAnalyzeCommand> &);
public:
  cAnalyzeFlowCommandDef(const cString &_name,
	 void (cAnalyze::*_cf)(cString, tList<cAnalyzeCommand> &))
    : cAnalyzeCommandDefBase(_name), CommandFunction(_cf) { ; }
  virtual ~cAnalyzeFlowCommandDef() { ; }
			 
  virtual void Run(cAnalyze * analyze, const cString & args,
		   cAnalyzeCommand & command) const {
    (analyze->*CommandFunction)(args, *(command.GetCommandList()) );
  }

  virtual bool IsFlowCommand() { return true; }
};

#endif
