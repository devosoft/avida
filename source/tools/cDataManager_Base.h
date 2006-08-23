/*
 *  cDataManager_Base.h
 *  Avida
 *
 *  Called "data_manager_base.hh" prior to 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cDataManager_h
#define cDataManager_h

#include <iostream>

#ifndef cString_h
#include "cString.h"
#endif

class cDataFile;

class cDataManager_Base {
private:
  cString filetype;
  
  
  cDataManager_Base(); // @not_implemented
  cDataManager_Base(const cDataManager_Base&); // @not_implemented
  cDataManager_Base& operator=(const cDataManager_Base&); // @not_implemented

public:
  cDataManager_Base(const cString & in_filetype) : filetype(in_filetype) { ; }
  virtual ~cDataManager_Base() { ; }
  
  const cString& GetFiletype() const { return filetype; }

  virtual bool Print(const cString& name, std::ostream& fp) = 0;
  virtual bool GetDesc(const cString& name, cString& out_desc) = 0;

  bool PrintRow(cDataFile& data_file, cString row_entries, char sep=' ');
};


#ifdef ENABLE_UNIT_TESTS
namespace nDataManager_Base {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
