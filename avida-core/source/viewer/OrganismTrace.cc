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
 *  Authors: David M. Bryson <david@programerror.com>, Charles Ofria <charles.ofria@gmail.com>
 *
 */

#include "avida/viewer/OrganismTrace.h"

#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"
#include "avida/viewer/GraphicsContext.h"

#include "apto/rng.h"

#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cHardwareTracer.h"
#include "cHeadCPU.h"
#include "cInstSet.h"
#include "cOrganism.h"
#include "cTestCPU.h"
#include "cWorld.h"

using namespace Avida;
using namespace Avida::Viewer;


// Private Declarations
// --------------------------------------------------------------------------------------------------------------  

namespace Avida {
  namespace Viewer {
    namespace Private {
      
      class SnapshotTracer;
      class InstructionColorChart;
      
      typedef Apto::SmartPtr<InstructionColorChart> InstructionColorChartPtr;
      
    };
  };
};


// Private::InstructionColorChart
// --------------------------------------------------------------------------------------------------------------  

class Private::InstructionColorChart : public GraphicsContextData
{
private:
  Apto::Array<Color> m_inst_colors;
  
  static const char* s_id_format;
  
  struct HSV { float h, s, v; };
  
  
  LIB_LOCAL InstructionColorChart(const cInstSet& inst_set);
  
public:
  LIB_LOCAL inline const Color& ColorOf(Instruction inst) const { return m_inst_colors[inst.GetOp()]; }
  
  LIB_LOCAL static InstructionColorChartPtr OfInstSetForGraphicsContext(const cInstSet& inst_set, GraphicsContext& gctx);
};




// Private::InstructionColorChart Implementation
// --------------------------------------------------------------------------------------------------------------  

const char* Private::InstructionColorChart::s_id_format = "_inst_color_chart[%s]";


Private::InstructionColorChart::InstructionColorChart(const cInstSet& inst_set)
{
  HSV class_colors[NUM_INST_CLASSES];
  
  // initialize starting colors
  for (int i = 0; i < NUM_INST_CLASSES; i++) {
    class_colors[i].h = (360.0f / NUM_INST_CLASSES) * (i + 1) - 7.0f;
    class_colors[i].s = 0.6f;
    class_colors[i].v = 1.0f;
  }
  
  m_inst_colors.Resize(inst_set.GetSize());
  
  for (int i = 0; i < inst_set.GetSize(); i++) {
    InstructionClass inst_class = inst_set.GetInstLib()->Get(inst_set.GetLibFunctionIndex(Instruction(i))).GetClass();
    m_inst_colors[i] = Color::WithHSV(class_colors[inst_class].h, class_colors[inst_class].s, class_colors[inst_class].v);
    class_colors[inst_class].v -= 0.04;
  }
}


Private::InstructionColorChartPtr Private::InstructionColorChart::OfInstSetForGraphicsContext(const cInstSet& inst_set,
                                                                                              GraphicsContext& gctx)
{
  Apto::String chart_id = Apto::FormatStr(s_id_format, (const char*)inst_set.GetInstSetName());
  InstructionColorChartPtr chart = gctx.GetDataForID<InstructionColorChart>(chart_id);
  if (!chart) {
    chart = InstructionColorChartPtr(new InstructionColorChart(inst_set));
    assert(chart);
    gctx.AttachDataWithID(chart, chart_id);
  }
  return chart;
}




// Private::SnapshotTracer
// --------------------------------------------------------------------------------------------------------------  

class Private::SnapshotTracer : public cHardwareTracer, public WorldDriver
{
private:
  cWorld* m_world;
  
  class NullFeedback : public Avida::Feedback
  {
    void Error(const char* fmt, ...) { (void)fmt; }
    void Warning(const char* fmt, ...) { (void)fmt; }
    void Notify(const char* fmt, ...) { (void)fmt; }
  } m_feedback;

  Apto::Array<HardwareSnapshot*>* m_snapshots;
  int m_snapshot_count;
  
  int m_genome_length;
  Instruction m_first_inst;
  int m_last_mem_space;
  int m_last_idx;
  GenomePtr m_genome;
  GenomePtr m_offspring_genome;
  

public:
  LIB_LOCAL inline SnapshotTracer(cWorld* world) : m_world(world), m_snapshots(NULL) { ; }
  
  LIB_LOCAL void TraceGenome(GenomePtr genome, Apto::Array<HardwareSnapshot*>& snapshots, double mut_rate, int seed);
  
  LIB_LOCAL GenomePtr OffspringGenome() { return m_offspring_genome; }
  
  // cHardwareTracer
  LIB_LOCAL void TraceHardware(cAvidaContext& ctx, cHardwareBase&, bool bonus = false, bool mini = false, int exec_success = -2);
  LIB_LOCAL void PrintSuccess(cOrganism* org, int exec_success = -2);
  LIB_LOCAL void TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism);
  
  
  // WorldDriver
  LIB_LOCAL void Pause() { ; }
  LIB_LOCAL void Finish() { ; }
  LIB_LOCAL void Abort(AbortCondition condition) { (void)condition; }
  
  LIB_LOCAL Avida::Feedback& Feedback() { return m_feedback; }
  
  LIB_LOCAL void RegisterCallback(DriverCallback callback) { (void)callback; }
};




// Private::SnapshotTracer Implementation
// --------------------------------------------------------------------------------------------------------------  

void Private::SnapshotTracer::TraceGenome(GenomePtr genome, Apto::Array<HardwareSnapshot*>& snapshots, double mut_rate, int seed)
{
  // Create internal reference to the snapshot array so that the tracing methods can create snapshots
  m_snapshots = &snapshots;
  m_snapshots->Resize(300);
  
  // Set up tracking objects and variables
  m_snapshot_count = 0;
  
  InstructionSequencePtr seq;
  seq.DynamicCastFrom(genome->Representation());
  m_genome_length = seq->GetSize();
  m_first_inst = (*seq)[0];
  
  m_last_mem_space = 0;
  m_last_idx = 0;
  
  m_genome = genome;
  
  
  // Setup context
  Apto::RNG::AvidaRNG rng(seed);
  cAvidaContext ctx(this, rng);
  
  // Create a test cpu
  cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU(ctx);
  
  // Setup test info to trace into this tracer
  HardwareTracerPtr thisPtr(this);
  this->AddReference();
  
  cCPUTestInfo test_info(1);
  test_info.MutationRates().SetCopyMutProb(mut_rate);
  test_info.UseRandomInputs();
  test_info.SetTraceExecution(thisPtr);
  
  // Test the actual genome
  testcpu->TestGenome(ctx, test_info, *genome);
  
  // Clear internal reference to the snapshot array
  m_snapshots = NULL;
  
  m_genome = GenomePtr();
}


void Private::SnapshotTracer::TraceHardware(cAvidaContext& ctx, cHardwareBase& hw, bool bonus, bool mini, int exec_success)
{
  (void)ctx;
  (void)bonus;
  (void)exec_success;
  
  if (mini) return;
  
  // Create snapshot based on current hardware state
  m_snapshot_count++;
  
  // Make sure snapshot array is big enough (just in case bonus cycles or threads are in use)
  if (m_snapshots->GetSize() < m_snapshot_count) m_snapshots->Resize(m_snapshots->GetSize() * 2);
  
  HardwareSnapshot* prev_snapshot = (m_snapshot_count > 1) ? (*m_snapshots)[m_snapshot_count - 2] : NULL;
  HardwareSnapshot* snapshot = new HardwareSnapshot(hw.GetNumRegisters(), prev_snapshot);
  (*m_snapshots)[m_snapshot_count - 1] = snapshot;
  
  snapshot->SetInstSet(hw.GetInstSet());
  
  // Store register states
  for (int reg = 0; reg < hw.GetNumRegisters(); reg++) snapshot->SetRegister(reg, hw.GetRegister(reg));
  
  Apto::Array<int> buffer_values;

  // Handle Input Buffer
  buffer_values.Resize(hw.GetInputBuf().GetCapacity());
  for (int i = 0; i < hw.GetInputBuf().GetCapacity(); i++) buffer_values[i] = hw.GetInputBuf()[i];
  snapshot->AddBuffer("input", buffer_values);
  
  // Handle Output Buffer
  buffer_values.Resize(hw.GetOutputBuf().GetCapacity());
  for (int i = 0; i < hw.GetOutputBuf().GetCapacity(); i++) buffer_values[i] = hw.GetOutputBuf()[i];
  snapshot->AddBuffer("output", buffer_values);
  
  // Handle Stacks
  buffer_values.Resize(nHardware::STACK_SIZE);
  for (int stk = 0; stk < hw.GetNumStacks(); stk++) {
    for (int i = 0; i < nHardware::STACK_SIZE; i++) buffer_values[i] = hw.GetStack(i, stk);
    snapshot->AddBuffer(Apto::FormatStr("stack %c", 'A' + stk), buffer_values);
  }
  snapshot->SetSelectedBuffer(Apto::FormatStr("stack %c", 'A' + hw.GetCurStack()));
  
  // Handle function counts
  const Apto::Array<int>& task_counts = hw.GetOrganism()->GetPhenotype().GetCurTaskCount();
  for (int i = 0; i < task_counts.GetSize(); i++) {
    snapshot->SetFunctionCount((const char*)m_world->GetEnvironment().GetTask(i).GetName(), task_counts[i]);
  }
  
  // Handle memory spaces
  Apto::Array<Instruction> memory;
  Apto::Array<bool> mutated;
  
  // - handle the genome part of the memory
  memory.Resize((m_genome_length < hw.GetMemory().GetSize()) ? m_genome_length : hw.GetMemory().GetSize());
  mutated.Resize(memory.GetSize());
  for (int i = 0; i < m_genome_length && i < hw.GetMemory().GetSize(); i++) {
    memory[i] = hw.GetMemory()[i];
    mutated[i] = hw.GetMemory().FlagMutated(i);
  }
  snapshot->AddMemSpace("genome", memory, mutated);
  
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
  mutated.Resize(memory.GetSize());
  mutated.SetAll(false);
  for (int i = m_genome_length; i < hw.GetMemory().GetSize(); i++) {
    memory[i - m_genome_length] = hw.GetMemory()[i];
    mutated[i - m_genome_length] = hw.GetMemory().FlagMutated(i);
  }
  
  // - determine the maximum position of any head
  int max_head_pos = 0;
  for (int i = 0; i < hw.GetNumHeads(); i++) {
    int head_pos = hw.GetHead(i).GetPosition();
    if (head_pos > max_head_pos) max_head_pos = head_pos;
  }
  
  // Hack to get fixed length Avida-ED organisms to show offspring as expected
  if (hw.GetHead(1).GetPosition() == m_genome_length) max_head_pos = hw.GetMemory().GetSize() - 1;
  
  // - if the maximum position is in the offspring part of the memory
  if (max_head_pos >= m_genome_length) {
    // truncate the offspring part of the memory to the position of the last head
    memory.Resize(max_head_pos - m_genome_length + 1);
    mutated.Resize(memory.GetSize());
    snapshot->AddMemSpace("offspring", memory, mutated);
    
    // handle all heads that are in the second part of the memory space
    for (int i = 0; i < hw.GetNumHeads(); i++) {
      Apto::String name = "FLOW";
      if (i == 0) name = "IP";
      if (i == 1) name = "READ";
      if (i == 2) name = "WRITE";
      if (hw.GetHead(i).GetPosition() >= m_genome_length) snapshot->AddHead(name, 1, hw.GetHead(i).GetPosition() - m_genome_length);
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


void Private::SnapshotTracer::TraceTestCPU(int time_used, int time_allocated, const cOrganism& organism)
{
  (void)time_used;
  (void)time_allocated;
  (void)organism;
  
  // Trace finished, cleanup...

  // Did the organism successfully reproduce before running out of time?
  if (time_used != time_allocated) {
    // Create snapshot based on current hardware state
    m_snapshot_count++;
    
    // Make sure snapshot array is big enough (just in case bonus cycles or threads are in use)
    if (m_snapshots->GetSize() < m_snapshot_count) m_snapshots->Resize(m_snapshots->GetSize() * 2);
    
    cHardwareBase& hw = const_cast<cHardwareBase&>(organism.GetHardware());
    
    HardwareSnapshot* prev_snapshot = (m_snapshot_count > 1) ? (*m_snapshots)[m_snapshot_count - 2] : NULL;
    HardwareSnapshot* snapshot = new HardwareSnapshot(hw.GetNumRegisters(), prev_snapshot);
    (*m_snapshots)[m_snapshot_count - 1] = snapshot;
    
    snapshot->SetInstSet(organism.GetHardware().GetInstSet());
    snapshot->SetPostDivide();
    
    // Store register states
    for (int reg = 0; reg < hw.GetNumRegisters(); reg++) snapshot->SetRegister(reg, hw.GetRegister(reg));
    
    Apto::Array<int> buffer_values;
    
    // Handle Input Buffer
    buffer_values.Resize(hw.GetInputBuf().GetCapacity());
    for (int i = 0; i < hw.GetInputBuf().GetCapacity(); i++) buffer_values[i] = hw.GetInputBuf()[i];
    snapshot->AddBuffer("input", buffer_values);
    
    // Handle Output Buffer
    buffer_values.Resize(hw.GetOutputBuf().GetCapacity());
    for (int i = 0; i < hw.GetOutputBuf().GetCapacity(); i++) buffer_values[i] = hw.GetOutputBuf()[i];
    snapshot->AddBuffer("output", buffer_values);
    
    // Handle Stacks
    buffer_values.Resize(nHardware::STACK_SIZE);
    for (int stk = 0; stk < hw.GetNumStacks(); stk++) {
      for (int i = 0; i < nHardware::STACK_SIZE; i++) buffer_values[i] = hw.GetStack(i, stk);
      snapshot->AddBuffer(Apto::FormatStr("stack %c", 'A' + stk), buffer_values);
    }
    snapshot->SetSelectedBuffer(Apto::FormatStr("stack %c", 'A' + hw.GetCurStack()));
    
    // Handle function counts
    const Apto::Array<int>& task_counts = organism.GetPhenotype().GetLastTaskCount();
    for (int i = 0; i < task_counts.GetSize(); i++) {
      snapshot->SetFunctionCount((const char*)m_world->GetEnvironment().GetTask(i).GetName(), task_counts[i]);
    }

    
    
    
    // Handle memory spaces
    Apto::Array<Instruction> memory;
    Apto::Array<bool> mutated;
    ConstInstructionSequencePtr seq;
    
    // - handle the genome part of the memory
    seq.DynamicCastFrom(m_genome->Representation());
    memory.Resize(seq->GetSize());
    mutated.Resize(memory.GetSize());
    mutated.SetAll(false);
    for (int i = 0; i < m_genome_length && i < seq->GetSize(); i++) {
      memory[i] = (*seq)[i];
    }
    snapshot->AddMemSpace("genome", memory, mutated);
    
    // - handle the offspring part of the memory
    m_offspring_genome = GenomePtr(new Genome(organism.OffspringGenome()));
    seq.DynamicCastFrom(organism.OffspringGenome().Representation());
    memory.Resize(seq->GetSize());
    mutated.Resize(memory.GetSize());
    mutated.SetAll(false);
    for (int i = 0; i < seq->GetSize(); i++) {
      memory[i] = (*seq)[i];
    }
    
    // Hack to get mutated state into the final state... potential for offset, etc.
    const Apto::Array<bool>& prev_mutated = prev_snapshot->MutatedStateOfMemSpace(1);
    for (int i = 0; i < mutated.GetSize() && i < prev_mutated.GetSize(); i++) {
      mutated[i] = prev_mutated[i];
    }
    snapshot->AddMemSpace("offspring", memory, mutated);
  }
  
  // Resize the snapshot array to the actual number of snapshots
  m_snapshots->Resize(m_snapshot_count);
}




// HardwareSnapshot Implementation
// --------------------------------------------------------------------------------------------------------------  

HardwareSnapshot::HardwareSnapshot(int num_regs, HardwareSnapshot* previous_snapshot)
: m_inst_set(NULL), m_registers(num_regs), m_post_divide(false)
{
  if (previous_snapshot) m_jumps = previous_snapshot->m_jumps;
}


HardwareSnapshot::~HardwareSnapshot()
{
}


void HardwareSnapshot::AddBuffer(const Apto::String& description, const Apto::Array<int>& values)
{
  m_buffers.Set(description, values);
}


void HardwareSnapshot::SetFunctionCount(const Apto::String& function, int count)
{
  m_function_counts.Set(function, count);
}


int HardwareSnapshot::AddMemSpace(const Apto::String& label, const Apto::Array<Instruction>& memory, const Apto::Array<bool>& mutated)
{
  int idx = m_mem_spaces.GetSize();
  m_mem_spaces.Resize(idx + 1);
  m_mem_spaces[idx].label = label;
  m_mem_spaces[idx].memory = memory;
  m_mem_spaces[idx].mutated = mutated;
  
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

Avida::Viewer::ConstGraphicPtr Avida::Viewer::HardwareSnapshot::GraphicForContext(GraphicsContext& gctx) const
{
  (void)gctx;
  
  GraphicPtr graphic(new Graphic);
  
  // Build the various graphic objects that need to be displayed.
  const double genome_spacing = (m_post_divide) ? 0.2 : 0.1;                 // Space between two genome circles.
  const double inst_radius = 0.1;                   // Radius of each instruction circle
  const double jump_freq_increment = 0.025;
  
  // Calculated constants, based on parameters set above
  const double inst_diameter = inst_radius * 2.0;    // Circumference of each instruction circle
  const double inst_spacing = inst_diameter * 1.05;  // How much room to leave for each instruction?
  const double PI = 3.14159265;
  const double angular_offset = PI / 2.0;            // 1/4 angle offset to line up memory space starting points

  
  Private::InstructionColorChartPtr inst_color_chart =
    Private::InstructionColorChart::OfInstSetForGraphicsContext(*m_inst_set, gctx);
  assert(inst_color_chart);

  
  // Draw arcs showing prior execution path (do first so that they are layered below other objects)
  for (int jump_idx = 0; jump_idx < m_jumps.GetSize(); jump_idx++) {
    const Jump& jmp = m_jumps[jump_idx];
    if (jmp.from_mem_space == jmp.to_mem_space) {
      // Intra-memory space jump, handle internal arcs
      
      const MemSpace& cur_memspace = m_mem_spaces[jmp.from_mem_space];
      const Apto::Array<Instruction>& cur_mem = cur_memspace.memory;
      const int cur_length = cur_mem.GetSize();
      
      const double genome_circumference = ((double) cur_length) * inst_spacing;
      const double angle_step = 2.0*PI / (double) cur_length;
      const double genome_radius = genome_circumference / (2.0*PI);
      const double genome_offset = genome_radius + genome_spacing;
      
      // Setup the central position for this memory space.  For the moment, assume we only have parent and offspring.
      double center_x = 0.0;
      double center_y = 0.0;
      if (jmp.from_mem_space == 0) center_x -= genome_offset; else center_x += genome_offset;
      
      // Center position + angular offset - radius (since we need the lower, left corner of each circle to draw)
      double from_inst_angle = angle_step * (double) jmp.from_idx;
      double to_inst_angle = angle_step * (double) jmp.to_idx;
      
      // rotate in opposite 1/4 angles based on mem_space, if post_divide rotate both the same direction
      from_inst_angle += (jmp.from_mem_space == 0 || m_post_divide) ? angular_offset : -angular_offset;
      to_inst_angle += (jmp.from_mem_space == 0 || m_post_divide) ? angular_offset : -angular_offset;
      
      
      float x = center_x + sin(from_inst_angle) * (genome_radius - inst_radius);
      float y = center_y + cos(from_inst_angle) * (genome_radius - inst_radius);
      float x2 = center_x + sin(to_inst_angle) * (genome_radius - inst_radius);
      float y2 = center_y + cos(to_inst_angle) * (genome_radius - inst_radius);
      
      GraphicObject* arc_go = new GraphicObject(x, y, x2, y2, SHAPE_CURVE);
      
      // Set control points
      double freq_offset = jmp.freq * jump_freq_increment + inst_radius;
      double freq_radius = genome_radius - inst_radius - freq_offset;
      if (freq_radius < inst_radius) freq_radius = inst_radius;
      
      arc_go->ctrl_x = center_x + sin(from_inst_angle) * freq_radius;
      arc_go->ctrl_y = center_y + cos(from_inst_angle) * freq_radius;
      arc_go->ctrl_x2 = center_x + sin(to_inst_angle) * freq_radius;
      arc_go->ctrl_y2 = center_y + cos(to_inst_angle) * freq_radius;
      
      if (jmp.to_idx < jmp.from_idx) {
        arc_go->line_color = Color::RED();
      } else {
        arc_go->line_color = Color::BLACK();
      }
      
      graphic->AddObject(arc_go);
    } else {
      // Cross memory space jumps... do nothing for now.
      // @TODO
    }
  }
  
  
  
  // Draw each memory space
  int num_mem_spaces = m_mem_spaces.GetSize();
  for (int cur_mem_id = 0; cur_mem_id < 2 && cur_mem_id < num_mem_spaces; cur_mem_id++) {
    const MemSpace& cur_memspace = m_mem_spaces[cur_mem_id];
    const Apto::Array<Instruction>& cur_mem = cur_memspace.memory;
    const int cur_length = cur_mem.GetSize();

    const double genome_circumference = ((double) cur_length) * inst_spacing;
    const double angle_step = 2.0*PI / (double) cur_length;
    const double genome_radius = genome_circumference / (2.0*PI);
    const double genome_offset = genome_radius + genome_spacing;
    const double head_radius = genome_radius - 2.0 * inst_radius;

    // Setup the central position for this memory space.  For the moment, assume we only have parent and offspring.
    double center_x = 0.0;
    double center_y = 0.0;
    if (cur_mem_id == 0) center_x -= genome_offset; else center_x += genome_offset;
    
    GraphicObject* mem_center = new GraphicObject(center_x, center_y, 0, 0, SHAPE_OVAL);
    mem_center->active_region_id = cur_mem_id;
    graphic->AddObject(mem_center);

    // Step through the genome, placing each instruction on the screen;
    for (int cur_inst_idx = 0; cur_inst_idx < cur_length; cur_inst_idx++) {
      // Calculate where this instruction should be drawn on the screen.
      // Center position + angular offset - radius (since we need the lower, left corner of each circle to draw)
      double cur_angle = angle_step * (double) cur_inst_idx;
      
      // rotate in opposite 1/4 angles based on mem_space, if post_divide rotate both the same direction
      cur_angle += (cur_mem_id == 0 || m_post_divide) ? angular_offset : -angular_offset;
      
      float inst_x = center_x + sin(cur_angle) * genome_radius - inst_radius;
      float inst_y = center_y + cos(cur_angle) * genome_radius - inst_radius;

      GraphicObject* inst_go = new GraphicObject(inst_x, inst_y, inst_diameter, inst_diameter, SHAPE_OVAL);
      
      inst_go->label = cur_mem[cur_inst_idx].AsString();
      inst_go->fill_color = inst_color_chart->ColorOf(cur_mem[cur_inst_idx]);
      
      // Determine what, if any, heads are pointing at this instruction
      Color& line_color = inst_go->line_color;
      inst_go->line_width = 2.0;
      for (Apto::Map<Apto::String, int>::ConstIterator it = cur_memspace.heads.Begin(); it.Next(); ) {
        if (*it.Get()->Value2() == cur_inst_idx) {
          Apto::String head = it.Get()->Value1();
          
          float head_x = center_x + sin(cur_angle) * head_radius - inst_radius;
          float head_y = center_y + cos(cur_angle) * head_radius - inst_radius;
          GraphicObject* head_go = new GraphicObject(head_x, head_y, inst_diameter, inst_diameter, SHAPE_OVAL);
          head_go->fill_color = Color::DARKGRAY();
          head_go->fill_color.a = 0.8;
          
          
          if (head == "IP") {
            line_color = Color::BLACK();
            inst_go->line_width = 3.0;
            head_go->label = "I";
          } else if (head == "FLOW") {
            line_color.g = 1.0;
            line_color.a = 1.0;
            head_go->label = "F";
            head_go->label_color = Color::GREEN();
          } else if (head == "READ") {
            line_color.b = 1.0;
            line_color.a = 1.0;
            head_go->label = "R";
            head_go->label_color = Color::BLUE();
          } else if (head == "WRITE") {
            line_color.r = 1.0;
            line_color.a = 1.0;
            head_go->label = "W";
            head_go->label_color = Color::RED();
          } else {
            // Unknown head
            delete head_go;
            continue;
          }
          
          graphic->AddObject(head_go);
        }
      }
      
      // if mutated, highlight border as such
      if (cur_memspace.mutated[cur_inst_idx]) {
        line_color = Color::GREEN();
        inst_go->line_width = 3.0;
      }
      
      graphic->AddObject(inst_go);
    }
  }
  
  
  // @CAO Draw other hardware as needed
  
  return graphic;
}

const Apto::Array<bool>& Avida::Viewer::HardwareSnapshot::MutatedStateOfMemSpace(int idx) const
{
  return m_mem_spaces[idx].mutated;
}



// OrganismTrace Implementation
// --------------------------------------------------------------------------------------------------------------  

OrganismTrace::OrganismTrace(cWorld* world, GenomePtr genome, double mut_rate, int seed)
  : m_genome(genome)
{
  Private::SnapshotTracer tracer(world);
  tracer.TraceGenome(genome, m_snapshots, mut_rate, seed);
  m_offspring_genome = tracer.OffspringGenome();
}


OrganismTrace::~OrganismTrace()
{
  for (int i = 0; i < m_snapshots.GetSize(); i++) delete m_snapshots[i];
}
