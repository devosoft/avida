/*
 *  private/util/GenomeLoader.h
 *  Avida
 *
 *  Created by David on 12/22/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
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

#include "avida/private/util/GenomeLoader.h"

#include "avida/core/Genome.h"

#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cInstSet.h"
#include "cString.h"


Avida::GenomePtr Avida::Util::LoadGenomeDetailFile(const cString& fname, const cString& wdir, cHardwareManager& hwm,
                                                   Feedback& feedback, Apto::String specified_instset)
{
  bool success = true;
  
  Apto::Set<Apto::String> custom_directives;
  custom_directives.Insert("inst_set");
  custom_directives.Insert("hw_type");
  
  cInitFile input_file(fname, wdir, feedback, &custom_directives);
  if (!input_file.WasOpened()) return GenomePtr();
  
  
  const cInstSet* is = &hwm.GetDefaultInstSet();
  
  if (specified_instset.GetSize() || input_file.GetCustomDirectives().Has("inst_set")) {
    if (!specified_instset.GetSize()) specified_instset = input_file.GetCustomDirectives().GetWithDefault("inst_set", "");
    
    specified_instset.Trim();
    if (hwm.IsInstSet(specified_instset)) {
      is = &hwm.GetInstSet(specified_instset);
    } else {
      feedback.Error("invalid instruction set '%s' defined in organism '%s'", (const char*)specified_instset, (const char*)fname);
      return GenomePtr();
    }
  }
  
  int hw_type = -1;
  cString inst_set;
  
  if (input_file.GetCustomDirectives().Has("hw_type")) {
    hw_type = Apto::StrAs(input_file.GetCustomDirectives().GetWithDefault("hw_type", "0"));
    if (is->GetHardwareType() != hw_type) {
      feedback.Error("hardware type mismatch in organism '%s': is = %d, org = %d",
                     (const char*)fname, is->GetHardwareType(), hw_type);
      return GenomePtr();
    }
  }
  
  hw_type = is->GetHardwareType();
  inst_set = is->GetInstSetName();
  InstructionSequencePtr new_seq(new InstructionSequence(input_file.GetNumLines()));
  
  for (int line_num = 0; line_num < new_seq->GetSize(); line_num++) {
    cString cur_line = input_file.GetLine(line_num);
    (*new_seq)[line_num] = is->GetInst(cur_line);
    
    if ((*new_seq)[line_num] == is->GetInstError()) {
      if (success) {
        feedback.Error("unable to load organism '%s'", (const char*)fname);
        success = false;
      } else {
        feedback.Error("  unknown instruction: %s (best match: %s)",
                       (const char*)cur_line, (const char*)is->FindBestMatch(cur_line));
      }
    }    
  }
  
  
  if (!success) return GenomePtr();
  
  HashPropertyMap props;
  cHardwareManager::SetupPropertyMap(props, (const char*)inst_set);
  return GenomePtr(new Genome(hw_type, props, new_seq));
}
