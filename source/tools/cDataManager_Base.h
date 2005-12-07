/*
 *  cDataManager_Base.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
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
public:
  cDataManager_Base(const cString & in_filetype) : filetype(in_filetype) { ; }
  virtual ~cDataManager_Base() { ; }
  
  const cString& GetFiletype() const { return filetype; }

  virtual bool Print(const cString& name, std::ostream& fp) = 0;
  virtual bool GetDesc(const cString& name, cString& out_desc) = 0;

  bool PrintRow(cDataFile& data_file, cString row_entries, char sep=' ');
};

#endif
