//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_FACTORY_HH
#define HARDWARE_FACTORY_HH

#ifndef TLIST_HH
#include "tList.hh"
#endif

class cHardwareBase;
class cOrganism;
class cInstSet;

class cHardwareFactory {
private:
  // A static list for each hardware type.
  static tList<cHardwareBase> hardware_cpu_list;
  static int new_count;
  static int recycle_count;
public:
  static cHardwareBase * Create(cOrganism * in_org, cInstSet * inst_set,
				int type=0);
  static void Recycle(cHardwareBase * out_hardware);
  static void Update();
};

#endif
