/*
 *  cAnalyzeCommandDef.h
 *  Avida
 *
 *  Created by David on 12/1/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cAnalyzeCommandDef_h
#define cAnalyzeCommandDef_h

#ifndef cAnalyzeCommandDefBase_h
#include "cAnalyzeCommandDefBase.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cAnalyze;
class cAnalyzeCommand;

class cAnalyzeCommandDef : public cAnalyzeCommandDefBase {
private:
  void (cAnalyze::*CommandFunction)(cString);
public:
  cAnalyzeCommandDef(const cString & _name, void (cAnalyze::*_cf)(cString))
    : cAnalyzeCommandDefBase(_name), CommandFunction(_cf) { ; }
  ~cAnalyzeCommandDef() { ; }

  void Run(cAnalyze * analyze, const cString & args, cAnalyzeCommand & command) const
  {
    (void) command; // used in other types of command defininitions.
    (analyze->*CommandFunction)(args);
  }

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
