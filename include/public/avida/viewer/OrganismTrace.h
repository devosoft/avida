/*
 *  viewer/OrganismTrace.h
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

#ifndef AvidaViewerOrganismTrace_h
#define AvidaViewerOrganismTrace_h

#include "avida/core/Genome.h"
#include "avida/core/InstructionSequence.h"
#include "avida/viewer/Graphic.h"
#include "avida/viewer/Types.h"

class cInstSet;
class cWorld;


namespace Avida {
  namespace Viewer {    
    
    // HardwareSnapshot
    // --------------------------------------------------------------------------------------------------------------  
    
    class HardwareSnapshot
    {
    private:
      const cInstSet* m_inst_set;
      
      Apto::Array<int> m_registers;
      Apto::Map<Apto::String, Apto::Array<int> > m_buffers;
      Apto::String m_selected_buffer;
      Apto::Array<int> m_default_buffer;
      Apto::Map<Apto::String, int> m_function_counts;
      
      struct MemSpace
      {
        Apto::String label;
        Apto::Array<Instruction> memory;
        Apto::Array<bool> mutated;
        Apto::Map<Apto::String, int> heads;
      };
      Apto::Array<MemSpace, Apto::ManagedPointer> m_mem_spaces;
      
      struct Jump
      {
        int from_mem_space;
        int from_idx;
        int to_mem_space;
        int to_idx;
        int freq;
        
        LIB_LOCAL inline Jump() : from_mem_space(-1), from_idx(0), to_mem_space(-1), to_idx(0), freq(0) { ; }
        LIB_LOCAL inline Jump(int fm, int fi, int tm, int ti, int f)
          : from_mem_space(fm), from_idx(fi), to_mem_space(tm), to_idx(ti), freq(f) { ; }
      };
      Apto::Array<Jump, Apto::Smart> m_jumps;
      
      Instruction m_next_inst;
      
      bool m_post_divide;
      
    public:
      LIB_EXPORT HardwareSnapshot(int num_regs, HardwareSnapshot* previous_snapshot = NULL);
      LIB_EXPORT ~HardwareSnapshot();
      
      
      // Definition Methods
      LIB_LOCAL inline void SetInstSet(const cInstSet& inst_set) { m_inst_set = &inst_set; }
      LIB_LOCAL inline void SetPostDivide() { m_post_divide = true; }
      
      LIB_LOCAL inline void SetRegister(int idx, int value) { m_registers[idx] = value; }
      LIB_LOCAL void AddBuffer(const Apto::String& description, const Apto::Array<int>& values);
      LIB_LOCAL void SetFunctionCount(const Apto::String& function, int count);
      LIB_LOCAL int AddMemSpace(const Apto::String& label, const Apto::Array<Instruction>& memory, const Apto::Array<bool>& mutated);
      LIB_LOCAL void AddHead(const Apto::String& label, int mem_space, int index);
      LIB_LOCAL void AddJump(int from_mem_space, int from_idx, int to_mem_space, int to_idx);
      LIB_LOCAL inline void SetNextInst(Instruction inst) { m_next_inst = inst; }
      LIB_LOCAL void SetSelectedBuffer(const Apto::String& buffer) { m_selected_buffer = buffer; }


      // Access Methods
      LIB_EXPORT inline const cInstSet& InstSet() const { return *m_inst_set; }
      LIB_EXPORT inline bool IsPostDivide() const { return m_post_divide; }
      LIB_EXPORT inline int NumRegisters() const { return m_registers.GetSize(); }
      LIB_EXPORT inline int Register(int idx) const { return m_registers[idx]; }
      
      LIB_EXPORT inline int NumBuffers() const { return m_buffers.GetSize(); };
      LIB_EXPORT inline const Apto::Array<int>& Buffer(const Apto::String& desc) const { return m_buffers.GetWithDefault(desc, m_default_buffer); }
      LIB_EXPORT inline const Apto::String& SelectedBuffer() const { return m_selected_buffer; }
      
      LIB_EXPORT inline int FunctionCount(const Apto::String& function) const { return m_function_counts.GetWithDefault(function, 0); };
      
      
      LIB_EXPORT inline Instruction NextInstruction() const { return m_next_inst; }
      
      
      LIB_EXPORT ConstGraphicPtr GraphicForContext(GraphicsContext& gctx) const;
      
      
      // Internal Access Methods
      LIB_LOCAL const Apto::Array<bool>& MutatedStateOfMemSpace(int idx) const;
    };
    

    // OrganismTrace
    // --------------------------------------------------------------------------------------------------------------  
    
    class OrganismTrace
    {      
    private:
      GenomePtr m_genome;
      Apto::Array<HardwareSnapshot*> m_snapshots;
      GenomePtr m_offspring_genome;
      
    public:
      LIB_EXPORT OrganismTrace(cWorld* world, GenomePtr genome, double mut_rate = 0.0, int seed = -1);
      LIB_EXPORT ~OrganismTrace();
      
      LIB_EXPORT inline ConstGenomePtr OrganismGenome() const { return m_genome; }
      LIB_EXPORT inline ConstGenomePtr OffspringGenome() const { return m_offspring_genome; }
      
      LIB_EXPORT inline int SnapshotCount() const { return m_snapshots.GetSize(); }
      LIB_EXPORT inline const HardwareSnapshot& Snapshot(int idx) const { return *m_snapshots[idx]; }      
    };
    
  };
};

#endif
