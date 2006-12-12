/*
 *  cTextViewerDriver_Base.h
 *  Avida
 *
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cTextViewerDriver_Base_h
#define cTextViewerDriver_Base_h

#ifndef cAvidaDriver_h
#include "cAvidaDriver.h"
#endif

#ifndef cWorldDriver_h
#include "cWorldDriver.h"
#endif

#include <sstream>
#include <iostream>

class cView_Base;
class cWorld;

class cTextViewerDriver_Base : public cAvidaDriver, public cWorldDriver {
protected:
  cWorld* m_world;
  cView_Base* m_view;
  bool m_done;  // This is set to true when run should finish.

  std::stringstream out_stream;
  std::stringstream err_stream;

  // Protected constructor.
  cTextViewerDriver_Base(cWorld * world);
  ~cTextViewerDriver_Base();

public:
  // Override standard output and error.
  bool IsInteractive() { return true; }
  void Flush();

  bool ProcessKeypress(int keypress);
};

#endif
