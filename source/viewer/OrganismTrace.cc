/*
 *  viewer/OrganismTrace.cc
 *  Avida
 *
 *  Created by David on 2/15/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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

#include "avida/viewer/OrganismTrace.h"

#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"

#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cTestCPU.h"
#include "cWorld.h"

using namespace Avida;
using namespace Avida::Viewer;


class SnapshotTracer : public cHardwareTracer, public WorldDriver
{
private:
  cWorld* m_world;
  
  class NullFeedback : public Avida::Feedback
  {
    void Error(const char* fmt, ...) { ; }
    void Warning(const char* fmt, ...) { ; }
    void Notify(const char* fmt, ...) { ; }
  } m_feedback;

  Apto::Array<HardwareSnapshot*>* m_snapshots;
  int m_snapshot_count;
  

public:
  inline SnapshotTracer(cWorld* world) : m_world(world), m_snapshots(NULL) { ; }
  
  void TraceGenome(GenomePtr genome, Apto::Array<HardwareSnapshot*>& snapshots);
  
  // cHardwareTracer
  void TraceHardware(cAvidaContext& ctx, cHardwareBase&, bool bonus = false, bool mini = false, int exec_success = -2);
  void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism);
  
  
  // WorldDriver
  void Pause() { ; }
  void Finish() { ; }
  void Abort(AbortCondition condition) { ; }
  
  Avida::Feedback& Feedback() { return m_feedback; }
  
  void RegisterCallback(DriverCallback callback) { ; }
};


void SnapshotTracer::TraceGenome(GenomePtr genome, Apto::Array<HardwareSnapshot*>& snapshots)
{
  // Create internal reference to the snapshot array so that the tracing methods can create snapshots
  m_snapshots = &snapshots;
  
  // Set up tracking objects and variables
  m_snapshot_count = 0;
  
  
  // Setup context
  cRandom rng(100);
  cAvidaContext ctx(this, rng);
  
  // Create a test cpu
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
  
  // Setup test info to trace into this tracer
  cCPUTestInfo test_info;  
  test_info.SetTraceExecution(this);
  
  // Test the actual genome
  testcpu->TestGenome(ctx, test_info, *genome);
  
  // Clear internal reference to the snapshot array
  m_snapshots = NULL;
}



void SnapshotTracer::TraceHardware(cAvidaContext& ctx, cHardwareBase& hw, bool bonus, bool mini, int exec_success)
{
  // Create snapshot based on current hardware state
  m_snapshot_count++;
  
  // Make sure snapshot array is big enough (just in case bonus cycles or threads are in use)
  if (m_snapshots->GetSize() < m_snapshot_count) m_snapshots->Resize(m_snapshots->GetSize() * 2);
  
  HardwareSnapshot* snapshot = new HardwareSnapshot(hw.GetNumRegisters());
  (*m_snapshots)[m_snapshot_count - 1] = snapshot;
  
  // Store register states
  for (int reg = 0; reg < hw.GetNumRegisters(); reg++) snapshot->SetRegister(reg, hw.GetRegister(reg));
  
  Apto::Array<int> buffer_values;

  // Handle Input Buffer
  buffer_values.Resize(hw.GetInputBuf().GetNumStored());
  for (int i = 0; i < hw.GetInputBuf().GetNumStored(); i++) buffer_values[i] = hw.GetInputBuf()[i];
  snapshot->AddBuffer("input", buffer_values);
  
  // Handle Output Buffer
  buffer_values.Resize(hw.GetOutputBuf().GetNumStored());
  for (int i = 0; i < hw.GetOutputBuf().GetNumStored(); i++) buffer_values[i] = hw.GetOutputBuf()[i];
  snapshot->AddBuffer("output", buffer_values);
  
  // Handle Stacks
  buffer_values.Resize(nHardware::STACK_SIZE);
  for (int stk = 0; stk < hw.GetNumStacks(); stk++) {
    for (int i = 0; i < nHardware::STACK_SIZE; i++) buffer_values[i] = hw.GetStack(i, stk);
    snapshot->AddBuffer(Apto::FormatStr("stack %c", 'A' + stk), buffer_values);
  }
  
  
  
  // @TODO
}


void SnapshotTracer::TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)
{
  // Trace finished, cleanup...
  
  // Resize the snapshot array to the actual number of snapshots
  m_snapshots->Resize(m_snapshot_count);
}




HardwareSnapshot::HardwareSnapshot(int num_regs)
: m_registers(num_regs), m_layout(false)
{
  
}


HardwareSnapshot::~HardwareSnapshot()
{
  for (int i = 0; i < m_graphic_objects.GetSize(); i++) delete m_graphic_objects[i]; 
}



void HardwareSnapshot::AddBuffer(const Apto::String& description, const Apto::Array<int>& values)
{
  m_buffers[description] = values;
}


void HardwareSnapshot::SetFunctionCount(const Apto::String& function, int count)
{
  m_function_counts[function] = count;
}


int HardwareSnapshot::AddMemSpace(const Apto::String& label, const Apto::Array<Instruction>& memory)
{
  int idx = m_mem_spaces.GetSize();
  m_mem_spaces.Resize(idx + 1);
  m_mem_spaces[idx].label = label;
  m_mem_spaces[idx].memory = memory;
  
  return idx;
}


void HardwareSnapshot::AddHead(const Apto::String& label, int mem_space, int index)
{
  m_mem_spaces[mem_space].heads[label] = index;
}


void HardwareSnapshot::AddJump(int from_mem_space, int from_idx, int to_mem_space, int to_idx, int freq)
{
  m_jumps.Push(Jump(from_mem_space, from_idx, to_mem_space, to_idx, freq));
}


void Avida::Viewer::HardwareSnapshot::doLayout() const
{
  
}



OrganismTrace::OrganismTrace(cWorld* world, GenomePtr genome)
  : m_genome(genome)
{
  SnapshotTracer tracer(world);
  tracer.TraceGenome(genome, m_snapshots);
}


OrganismTrace::~OrganismTrace()
{
  for (int i = 0; i < m_snapshots.GetSize(); i++) delete m_snapshots[i];
}
