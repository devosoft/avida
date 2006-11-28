/*
 *  cResourceLib.h
 *  Avida
 *
 *  Called "resource_lib.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cResourceLib_h
#define cResourceLib_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cResource;
class cString;

class cResourceLib
{
private:
  tArray<cResource*> resource_array;
  
  cResourceLib(const cResourceLib&); // @not_implemented
  cResourceLib& operator=(const cResourceLib&); // @not_implemented
  
public:
  cResourceLib() { ; }
  ~cResourceLib();

  int GetSize() const { return resource_array.GetSize(); }

  cResource* AddResource(const cString& res_name);
  cResource* GetResource(const cString& res_name) const;
  cResource* GetResource(int id) const;
  bool DoesResourceExist(const cString & res_name);
};


#ifdef ENABLE_UNIT_TESTS
namespace nResourceLib {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
