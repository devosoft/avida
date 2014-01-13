//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cBarScreen_h
#define cBarScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif
#ifndef cString_h
#include "cString.h"
#endif

class cBarScreen : public cScreen {
protected:
  cWorld* m_world;
  cString prog_name;

  int AddMenuOption(const cString option, int max_x, int cur_x);
public:
  cBarScreen(cWorld* world, int y_size, int x_size, int y_start, int x_start, cViewInfo & in_info, const cString & in_name)
    : cScreen(y_size, x_size, y_start, x_start, in_info), m_world(world), prog_name(in_name) { ; }
  ~cBarScreen() { ; }

  // Virtual in base screen...
  void Draw(cAvidaContext& ctx);
  void Update(cAvidaContext& ctx);
  void DoInput(cAvidaContext& ctx, int in_char) { (void) in_char; }
};

#endif
