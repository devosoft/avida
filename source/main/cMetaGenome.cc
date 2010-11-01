/*
 *  cMetaGenome.cc
 *  Avida
 *
 *  Created by David on 12/21/09.
 *  Copyright 2009-2010 Michigan State University. All rights reserved.
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

#include "cMetaGenome.h"

#include "cDataFile.h"
#include "cStringUtil.h"
#include "tDictionary.h"


cMetaGenome::cMetaGenome(const cString& gen_str)
{
  cString str = gen_str;
  m_hw_type = str.Pop(',').AsInt();
  m_inst_set = str.Pop(',');
  m_genome = cGenome(str);
}

void cMetaGenome::Load(const tDictionary<cString>& props)
{
  if (props.HasEntry("hw_type")) {
    m_hw_type = props.Get("hw_type").AsInt();
  } else {
    m_hw_type = 0; // Default when not found, for backwards compatibility
  }
  if (props.HasEntry("inst_set")) {
    m_inst_set = props.Get("inst_set");
  } else {
    m_inst_set = "(unknown)"; // Default when not found, for backwards compatibility
  }
  assert(props.HasEntry("sequence"));
  m_genome = cGenome(props.Get("sequence"));
}

void cMetaGenome::Save(cDataFile& df)
{
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_inst_set, "Inst Set Name" , "inst_set");
  df.Write(m_genome.AsString(), "Genome Sequence", "sequence");
}

cString cMetaGenome::AsString() const
{
  return cStringUtil::Stringf("%d,%s,%s", m_hw_type, (const char*)m_inst_set, (const char*)m_genome.AsString());
}
