/*
 *  cMxCodeArray.cc
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cMxCodeArray.h"

#include "cCPUTestInfo.h"
#include "cGenome.h"
#include "cInstSet.h"
#include "cHardwareManager.h"
#include "MyCodeArrayLessThan.h"
#include "cPhenotype.h"
#include "cTestCPU.h"
#include "cTools.h"
#include "cWorld.h"

#include <iomanip>

using namespace std;

const int cMxCodeArray::m_max_gestation_time = 1000000000;


cMxCodeArray::cMxCodeArray(cWorld* world) : m_world(world)
{
  // Initialize the code array as empty.
  size = 0;
  max_size = 0;
  data = NULL;
  m_merit = 0;
  m_gestation_time = m_max_gestation_time;
  m_num_instructions = 0;
}

cMxCodeArray::cMxCodeArray(cWorld* world, int n_inst, int in_size, int in_max_size)
: m_world(world), m_merit(0), m_gestation_time(m_max_gestation_time), m_num_instructions(n_inst)
{  
  assert (in_max_size == 0 || in_max_size >= in_size);

  // Initialize the code array randomly.
  size = in_size;
  if (in_max_size != 0) max_size = in_max_size;
  else max_size = size;

  data = new cInstruction[max_size];

  for (int i = 0; i < size; i++)
    {
      data[i].SetOp(m_world->GetRandom().GetUInt(m_world->GetNumInstructions()));
    }

}

cMxCodeArray::cMxCodeArray(const cMxCodeArray &in_code)
{
  m_world = in_code.m_world;
  size = in_code.size;
  max_size = in_code.max_size;
  m_merit = in_code.m_merit;
  m_gestation_time = in_code.m_gestation_time;
  m_num_instructions = in_code.m_num_instructions;
  m_trans_probs = in_code.m_trans_probs;

  data = new cInstruction[max_size];

  for (int i = 0; i < size; i++)
    {
      data[i] = in_code.data[i];
    }

}


cMxCodeArray::cMxCodeArray(cWorld* world, const cGenome &in_code, int in_max_size)
: m_world(world), m_merit(0), m_gestation_time(m_max_gestation_time), m_num_instructions(0)
{
  assert (in_max_size == 0 || in_max_size >= in_code.GetSize());

  size = in_code.GetSize();
  if (in_max_size != 0) max_size = in_max_size;
  else max_size = size;

  data = new cInstruction[max_size];

  for (int i = 0; i < size; i++) {
    data[i] = in_code[i];
  }
}


cMxCodeArray::~cMxCodeArray()
{
  // TODO - what is this?
  // g_memory.Remove(C_CODE_ARRAY);

  if (data) delete [] data;
}

void cMxCodeArray::Resize(int new_size)
{
  assert (new_size <= MAX_CREATURE_SIZE);

  // If we are not changing the size, just return.
  if (new_size == size) return;

  // Re-construct the arrays only if we have to...
  if (new_size > max_size || new_size * 4 < max_size) {
    cInstruction * new_data = new cInstruction[new_size];

    for (int i = 0; i < size && i < new_size; i++) {
      new_data[i] = data[i];
    }

    if (data) delete [] data;

    data = new_data;
    max_size = new_size;
  }

  // Fill in the un-filled-in bits...
  for (int i = size; i < new_size; i++) {
    data[i].SetOp(m_world->GetRandom().GetUInt(m_world->GetNumInstructions()));
  }

  size = new_size;
}

// Resize the arrays, and leave memeory uninitializes! (private method)
void cMxCodeArray::ResetSize(int new_size, int new_max)
{
  assert (new_size >= 0);
  assert (new_max == 0 || new_max >= new_size);

  // If we are clearing it totally, do so.
  if (!new_size) {
    if (data) delete [] data;
    data = NULL;
    size = 0;
    max_size = 0;
    return;
  }

  // If a new max is _not_ being set, see if one needs to be.
  if (new_max == 0 &&
      (max_size < new_size) || (max_size > new_size * 2)) {
    new_max = new_size;
  }

  // If the arrays need to be rebuilt, do so.
  if (new_max != 0 && new_max != max_size) {
    cInstruction * new_data = new cInstruction[new_max];

    if (data) delete [] data;
    data = new_data;
    max_size = new_max;
  }

  // Reset the array size.
  size = new_size;
}

/**
 * Resizes and reinitializes the genome. The previous data is lost.
 **/
void cMxCodeArray::Reset()
{
  // Initialze the array
  if( m_world->GetConfig().ALLOC_METHOD.Get() == ALLOC_METHOD_RANDOM ){
    // Randomize the initial contents of the new array.
    Randomize();
  }else{
    // Assume default instuction initialization
    Clear();
  }
}

/**
 * Sets all instructions to @ref cInstruction::GetInstDefault().
 **/
void cMxCodeArray::Clear()
{
  int i;
  for (i = 0; i < size; i++)
    {
      data[i] = cInstSet::GetInstDefault();
    }
}

/**
 * Replaces all instructions in the genome with a sequence of random
 * instructions.
 **/

void cMxCodeArray::Randomize()
{
  int i;
  for (i = 0; i < size; i++)
    {
      data[i].SetOp(m_world->GetRandom().GetUInt(m_world->GetNumInstructions()));
    }
}

void cMxCodeArray::operator=(const cMxCodeArray & other_code)
{
  // If we need to resize, do so...

  ResetSize(other_code.size);

  // Now that both code arrays are the same size, copy the other one over.

  for (int i = 0; i < size; i++)
    {
      data[i] = other_code.data[i];
    }

    m_merit = other_code.m_merit;
    m_gestation_time = other_code.m_gestation_time;
    m_num_instructions = other_code.m_num_instructions;
    m_trans_probs = other_code.m_trans_probs;
}


/*
 * Let the 0th  instruction be the least significant,
 * so longer codes are always greater.
 */
bool cMxCodeArray::operator<(const cMxCodeArray &other_code) const
{

  if (size < other_code.size)
    return true;
  else if (size > other_code.size)
    return false;

  bool result = false;

  for (int i = size-1; i >= 0; i-- )
    {
      if (data[i].GetOp() < other_code.data[i].GetOp())
	{
	  result = true;
	  break;
	}
      else if (other_code.data[i].GetOp() < data[i].GetOp())
	{
	  result = false;
	  break;
	}
    }

  return result;
}


void cMxCodeArray::CopyDataTo(cGenome & other_code) const
{
  // @CAO this is ugly, but the only option I see...
  other_code = cGenome(size);

  for (int i = 0; i < size; i++) {
    other_code[i] = data[i];
  }
}

bool cMxCodeArray::OK() const
{
  bool result = true;

  assert (size >= 0 && size <= max_size && max_size <= MAX_CREATURE_SIZE);

  return result;
}


int cMxCodeArray::FindInstruction(const cInstruction & inst, int start_index)
{
  assert(start_index < GetSize());
  for(int i=0; i<GetSize(); ++i ){
    if( Get(i) == inst ){
      return i;
    }
  }
  return -1;
}

cString cMxCodeArray::AsString() const
{
  cString out_string(size);
  for (int i = 0; i < size; i++) {
    int cur_char = data[i].GetOp();
    if (cur_char < 26) {
      out_string[i] = cur_char + 'a';
    } else if (cur_char < 52) {
      out_string[i] = cur_char - 26 + 'A';
    } else if (cur_char < 62) {
      out_string[i] = cur_char - 52 + '1';
    } else {
      out_string[i] = '*';
    }
  }

  return out_string;
}


void cMxCodeArray::Get1Mutants(list<cMxCodeArray>& mutants) const
{
  for (int line_num = 0; line_num < size; line_num++)
    {
      int cur_inst = data[line_num].GetOp();

      for (int inst_num = 0; inst_num < m_num_instructions; inst_num++)
	{
	  if (cur_inst == inst_num) continue;

	  cMxCodeArray newMutant(*this);
	  newMutant[line_num].SetOp(inst_num);
	  mutants.push_back(newMutant);
	}
    }
}


/*
  Make a list of the transition probabilities only to those other
  genotypes that are within the given hamming distance of this
  genotype.

  The list is actually implemented as a map from indexes to
  probabilities, where the index is the place of the genotype in
  the complete ordered set of genotypes.

  TODO this seems kind of inefficient that you have to calculate the
  hamming distance twice - maybe I should pass it as a parameter to
  the TransitionProbability function after it is first calculated?  Or
  maybe the threshold should be in the transition probability?
*/

  /* dropped const for win32 -- kgn */
//int cMxCodeArray::MakeTransitionList(const set<cMxCodeArray, MyCodeArrayLessThan > &data_set, int hamDistThresh, double errorRate) const
int cMxCodeArray::MakeTransitionList(set<cMxCodeArray, MyCodeArrayLessThan > &data_set, int hamDistThresh, double errorRate) const
{
  set<cMxCodeArray, MyCodeArrayLessThan >::iterator set_iter;
  int index = 0;
  double totalProb = 0.0;
  int num = 0;

  for (set_iter = data_set.begin(); set_iter != data_set.end(); set_iter++, index++)
    {
      if (HammingDistance(*set_iter) <= hamDistThresh)
	{
	  double transProb =  TransitionProbability(*set_iter, errorRate);
	  m_trans_probs[index] = transProb * GetFitness();
	  totalProb += transProb;
	  num += 1;
	}
      else
	{
	  m_trans_probs[index]= 0.0;
	}
    }

  /* The transition probability for "other"
     index got incremented at the very end of the loop so its pointing
     to the next element now */

  m_trans_probs[index] = (1.0 - totalProb) * GetFitness();

  return num;

}


const map<int, double, less<int> >& cMxCodeArray::GetTransitionList() const
{
  return m_trans_probs;
}


int cMxCodeArray::HammingDistance(const cMxCodeArray &other_gene) const
{
  int distance = 0;

  assert (size == other_gene.GetSize());

  for (int i = 0; i < size; i++)
    {
      if (this->Get(i) != other_gene.Get(i))
	distance++;
    }

  return distance;
}


double cMxCodeArray::TransitionProbability(const cMxCodeArray &other_gene, double errorRate) const
{
  double Q = 0.0;
  assert (size == other_gene.GetSize());

  if ( (*this) == other_gene)
    {
      Q = pow((1-errorRate), size);
    }
  else
    {
      int distance = HammingDistance(other_gene);
      Q = pow((1.0-errorRate), (size -distance)) * pow((errorRate/m_num_instructions),distance);
    }

  return Q;
}

/*
  Used in testing the diagonalization.
*/

void cMxCodeArray::PrintTransitionList(ostream& fp, int size) const
{

  fp.setf(ios::fixed);

  fp << "{" ;

  for (int index=0 ; index < size; index++)
    {
      map<int, double, less<int> >::const_iterator find_results = m_trans_probs.find(index);
      if (find_results == m_trans_probs.end())
	{
	  fp << 0.0 ;
	}
      else
	{
	  fp << setprecision(10) << (*find_results).second ;
	}

      if (index < size-1)
	fp << " , " ;

    }

  fp << "}" ;
}


void cMxCodeArray::CalcFitness(cAvidaContext& ctx)
{
  cGenome temp(1);
  
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
  cCPUTestInfo test_info;
  CopyDataTo(temp); 
  testcpu->TestGenome(ctx, test_info, temp);
  delete testcpu;
  
  if (test_info.IsViable())
    m_gestation_time = test_info.GetTestPhenotype().GetGestationTime();
  else // if not viable, set a really high gestation time
    m_gestation_time = m_max_gestation_time;
  m_merit = test_info.GetTestPhenotype().GetMerit().GetDouble();
}
