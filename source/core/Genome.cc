/*
 *  core/Genome.cc
 *  avida-core
 *
 *  Created by David on 12/21/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/core/Genome.h"

#include "apto/core/Set.h"
#include "avida/core/Feedback.h"

#include "cInstSet.h"
#include "cHardwareManager.h"

#include "cDataFile.h"
#include "cInitFile.h"
#include "cStringUtil.h"
#include "tDictionary.h"


Avida::Genome::Genome(const cString& seq_str)
{
  cString str = seq_str;
  m_hw_type = str.Pop(',').AsInt();
  m_inst_set = str.Pop(',');
  m_seq = InstructionSequence(str);
}

void Avida::Genome::Load(const tDictionary<cString>& props, cHardwareManager& hwm)
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
  m_seq = Sequence(props.Get("sequence"));
}

void Avida::Genome::Save(cDataFile& df)
{
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_inst_set, "Inst Set Name" , "inst_set");
  df.Write(m_seq.AsString(), "Genome Sequence", "sequence");
}

cString Avida::Genome::AsString() const
{
  return cStringUtil::Stringf("%d,%s,%s", m_hw_type, (const char*)m_inst_set, (const char*)m_seq.AsString());
}

bool Avida::Genome::LoadFromDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm,
                                        Feedback& feedback)
{
  bool success = true;

  Apto::Set<cString> custom_directives;
  custom_directives.Insert("inst_set");
  custom_directives.Insert("hw_type");
  
  cInitFile input_file(fname, wdir, feedback, &custom_directives);
  if (!input_file.WasOpened()) return false;
  

  const cInstSet* is = &hwm.GetDefaultInstSet();
  
  if (input_file.GetCustomDirectives().HasEntry("inst_set")) {
    cString isname = input_file.GetCustomDirectives().Get("inst_set");
    isname.Trim();
    if (hwm.IsInstSet(isname)) {
      is = &hwm.GetInstSet(isname);
    } else {
      feedback.Error("invalid instruction set '%s' defined in organism '%s'", (const char*)isname, (const char*)fname);
      return false;
    }
  }
  
  if (input_file.GetCustomDirectives().HasEntry("hw_type")) {
    m_hw_type = input_file.GetCustomDirectives().Get("hw_type").AsInt();
    if (is->GetHardwareType() != m_hw_type) {
      feedback.Error("hardware type mismatch in organism '%s': is = %d, org = %d",
                     (const char*)fname, is->GetHardwareType(), m_hw_type);
      return false;
    }
  }
  
  m_hw_type = is->GetHardwareType();
  m_inst_set = is->GetInstSetName();
  InstructionSequence new_seq(input_file.GetNumLines());
  
  for (int line_num = 0; line_num < new_seq.GetSize(); line_num++) {
    cString cur_line = input_file.GetLine(line_num);
    new_seq[line_num] = is->GetInst(cur_line);
    
    if (new_seq[line_num] == is->GetInstError()) {
      if (success) {
        feedback.Error("unable to load organism '%s'", (const char*)fname);
        success = false;
      } else {
        feedback.Error("  unknown instruction: %s (best match: %s)",
                       (const char*)cur_line, (const char*)is->FindBestMatch(cur_line));
      }
    }    
  }
  
  if (success) m_seq = new_seq;
  return success;
}


void Avida::Genome::SaveAsDetailFile(cDataFile& df, cHardwareManager& hwm)
{
  df.WriteRaw(cString("#inst_set ") + m_inst_set);
  df.WriteRaw(cStringUtil::Stringf("#hw_type %d", m_hw_type));
  cInstSet& is = hwm.GetInstSet(m_inst_set);
  
  for (int i = 0; i < m_seq.GetSize(); i++) {
    df.Write(is.GetName(m_seq[i]), "Instruction", "inst");
    df.Endl();
  }
}
