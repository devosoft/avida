//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cStatsScreen_h
#define cStatsScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif
class cWorld;

class cStatsScreen : public cScreen {
protected:
  cWorld* m_world;
  int task_offset;
  int task_rows;
  int task_cols;
public:
  cStatsScreen(cWorld* world, int y_size, int x_size, int y_start, int x_start, cViewInfo& in_info)
    : cScreen(y_size, x_size, y_start, x_start, in_info), m_world(world), task_offset(0)
  {
    task_rows = Height() - 16;
    task_cols = Width() / 20;
  }
  virtual ~cStatsScreen() { ; }

  // Virtual in base screen...
  void Draw(cAvidaContext& ctx);
  void Update(cAvidaContext& ctx);
  void DoInput(cAvidaContext& ctx, int in_char);
};

#endif
