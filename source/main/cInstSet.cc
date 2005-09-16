//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INST_SET_HH
#include "cInstSet.h"
#endif

#ifndef STRING_UTIL_HH
#include "string_util.hh"
#endif
#ifndef TOOLS_HH
#include "tools.hh"
#endif

using namespace std;

//////////////////////
//  cInstSet
//////////////////////

// Initialize static variables
const cInstruction cInstSet::inst_default(   0 );
cInstruction cInstSet::inst_default2(   0 );
cInstruction cInstSet::inst_error2  ( 255 );

cInstSet::cInstSet()
{
}

cInstSet::cInstSet(const cInstSet & in_inst_set)
  : m_inst_lib(in_inst_set.m_inst_lib)
  , m_lib_name_map(in_inst_set.m_lib_name_map)
  , m_lib_nopmod_map(in_inst_set.m_lib_nopmod_map)
  , mutation_chart2(in_inst_set.mutation_chart2)
{
}

cInstSet::~cInstSet()
{
}

cInstSet & cInstSet::operator=(const cInstSet & _in)
{
  m_inst_lib = _in.m_inst_lib;
  m_lib_name_map = _in.m_lib_name_map;
  m_lib_nopmod_map = _in.m_lib_nopmod_map;
  mutation_chart2 = _in.mutation_chart2;
  return *this;
}

bool cInstSet::OK() const
{
  assert(m_lib_name_map.GetSize() < 256);
  assert(m_lib_nopmod_map.GetSize() < m_lib_name_map.GetSize());

  // Make sure that all of the redundancies are represented the appropriate
  // number of times.
  tArray<int> test_redundancy2(m_lib_name_map.GetSize());
  test_redundancy2.SetAll(0);
  for (int i = 0; i < mutation_chart2.GetSize(); i++) {
    int test_id = mutation_chart2[i];
    test_redundancy2[test_id]++;
  }
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    assert(m_lib_name_map[i].redundancy == test_redundancy2[i]);
  }

  return true;
}

cInstruction cInstSet::GetRandomInst() const
{
  int inst_op = mutation_chart2[g_random.GetUInt(mutation_chart2.GetSize())];
  return cInstruction(inst_op);
}

int cInstSet::Add2(
  const int lib_fun_id,
  const int redundancy,
  const int ft_cost,
  const int cost,
  const double prob_fail
)
{
  const int inst_id = m_lib_name_map.GetSize();

  assert(inst_id < 255);

  // Increase the size of the array...
  m_lib_name_map.Resize(inst_id + 1);

  // Setup the new function...
  m_lib_name_map[inst_id].lib_fun_id = lib_fun_id;
  m_lib_name_map[inst_id].redundancy = redundancy;
  m_lib_name_map[inst_id].cost = cost;
  m_lib_name_map[inst_id].ft_cost = ft_cost;
  m_lib_name_map[inst_id].prob_fail = prob_fail;

  const int total_redundancy = mutation_chart2.GetSize();
  mutation_chart2.Resize(total_redundancy + redundancy);
  for (int i = 0; i < redundancy; i++) {
    mutation_chart2[total_redundancy + i] = inst_id;
  }

  return inst_id;
}

int cInstSet::AddNop2(
  const int lib_nopmod_id,
  const int redundancy,
  const int ft_cost,
  const int cost,
  const double prob_fail
)
{ 
  // Assert nops are at the _beginning_ of an inst_set.
  assert(m_lib_name_map.GetSize() == m_lib_nopmod_map.GetSize());

  const int inst_id = Add2(lib_nopmod_id, redundancy, ft_cost, cost, prob_fail);

  m_lib_nopmod_map.Resize(inst_id + 1);
  m_lib_nopmod_map[inst_id] = lib_nopmod_id;

  return inst_id;
}

cString cInstSet::FindBestMatch(const cString & in_name) const
{
  int best_dist = 1024;
  cString best_name("");
  
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    const cString & cur_name = m_inst_lib->GetName(m_lib_name_map[i].lib_fun_id);
    const int cur_dist = cStringUtil::EditDistance(cur_name, in_name);
    if (cur_dist < best_dist) {
      best_dist = cur_dist;
      best_name = cur_name;
    }
    if (cur_dist == 0) break;
  }

  return best_name;
}

