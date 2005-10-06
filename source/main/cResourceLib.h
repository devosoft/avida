//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_LIB_HH
#define RESOURCE_LIB_HH

#ifndef TARRAY_HH
#include "tArray.hh"
#endif

class cResource;
class cString;
class cResourceLib {
private:
  tArray<cResource *> resource_array;
public:
  cResourceLib() { ; }
  ~cResourceLib();

  int GetSize() const { return resource_array.GetSize(); }

  cResource * AddResource(const cString & res_name);
  cResource * GetResource(const cString & res_name) const;
  cResource * GetResource(int id) const;
};

#endif
