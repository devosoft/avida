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

#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cHeadCPU.h"
#include "cOrganism.h"
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
  
  int m_genome_length;
  Instruction m_first_inst;
  int m_last_mem_space;
  int m_last_idx;
  

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
  
  InstructionSequencePtr seq;
  seq.DynamicCastFrom(genome->Representation());
  m_genome_length = seq->GetSize();
  m_first_inst = (*seq)[0];
  
  m_last_mem_space = 0;
  m_last_idx = 0;
  
  
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
  (void)ctx;
  (void)bonus;
  (void)mini;
  (void)exec_success;
  
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
  
  // Handle function counts
  const tArray<int>& task_counts = hw.GetOrganism()->GetPhenotype().GetCurTaskCount();
  for (int i = 0; i < task_counts.GetSize(); i++) {
    snapshot->SetFunctionCount((const char*)m_world->GetEnvironment().GetTask(i).GetDesc(), task_counts[i]);
  }
  
  // Handle memory spaces
  Apto::Array<Instruction> memory;
  
  // - handle the genome part of the memory
  memory.Resize((m_genome_length < hw.GetMemory().GetSize()) ? m_genome_length : hw.GetMemory().GetSize());
  for (int i = 0; i < m_genome_length && i < hw.GetMemory().GetSize(); i++) {
    memory[i] = hw.GetMemory()[i];
  }
  snapshot->AddMemSpace("genome", memory);
  
  // - handle all heads that are in the first part of the memory space
  for (int i = 0; i < hw.GetNumHeads(); i++) {
    Apto::String name = "FLOW";
    if (i == 0) name = "IP";
    if (i == 1) name = "READ";
    if (i == 2) name = "WRITE";
    if (hw.GetHead(i).GetPosition() < m_genome_length) snapshot->AddHead(name, 0, hw.GetHead(i).GetPosition());
  }
  
  // - handle the offspring part of the memory
  memory.Resize(hw.GetMemory().GetSize() - memory.GetSize());
  for (int i = m_genome_length; i < hw.GetMemory().GetSize(); i++) {
    memory[i] = hw.GetMemory()[i];
  }
  
  // - determine the maximum position of any head
  int max_head_pos = 0;
  for (int i = 0; i < hw.GetNumHeads(); i++) {
    int head_pos = hw.GetHead(i).GetPosition();
    if (head_pos > max_head_pos) max_head_pos = head_pos;
  }
  
  // - if the maximum position is in the offspring part of the memory
  if (max_head_pos >= m_genome_length) {
    // truncate the offspring part of the memory to the position of the last head
    memory.Resize(max_head_pos - m_genome_length + 1);
    snapshot->AddMemSpace("offsping", memory);
    
    // handle all heads that are in the second part of the memory space
    for (int i = 0; i < hw.GetNumHeads(); i++) {
      Apto::String name = "FLOW";
      if (i == 0) name = "IP";
      if (i == 1) name = "READ";
      if (i == 2) name = "WRITE";
      if (hw.GetHead(i).GetPosition() >= m_genome_length) snapshot->AddHead(name, 1, hw.GetHead(i).GetPosition());
    }
  }  
  
  // Add/Update jump based on this current instruction execution
  snapshot->AddJump(m_last_mem_space, m_last_idx, hw.IP().GetMemSpace(), hw.IP().GetPosition());
  
  // Cache the head position for calculating the next jump
  m_last_mem_space = hw.IP().GetMemSpace();
  m_last_idx = hw.IP().GetPosition();  
  
  // Store next instruction that will be executed
  snapshot->SetNextInst(hw.IP().GetInst());
}


void SnapshotTracer::TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)
{
  (void)time_used;
  (void)time_allocated;
  (void)organism;
  
  // Trace finished, cleanup...
  
  // Resize the snapshot array to the actual number of snapshots
  m_snapshots->Resize(m_snapshot_count);
}




HardwareSnapshot::HardwareSnapshot(int num_regs, HardwareSnapshot* previous_snapshot)
: m_registers(num_regs), m_layout(false)
{
  if (previous_snapshot) m_jumps = previous_snapshot->m_jumps;
}


HardwareSnapshot::~HardwareSnapshot()
{
  for (int i = 0; i < m_graphic_objects.GetSize(); i++) delete m_graphic_objects[i]; 
}



void HardwareSnapshot::AddBuffer(const Apto::String& description, const Apto::Array<int>& values)
{
  m_buffers.Set(description, values);
}


void HardwareSnapshot::SetFunctionCount(const Apto::String& function, int count)
{
  m_function_counts.Set(function, count);
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


void HardwareSnapshot::AddJump(int from_mem_space, int from_idx, int to_mem_space, int to_idx)
{
  for (int i = 0; i < m_jumps.GetSize(); i++) {
    Jump& jmp = m_jumps[i];
    if (jmp.from_mem_space == from_mem_space && jmp.from_idx == from_idx &&
        jmp.to_mem_space == to_mem_space && jmp.to_idx == to_idx) {
      jmp.freq++;
      return;
    }
  }
  m_jumps.Push(Jump(from_mem_space, from_idx, to_mem_space, to_idx, 1));
}


// This function takes the current state of a CPU and translates it into a set of graphical objects that can be drawn on
// the screen.

void Avida::Viewer::HardwareSnapshot::doLayout() const
{
  // Build the various graphic objects that need to be displayed.
  const double genome_spacing = 0.1;                 // Space between two genome circles.
  const double inst_radius = 0.03;                   // Radius of each instruction circle
  const double inst_diameter = inst_radius * 2.0;    // Circumference of each instruction circle
  const double inst_spacing = inst_diameter * 1.05;  // How much room to leave for each instruction?
  const double PI = 3.14159265;

  // Draw each memory space
  int num_mem_spaces = m_mem_spaces.GetSize();
  for (cur_mem_id = 0; cur_mem_id < 2 && cur_mem_id < num_mem_spaces; cur_mem_id++) {
    MemSpace * cur_memspace = m_mem_spaces[cur_mem_id];
    Apto::Array<Instruction> & cur_mem = cur_memspace->memory;
    const int cur_length = cur_mem.GetSize();

    const double genome_circumference = ((double) cur_length) * inst_spacing;
    const double angle_step = 2.0*PI / (double) cur_length;
    const double genome_radius = genome_circumference / (2.0*PI);
    const double genome_offset = genome_radius + genome_spacing;

    // Setup the central position for this memory space.  For the moment, assume we only have parent and offspring.
    double center_x = 0.0;
    double center_y = 0.0;
    if (cur_mem_id == 0) center_x -= genome offset; else center_x += genome_offset;

    // Step through the genome, placing each instruction on the screen;
    for (int i = 0; i < cur_length; i++) {
      // Calculate where this instruction should be drawn on the screen.
      // Center position + angular offset - radius (since we need the lower, left corner of each circle to draw)
      double cur_angle = angle_step * (double) i;
      inst_x = center_x + sin(cur_angle) * genome_radius - inst_radius;
      inst_y = center_y + cos(cur_angle) * genome_radius - inst_radius;

      GraphicObject * inst_go = new GraphicObject(inst_x, inst_y, inst_diameter, inst_diameter, GraphicObject::SHAPE_OVAL);
      // @CAO setup color of circle based on instruction at that point;
      m_graphic_objects.Push(inst_go);
    }
  }

  // @CAO Draw heads
  // @CAO Draw arcs showing prior execution path.
  // @CAO Draw other hardware as needed

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
