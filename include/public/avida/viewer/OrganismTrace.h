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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaViewerOrganismTrace_h
#define AvidaViewerOrganismTrace_h

#include "avida/core/Genome.h"
#include "avida/core/InstructionSequence.h"
#include "avida/viewer/Types.h"

class cWorld;


namespace Avida {
  namespace Viewer {    
    
    // HardwareSnapshot
    // --------------------------------------------------------------------------------------------------------------  
    
    class HardwareSnapshot
    {
    public:
      class GraphicObject;
      
    private:
      Apto::Array<int> m_registers;
      Apto::Map<Apto::String, Apto::Array<int> > m_buffers;
      Apto::Array<int> m_default_buffer;
      Apto::Map<Apto::String, int> m_function_counts;
      
      struct MemSpace
      {
        Apto::String label;
        Apto::Array<Instruction> memory;
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
      
      mutable Apto::Array<GraphicObject*> m_graphic_objects;
      mutable bool m_layout;
      
      
    public:
      LIB_EXPORT HardwareSnapshot(int num_regs, HardwareSnapshot* previous_snapshot = NULL);
      LIB_EXPORT ~HardwareSnapshot();
      
      
      // Definition Methods
      LIB_LOCAL inline void SetRegister(int idx, int value) { m_registers[idx] = value; }
      LIB_LOCAL void AddBuffer(const Apto::String& description, const Apto::Array<int>& values);
      LIB_LOCAL void SetFunctionCount(const Apto::String& function, int count);
      LIB_LOCAL int AddMemSpace(const Apto::String& label, const Apto::Array<Instruction>& memory);
      LIB_LOCAL void AddHead(const Apto::String& label, int mem_space, int index);
      LIB_LOCAL void AddJump(int from_mem_space, int from_idx, int to_mem_space, int to_idx);
      LIB_LOCAL inline void SetNextInst(Instruction inst) { m_next_inst = inst; }


      // Access Methods
      LIB_EXPORT inline int NumRegisters() const { return m_registers.GetSize(); }
      LIB_EXPORT inline int Register(int idx) const { return m_registers[idx]; }
      
      LIB_EXPORT inline int NumBuffers() const { return m_buffers.GetSize(); };
      LIB_EXPORT inline const Apto::Array<int>& Buffer(const Apto::String& desc) const { return m_buffers.GetWithDefault(desc, m_default_buffer); }
      
      LIB_EXPORT inline int FunctionCount(const Apto::String& function) const { return m_function_counts.GetWithDefault(function, 0); };
      
      
      LIB_EXPORT inline Instruction NextInstruction() const { return m_next_inst; }
      
      
      
      
      
      LIB_EXPORT inline int NumGraphicObjects() const { if (!m_layout) doLayout(); return m_graphic_objects.GetSize(); }
      LIB_EXPORT inline const GraphicObject& Object(int idx) const { if (!m_layout) doLayout(); return *m_graphic_objects[idx]; }
      
      
    public:
      class GraphicObject
      {
      public:
        struct Color
        {
          float r, g, b, a;  // Red, Green, Blue, and Alpha (opacity), all 0.0 to 1.0.
          Color(float _r, float _g, float _b, float _a=0.0) : r(_r), g(_g), b(_b), a(_a) { ; }

          static inline Color NONE()  { return Color(0.0, 0.0, 0.0, 0.0); }
          static inline Color BLACK() { return Color(0.0, 0.0, 0.0, 1.0); }
          static inline Color WHITE() { return Color(1.0, 1.0, 1.0, 1.0); }
          static inline Color RED()   { return Color(1.0, 0.0, 0.0, 1.0); }
          static inline Color GREEN() { return Color(0.0, 1.0, 0.0, 1.0); }
          static inline Color BLUE()  { return Color(0.0, 0.0, 1.0, 1.0); }
       };

        // Bounding box
        float x, y;           // 1.0 = approximately 1 inch, unzoomed (72 pixels); screen center = (0,0) 
        float width, height;
        
        // Shape
        enum GraphicShape { SHAPE_NONE, SHAPE_OVAL, SHAPE_RECT, SHAPE_LINE } shape;
        Color fill_color;

        // Line
        float line_width;   // 1.0 is standard 1px
        Color line_color;

        // Label
        Apto::String label;
        float font_size;    // Relative value; 1.0 is default.
        Color label_color;
                
        // Shape specific details
        union {
          struct {  // For SHAPE_OVAL
            float start_angle;  // In radians (0 to 2PI); both zero indicate full circle.
            float end_angle;
          };
          struct {  // For SHAPE_RECT
            float x_round; // Radius for rounded corners.  Zero inidcated square corners.
            float y_round;
          };
        };
        
        // Is this graphic object an active region?
        int active_region_id;  // -1 is not active

      public:
        GraphicObject(float _x, float _y, float _width, float _height, GraphicShape _shape=SHAPE_NONE)
          : x(_x), y(_y), width(_width), height(_height), shape(_shape),
            fill_color(Color::NONE()), line_width(1.0), line_color(Color::BLACK()),
            label_color(Color::BLACK()), font_size(1.0), start_angle(0.0), end_angle(0.0)
        { ; }
        ~GraphicObject() { ; }
      };
      
    private:
      LIB_EXPORT void doLayout() const;
    };
    

    // OrganismTrace
    // --------------------------------------------------------------------------------------------------------------  
    
    class OrganismTrace
    {      
    private:
      GenomePtr m_genome;
      Apto::Array<HardwareSnapshot*> m_snapshots;
      
    public:
      LIB_EXPORT OrganismTrace(cWorld* world, GenomePtr genome);
      LIB_EXPORT ~OrganismTrace();
      
      LIB_EXPORT inline ConstGenomePtr OrganismGenome() const { return m_genome; }
      
      LIB_EXPORT inline int SnapshotCount() const { return m_snapshots.GetSize(); }
      LIB_EXPORT inline const HardwareSnapshot& Snapshot(int idx) const { return *m_snapshots[idx]; }      
    };
    
  };
};

#endif
