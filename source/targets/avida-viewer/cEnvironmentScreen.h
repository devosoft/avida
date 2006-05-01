//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef cEnvironmentScreen_h
#define cEnvironmentScreen_h

#ifndef cScreen_h
#include "cScreen.h"
#endif

class cWorld;

class cEnvironmentScreen : public cScreen {
protected:
  cWorld* m_world;
  int mode;
  int res_selection;
  int rxn_selection;

  static const int ENVIRONMENT_MODE_RESOURCE = 0;
  static const int ENVIRONMENT_MODE_REACTION = 1;

public:
  cEnvironmentScreen(cWorld* world, int y_size, int x_size, int y_start, int x_start, cViewInfo& in_info)
    : cScreen(y_size, x_size, y_start, x_start, in_info), m_world(world)
  {
    mode=ENVIRONMENT_MODE_RESOURCE;
    res_selection = rxn_selection = 0;
  }
  virtual ~cEnvironmentScreen() { ; }

  // Virtual in base screen...
  void Draw();
  void Update();
  void DoInput(int in_char);

  void DrawResource();
  void DrawReaction();
  void UpdateResource();
  void UpdateReaction();

public:
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  static void UnitTests(bool full = false);
  
};

#endif
