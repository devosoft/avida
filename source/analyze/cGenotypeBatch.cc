/*
 *  cGenotypeBatch.cc
 *  Avida
 *
 *  Created by David Bryson on 11/3/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

#include "cGenotypeBatch.h"

#include "cAnalyzeGenotype.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"


cAnalyzeGenotype* cGenotypeBatch::PopGenotype(const cString& desc)
{
  cString gen_desc(desc);
  gen_desc.ToLower();
  
  cAnalyzeGenotype* found_gen = NULL;
  
  if (gen_desc == "num_cpus")
    found_gen = m_list.PopIntMax(&cAnalyzeGenotype::GetNumCPUs);
  else if (gen_desc == "total_cpus")
    found_gen = m_list.PopIntMax(&cAnalyzeGenotype::GetTotalCPUs);
  else if (gen_desc == "merit")
    found_gen = m_list.PopDoubleMax(&cAnalyzeGenotype::GetMerit);
  else if (gen_desc == "fitness")
    found_gen = m_list.PopDoubleMax(&cAnalyzeGenotype::GetFitness);
  else if (gen_desc.IsNumeric(0))
    found_gen = m_list.PopIntValue(&cAnalyzeGenotype::GetID, gen_desc.AsInt());
// @TODO - should add support for random, but cGenotypeBatch currently does not have access to the world object
//  else if (gen_desc == "random") {
//    int gen_pos = random.GetUInt(m_list.GetSize());
//    found_gen = m_list.PopPos(gen_pos);
//  }
  else {
    cDriverManager::Status().SignalError(cString("unknown type ") + gen_desc);
  }
  
  return found_gen;
}
