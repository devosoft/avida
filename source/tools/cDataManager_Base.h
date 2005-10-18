//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DATA_MANAGER_BASE_HH
#define DATA_MANAGER_BASE_HH

#include <iostream>

#ifndef STRING_HH
#include "cString.h"
#endif

class cDataFile;
class cString; // aggregate

class cDataManager_Base {
private:
  cString filetype;
public:
  cDataManager_Base(const cString & in_filetype) : filetype(in_filetype) { ; }
  virtual ~cDataManager_Base();
  
  const cString & GetFiletype() const { return filetype; }

  virtual bool Print(const cString & name, std::ostream & fp) = 0;
  virtual bool GetDesc(const cString & name, cString & out_desc) = 0;

  bool PrintRow(cDataFile & data_file, cString row_entries, char sep=' ');
};

#endif
