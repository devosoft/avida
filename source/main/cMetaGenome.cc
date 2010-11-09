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
#include "cInitFile.h"
#include "cInstSet.h"
#include "cHardwareManager.h"
#include "cStringUtil.h"
#include "tDictionary.h"


cMetaGenome::cMetaGenome(const cString& seq_str)
{
  cString str = seq_str;
  m_hw_type = str.Pop(',').AsInt();
  m_inst_set = str.Pop(',');
  m_seq = cSequence(str);
}

void cMetaGenome::Load(const tDictionary<cString>& props, cHardwareManager& hwm)
{
  if (props.HasEntry("hw_type")) {
    m_hw_type = props.Get("hw_type").AsInt();
  } else {
    m_hw_type = -1; // Default when not found, for backwards compatibility
  }
  if (props.HasEntry("inst_set")) {
    m_inst_set = props.Get("inst_set");
  } else {
    m_inst_set = "(default)"; // Default when not found, for backwards compatibility
  }
  
  if (m_inst_set == "(default)") {
    const cInstSet& is = hwm.GetDefaultInstSet();
    m_hw_type = is.GetHardwareType();
    m_inst_set = is.GetInstSetName();
  }
  
  assert(props.HasEntry("sequence"));
  m_seq = cSequence(props.Get("sequence"));
}

void cMetaGenome::Save(cDataFile& df)
{
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_inst_set, "Inst Set Name" , "inst_set");
  df.Write(m_seq.AsString(), "Genome Sequence", "sequence");
}

cString cMetaGenome::AsString() const
{
  return cStringUtil::Stringf("%d,%s,%s", m_hw_type, (const char*)m_inst_set, (const char*)m_seq.AsString());
}

bool cMetaGenome::LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm,
                                     tList<cString>* errors)
{
  cInitFile input_file(fname, wdir);
  if (errors) errors->Append(input_file.GetErrors());
  bool success = true;

  if (!input_file.WasOpened()) return false;
  
  const cInstSet& is = hwm.GetDefaultInstSet();
  m_hw_type = is.GetHardwareType();
  m_inst_set = is.GetInstSetName();
  cSequence new_seq(input_file.GetNumLines());
  
  for (int line_num = 0; line_num < new_seq.GetSize(); line_num++) {
    cString cur_line = input_file.GetLine(line_num);
    new_seq[line_num] = is.GetInst(cur_line);
    
    if (new_seq[line_num] == is.GetInstError()) {
      if (success) {
        if (errors) errors->PushRear(new cString(cStringUtil::Stringf("unable to load organism '%s'", (const char*)fname)));
        success = false;
      } else {
        if (errors) errors->PushRear(new cString(cStringUtil::Stringf("  unknown instruction: %s (best match: %s)",
                                                                      (const char*)cur_line,
                                                                      (const char*)is.FindBestMatch(cur_line))));
      }
    }    
  }
  
  if (success) m_seq = new_seq;
  return success;
}


void cMetaGenome::SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm)
{
  df.WriteRaw(cString("#inst_set ") + m_inst_set);
  cInstSet& is = hwm.GetInstSet(m_inst_set);
  
  for (int i = 0; i < m_seq.GetSize(); i++) {
    df.Write(is.GetName(m_seq[i]), "Instruction", "inst");
    df.Endl();
  }
}
