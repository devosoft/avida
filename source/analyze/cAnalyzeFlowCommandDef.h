/*
 *  cAnalyzeFlowCommandDef.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeFlowCommandDef_h
#define cAnalyzeFlowCommandDef_h

#ifndef cAnalyzeCommand_h
#include "cAnalyzeCommand.h"
#endif
#ifndef cAnalyzeCommandDefBase_h
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cAnalyze;

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
