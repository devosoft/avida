/*
 *  cInstLibEntry.h
 *  Avida
 *
 *  Created by David on 2/17/07.
 *  Copyright 2007-2010 Michigan State University. All rights reserved.
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
  const unsigned int PROMOTER = 0x8;
  const unsigned int STALL = 0x10;
  const unsigned int SLEEP = 0x20;
}

class cInstLibEntry
{
private:
  const cString m_name;
  const unsigned int m_flags;
  const cString m_desc;
  
  cInstLibEntry(); // @not_implemented
  
public:
  cInstLibEntry(const cString& name, unsigned int flags, const cString& desc) : m_name(name), m_flags(flags), m_desc(desc) { ; }

  const cString& GetName() const { return m_name; }
  const cString& GetDescription() const { return m_desc; }
  
  inline unsigned int GetFlags() const { return m_flags; }
  inline bool IsDefault() const { return (m_flags & nInstFlag::DEFAULT); }
  inline bool IsNop() const { return (m_flags & nInstFlag::NOP); }
  inline bool IsLabel() const { return (m_flags & nInstFlag::LABEL); }
  inline bool IsPromoter() const { return (m_flags & nInstFlag::PROMOTER); }
  inline bool ShouldStall() const { return (m_flags & nInstFlag::STALL); }
  inline bool ShouldSleep() const { return (m_flags & nInstFlag::SLEEP); }
};

#endif
