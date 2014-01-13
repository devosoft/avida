/*
 *  cInstLibEntry.h
 *  Avida
 *
 *  Created by David on 2/17/07.
 *  Copyright 2007-2011 Michigan State University. All rights reserved.
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

#ifndef cInstLibEntry_h
#define cInstLibEntry_h


namespace nInstFlag {
  const unsigned int DEFAULT = 0x1;
  const unsigned int NOP = 0x2;
  const unsigned int LABEL = 0x4;
  const unsigned int STALL = 0x8;
  const unsigned int SLEEP = 0x10;
  const unsigned int PROMOTER = 0x20;
  const unsigned int TERMINATOR = 0x40;
  const unsigned int IMMEDIATE_VALUE = 0x80;
}

enum InstructionClass {
  INST_CLASS_NOP = 0,
  INST_CLASS_FLOW_CONTROL,
  INST_CLASS_CONDITIONAL,
  INST_CLASS_ARITHMETIC_LOGIC,
  INST_CLASS_DATA,
  INST_CLASS_ENVIRONMENT,
  INST_CLASS_LIFECYCLE,
  INST_CLASS_OTHER,
  
  NUM_INST_CLASSES
};

enum BehavClass {
  BEHAV_CLASS_INPUT = 0,
  BEHAV_CLASS_ACTION,
  BEHAV_CLASS_COPY,
  BEHAV_CLASS_NONE,
  BEHAV_CLASS_BREAK,
  BEHAV_CLASS_END_GENE,
};

class cInstLibEntry
{
private:
  const cString m_name;
  const unsigned int m_flags;
  const cString m_desc;
  const InstructionClass m_class;
  const BehavClass m_bclass;
  
  cInstLibEntry(); // @not_implemented
  
public:
  cInstLibEntry(const cString& name, InstructionClass _class, unsigned int flags, const cString& desc, BehavClass _bclass)
    : m_name(name), m_flags(flags), m_desc(desc), m_class(_class), m_bclass(_bclass) { ; }

  inline const cString& GetName() const { return m_name; }
  inline const cString& GetDescription() const { return m_desc; }
  
  inline InstructionClass GetClass() const { return m_class; }
  inline BehavClass GetBehavClass() const { return m_bclass; }
  
  inline unsigned int GetFlags() const { return m_flags; }
  inline bool IsDefault() const { return (m_flags & nInstFlag::DEFAULT) != 0; }
  inline bool IsNop() const { return (m_flags & nInstFlag::NOP) != 0; }
  inline bool IsLabel() const { return (m_flags & nInstFlag::LABEL) != 0; }
  inline bool IsPromoter() const { return (m_flags & nInstFlag::PROMOTER) != 0; }
  inline bool IsTerminator() const { return (m_flags & nInstFlag::TERMINATOR) != 0; }
  inline bool ShouldStall() const { return (m_flags & nInstFlag::STALL) != 0; }
  inline bool ShouldSleep() const { return (m_flags & nInstFlag::SLEEP) != 0; }
  inline bool IsImmediateValue() const { return (m_flags & nInstFlag::IMMEDIATE_VALUE) != 0; }
};

#endif
