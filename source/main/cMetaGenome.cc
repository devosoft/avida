/*
 *  cMetaGenome.cc
 *  Avida
 *
 *  Created by David on 12/21/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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


void cMetaGenome::Save(cDataFile& df)
{
  df.Write(m_hw_type, "Hardware Type ID", "hw_type");
  df.Write(m_inst_set_id, "Inst Set ID" , "inst_set");
  df.Write(m_genome.AsString(), "Genome Sequence", "sequence");
}
