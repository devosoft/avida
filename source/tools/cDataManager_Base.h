/*
 *  cDataManager_Base.h
 *  Avida
 *
 *  Called "data_manager_base.hh" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
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

  virtual bool Print(const cString& name, std::ostream& fp) const = 0;
  virtual bool GetDesc(const cString& name, cString& out_desc) const = 0;

  bool PrintRow(cDataFile& data_file, cString row_entries, char sep=' ');
};

#endif
