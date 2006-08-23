/*
 *  cInstSet.cc
 *  Avida
 *
 *  Called "inst_set.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#include "cInstSet.h"

#include "cAvidaContext.h"
#include "cStringUtil.h"
#include "cWorld.h"

using namespace std;


// Initialize static variables
const cInstruction cInstSet::inst_default(0);
cInstruction cInstSet::inst_default2(0);
cInstruction cInstSet::inst_error2(255);


bool cInstSet::OK() const
{
  assert(m_lib_name_map.GetSize() < 256);
  assert(m_lib_nopmod_map.GetSize() < m_lib_name_map.GetSize());

  // Make sure that all of the redundancies are represented the appropriate
  // number of times.
  tArray<int> test_redundancy2(m_lib_name_map.GetSize());
  test_redundancy2.SetAll(0);
  for (int i = 0; i < m_mutation_chart.GetSize(); i++) {
    int test_id = m_mutation_chart[i];
    test_redundancy2[test_id]++;
  }
  for (int i = 0; i < m_lib_name_map.GetSize(); i++) {
    assert(m_lib_name_map[i].redundancy == test_redundancy2[i]);
  }

  return true;
}

cInstruction cInstSet::GetRandomInst(cAvidaContext& ctx) const
{
  int inst_op = m_mutation_chart[ctx.GetRandom().GetUInt(m_mutation_chart.GetSize())];
  return cInstruction(inst_op);
}

int cInstSet::AddInst(int lib_fun_id, int redundancy, int ft_cost, int cost, double prob_fail)
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

  const int total_redundancy = m_mutation_chart.GetSize();
  m_mutation_chart.Resize(total_redundancy + redundancy);
  for (int i = 0; i < redundancy; i++) {
    m_mutation_chart[total_redundancy + i] = inst_id;
  }

  return inst_id;
}

int cInstSet::AddNop(int lib_nopmod_id, int redundancy, int ft_cost, int cost, double prob_fail)
{
  // Assert nops are at the _beginning_ of an inst_set.
  assert(m_lib_name_map.GetSize() == m_lib_nopmod_map.GetSize());

  const int inst_id = AddInst(lib_nopmod_id, redundancy, ft_cost, cost, prob_fail);

  m_lib_nopmod_map.Resize(inst_id + 1);
  m_lib_nopmod_map[inst_id] = lib_nopmod_id;

  return inst_id;
}

cString cInstSet::FindBestMatch(const cString& in_name) const
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

