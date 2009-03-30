/*
 *  cInjectGenotype.cc
 *  Avida
 *
 *  Called "inject_genotype.cc" prior to 11/15/05.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#include "cInjectGenotype.h"

#include "cTools.h"
#include "cWorld.h"

using namespace std;

cInjectGenotype::cInjectGenotype(cWorld* world, int in_update_born, int in_id)
  : m_world(world)
  , genome(1)
  , name("p001-no_name")
  , flag_threshold(false)
  , is_active(true)
      , can_reproduce(false)
  , defer_adjust(0)
  , id_num(in_id)
  , symbol(0)
  , birth_data(in_update_born)
  , num_injected(0)
  , last_num_injected(0)
  , total_injected(0)
  , next(NULL)
  , prev(NULL)
{
}

cInjectGenotype::~cInjectGenotype()
{
  // Reset some of the variables to make sure program will crash if a deleted
  // cell is read!
  symbol = '!';

  num_injected = -1;
  total_injected = -1;

  next = NULL;
  prev = NULL;
}

bool cInjectGenotype::SaveClone(ofstream& fp)
{
  fp << id_num         << " ";
  fp << genome.GetSize() << " ";

  for (int i = 0; i < genome.GetSize(); i++) {
    fp << ((int) genome[i].GetOp()) << " ";
  }

  return true;
}

bool cInjectGenotype::LoadClone(ifstream & fp)
{
  int genome_size = 0;

  fp >> id_num;
  fp >> genome_size;

  genome = cGenome(genome_size);
  for (int i = 0; i < genome_size; i++) {
    cInstruction temp_inst;
    int inst_op;
    fp >> inst_op;
    temp_inst.SetOp(static_cast<unsigned char>(inst_op));
    genome[i] = temp_inst;
    // @CAO add something here to load arguments for instructions.
  }

  return true;
}

bool cInjectGenotype::OK()
{
  bool ret_value = true;

  // Check statistics
  assert( id_num >= 0 && num_injected >= 0 && total_injected >= 0 );
  assert( birth_data.update_born >= -1);

  return ret_value;
}

void cInjectGenotype::SetParent(cInjectGenotype * parent)
{
  birth_data.parent_genotype = parent;

  // If we have a real parent genotype, collect other data about parent.
  if (parent == NULL) return;
  birth_data.parent_id = parent->GetID();
  birth_data.gene_depth = parent->GetDepth() + 1;
  parent->AddOffspringGenotype();
}

void cInjectGenotype::UpdateReset()
{
  last_num_injected = num_injected;
  birth_data.birth_track.Next();
  birth_data.death_track.Next();
}

void cInjectGenotype::SetGenome(const cGenome & in_genome)
{
  genome = in_genome;

  name.Set("p%03d-no_name", genome.GetSize());
}

void cInjectGenotype::Deactivate(int update)
{
  is_active = false;
  birth_data.update_deactivated = update;
}

int cInjectGenotype::AddParasite()
{
  total_injected++;
  return num_injected++;
}

int cInjectGenotype::RemoveParasite()
{
  //birth_data.death_track.Inc();
  return num_injected--;
}

