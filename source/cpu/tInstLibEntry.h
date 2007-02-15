/*
 *  cInstLibEntry.h
 *  Avida
 *
 *  Created by David Bryson on 2/14/07.
 *  Copyright 2007 Michigan State University. All rights reserved.
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

#ifndef cInstLibEntry_h
#define cInstLibEntry_h

namespace nInstFlag {
  const unsigned int DEFAULT = 0x1;
  const unsigned int NOP = 0x2;
  const unsigned int LABEL = 0x4;
}

template <class FUN> class tInstLibEntry
{
public:
  const cString name;
  const FUN function;
  const unsigned int flags;
  const cString description;
  
  bool IsDefault() const { return (flags & nInstFlag::DEFAULT); }
  bool IsNop() const { return (flags & nInstFlag::NOP); }
  bool IsLabel() const { return (flags & nInstFlag::LABEL); }
  
  tInstLibEntry(const cString& in_name, FUN in_fun, unsigned int in_flags = 0, const cString& in_desc = "")
    : name(in_name), function(in_fun), flags(in_flags), description(in_desc) { ; }
};


#endif
