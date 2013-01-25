/*
 *  cDataManager_Base.h
 *  Avida
 *
 *  Called "data_manager_base.hh" prior to 12/7/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cDataManager_h
#define cDataManager_h

#include "avida/output/Types.h"

#include <iostream>

#include "cString.h"


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

  bool PrintRow(Avida::Output::File& data_file, cString row_entries, char sep=' ');
};

#endif
