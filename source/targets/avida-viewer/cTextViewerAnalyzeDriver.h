/*
 *  cTextViewerAnalyzeDriver.h
 *  Avida
 *
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cTextViewerAnalyzeDriver_h
#define cTextViewerAnalyzeDriver_h

#ifndef cTextViewerDriver_Base_h
#include "cTextViewerDriver_Base.h"
#endif

class cAnalyzeView;

class cTextViewerAnalyzeDriver : public cTextViewerDriver_Base
{
private:
  cTextViewerAnalyzeDriver(); // @not_implemented
  cTextViewerAnalyzeDriver(const cTextViewerAnalyzeDriver&); // @not_implemented
  cTextViewerAnalyzeDriver& operator=(const cTextViewerAnalyzeDriver&); // @not_implemented
  
protected:
  bool m_interactive;
  
public:
  cTextViewerAnalyzeDriver(cWorld* world, bool inter = false);
  ~cTextViewerAnalyzeDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { m_done = true; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
};


#ifdef ENABLE_UNIT_TESTS
namespace nTextViewerAnalyzeDriver {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
