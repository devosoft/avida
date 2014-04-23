/*
 *  hardware/types/GP8.h
 *  avida-core
 *
 *  Created by David on 7/24/13.
 *  Copyright 2013 Michigan State University. All rights reserved.
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

#ifndef AvidaHardwareTypesGP8_h
#define AvidaHardwareTypesGP8_h

#include "avida/hardware/InstArchCPU.h"
#include "avida/hardware/InstLib.h"
#include "avida/hardware/InstMemSpace.h"
#include "avida/util/NopSequence.h"

#include "avida/private/hardware/features/VisualSensor.h"


namespace Avida {
  namespace Hardware {
    namespace Types {
    
      // Hardware::InstArchCPU
      // --------------------------------------------------------------------------------------------------------------
      
      class GP8 : public InstArchCPU
      {
      private:
        typedef bool (GP8::*InstMethod)(Context& ctx);
        typedef int (GP8::*ImmMethod)(Context& ctx);
        
      private:
        // --------  Structure Constants  --------
        static const int NUM_REGISTERS = 8;
        static const int NUM_BEHAVIORS = 3; // num inst types capable of storing their own data
        static const int NUM_HEADS = NUM_REGISTERS;
        enum { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX };
        enum { hIP = 0, hREAD, hWRITE, hFLOW, hFLOW2, hFLOW3, hFLOW4, hFLOW5 };
        enum { uREAD = 0x1, uWRITE = 0x2, uATTACK = 0x4 };
        enum { aEAT, aMOVE, aROTATE };
        static const int NUM_NOPS = NUM_REGISTERS;
        static const int MAX_MEM_SPACES = NUM_NOPS;
        
        
        // --------  Data Structures  --------
        class GP8Inst;
        class GP8InstLib;
        
        struct AttackRegisters {
          int success_reg;
          int bonus_reg;
          int bin_reg;
        };
        
        
        // --------  Static Variables  --------
        static GP8InstLib* s_inst_slib;
        
        
      private:
        // --------  Define Internal Data Structures  --------
        struct DataValue
        {
          int value;
          
          // Actual age of this value
          unsigned int originated:15;
          unsigned int from_env:1;
          
          // Age of the oldest component used to create this value
          unsigned int oldest_component:15;
          unsigned int env_component:1;
          
          inline DataValue() { Clear(); }
          inline void Clear() { value = 0; originated = 0; from_env = 0, oldest_component = 0; env_component = 0; }
          inline DataValue& operator=(const DataValue& i);
        };
        
        class Head
        {
        protected:
          GP8* m_hw;
          int m_pos;
          unsigned int m_ms:31;
          bool m_is_gene:1;
          
          void fullAdjust(int mem_size = -1);
          
        public:
          inline Head(GP8* hw = NULL, int pos = 0, unsigned int ms = 0, bool is_gene = false)
          : m_hw(hw), m_pos(pos), m_ms(ms), m_is_gene(is_gene) { ; }
          
          inline void Reset(GP8* hw, int pos, unsigned int ms, bool is_gene)
          { m_hw = hw; m_pos = pos; m_ms = ms; m_is_gene = is_gene; }
          
          inline InstMemSpace& GetMemory() { return (m_is_gene) ? m_hw->m_genes[m_ms].memory : m_hw->m_mem_array[m_ms]; }
          
          inline void Adjust();
          
          inline unsigned int MemSpaceIndex() const { return m_ms; }
          inline bool MemSpaceIsGene() const { return m_is_gene; }
          
          inline int Position() const { return m_pos; }
          
          inline void SetPosition(int pos) { m_pos = pos; Adjust(); }
          inline void Set(int pos, unsigned int ms, bool is_gene) { m_pos = pos; m_ms = ms; m_is_gene = is_gene; Adjust(); }
          inline void Set(const Head& head) { m_pos = head.m_pos; m_ms = head.m_ms; m_is_gene = head.m_is_gene; }
          inline void SetAbsPosition(int new_pos) { m_pos = new_pos; }
          
          inline void Jump(int jump) { m_pos += jump; Adjust(); }
          inline void AbsJump(int jump) { m_pos += jump; }
          
          inline void Advance() { m_pos++; Adjust(); }
          
          inline const Instruction& GetInst() { return GetMemory()[m_pos]; }
          inline const Instruction& GetInst(int offset) { return GetMemory()[m_pos + offset]; }
          inline Instruction NextInst();
          inline Instruction PrevInst();
          
          inline void SetInst(const Instruction& value) { GetMemory()[m_pos] = value; }
          inline void InsertInst(const Instruction& inst) { GetMemory().Insert(m_pos, inst); }
          inline void RemoveInst() { GetMemory().Remove(m_pos); }
          
          inline void SetFlagCopied() { return GetMemory().SetFlagCopied(m_pos); }
          inline void SetFlagMutated() { return GetMemory().SetFlagMutated(m_pos); }
          inline void SetFlagExecuted() { return GetMemory().SetFlagExecuted(m_pos); }
          inline void SetFlagPointMut() { return GetMemory().SetFlagPointMut(m_pos); }
          inline void SetFlagCopyMut() { return GetMemory().SetFlagCopyMut(m_pos); }
          
          inline void ClearFlagCopied() { return GetMemory().ClearFlagCopied(m_pos); }
          inline void ClearFlagMutated() { return GetMemory().ClearFlagMutated(m_pos); }
          inline void ClearFlagExecuted() { return GetMemory().ClearFlagExecuted(m_pos); }
          inline void ClearFlagPointMut() { return GetMemory().ClearFlagPointMut(m_pos); }
          inline void ClearFlagCopyMut() { return GetMemory().ClearFlagCopyMut(m_pos); }
          
          // Operator Overloading...
          inline Head& operator++() { m_pos++; Adjust(); return *this; }
          inline Head& operator--() { m_pos--; Adjust(); return *this; }
          inline Head& operator++(int) { return operator++(); }
          inline Head& operator--(int) { return operator--(); }
          inline int operator-(const Head& rhs) { return m_pos - rhs.m_pos; }
          inline bool operator==(const Head& rhs) const;
          inline bool operator!=(const Head& rhs) const { return !operator==(rhs); }
          
          // Bool Tests...
          inline bool AtFront() { return (m_pos == 0); }
          inline bool AtEnd() { return (m_pos + 1 == GetMemory().GetSize()); }
          inline bool InMemory() { return (m_pos >= 0 && m_pos < GetMemory().GetSize()); }
        };
        
        
        class Stack
        {
        private:
          int m_sz;
          DataValue* m_stack;
          int m_sp;
          
        public:
          Stack() : m_sz(0), m_stack(NULL), m_sp(0) { ; }
          inline Stack(const Stack& is) : m_sp(is.m_sp) { Clear(is.m_sz); for (int i = 0; i < m_sz; i++) m_stack[i] = is.m_stack[i]; }
          ~Stack() { delete [] m_stack; }
          
          inline void operator=(const Stack& is) { m_sp = is.m_sp; Clear(is.m_sz); for (int i = 0; i < m_sz; i++) m_stack[i] = is.m_stack[i]; }
          
          inline void Push(const DataValue& value) { if (--m_sp < 0) m_sp = m_sz - 1; m_stack[(int)m_sp] = value; }
          inline DataValue Pop() { DataValue v = m_stack[(int)m_sp]; m_stack[(int)m_sp].Clear(); if (++m_sp == m_sz) m_sp = 0; return v; }
          inline DataValue& Peek() { return m_stack[(int)m_sp]; }
          inline const DataValue& Peek() const { return m_stack[(int)m_sp]; }
          inline const DataValue& Get(int d = 0) const { assert(d >= 0); int p = d + m_sp; return m_stack[(p >= m_sz) ? (p - m_sz) : p]; }
          inline void Clear(int sz) { delete [] m_stack; m_sz = sz; m_stack = new DataValue[sz]; }
        };
        
        
        
        
        struct Thread
        {
        public:
          Util::NopSequence thread_label;
          
          DataValue reg[NUM_REGISTERS];
          Head heads[NUM_HEADS];
          Stack stack;
          
          struct {
            unsigned int cur_stack:1;
            bool reading_label:1;
            bool reading_seq:1;
            bool running:1;
            bool active:1;
            bool wait_greater:1;
            bool wait_equal:1;
            bool wait_less:1;
            int wait_reg:4;
            unsigned int wait_dst:3;
          };
          int wait_value;
          
          Util::NopSequence read_label;
          Util::NopSequence read_seq;
          Util::NopSequence next_label;
          
          Features::VisualSensor::LookSettings sensor_session;
          
          inline Thread() { ; }
          inline ~Thread() { ; }
          
          void Reset(GP8* in_hardware, const Head& start_pos);
          
        private:
          Thread(const Thread& thread);
          void operator=(const Thread& thread);
        };
        
        struct Gene
        {
          InstMemSpace memory;
          Util::NopSequence label;
          int thread_id;
        };
        
        
      private:
        // --------  Member Variables  --------
        const InstMethod* m_functions;
        const unsigned int* m_hw_units;
        const ImmMethod* m_imm_methods;
        
        // Genes
        Apto::Array<Gene> m_genes;
        
        // Memory
        Apto::Array<InstMemSpace, Apto::ManagedPointer> m_mem_array;
        char m_mem_ids[MAX_MEM_SPACES];
        
        // Stacks
        Stack m_global_stack;     // A stack that all threads share.
        
        // Threads
        Apto::Array<Thread, Apto::ManagedPointer> m_threads;
        int m_cur_thread;
        
        
        Apto::Array<Features::VisualSensor::LookSettings> m_sensor_sessions;
        
        // Flags
        struct {
          unsigned int m_cycle_count:16;
          unsigned int m_last_output:16;
          
          unsigned int m_cur_uop:8;
          
          int m_cur_offspring:5;
          
          bool m_hw_queue_eat:1;    // Is an eat queued?
          bool m_hw_queue_move:1;   // Is a move queued?
          bool m_hw_queue_rotate:1; // Is a rotate queued?
          
          unsigned int m_hw_queue_rotate_num:3; // Queued number of rotations
          bool m_hw_queue_rotate_reverse:1;     // Should rotates be reverse (negative)
          
          bool m_advance_ip:1;  // Should the IP advance after this instruction?
          bool m_spec_stall:1;
          bool m_spec_die:1;
          bool m_hw_reset:1;
          
          bool m_no_cpu_cycle_time:1;
          bool m_slip_read_head:1;
          bool m_juv_enabled:1;
        };
        
        unsigned int m_waiting_threads;
        unsigned int m_running_threads;
        
        unsigned int m_hw_busy;
        
        char m_hw_queue[3];
        int m_hw_queued;
        
        Apto::Array<int> m_hw_queue_eat_threads;
        
        
        Apto::Array<int>* m_action_side_effect_queue;

      
      public:
        
        static InstLib* InstructionLibrary() { return s_inst_slib; }
      
      
      private:
        
        static GP8InstLib* getInstLib() { return s_inst_slib; }
        
        // --------  Core Execution Methods  --------
        bool executeInst(Context& ctx, const Instruction& cur_inst);
        void internalReset();
        void internalResetOnFailedDivide();
        void setupGenes();
        
        
        // --------  Stack Manipulation  --------
        inline DataValue stackPop();
        inline Stack& getStack(int stack_id);
        inline void switchStack();
        
        
        // --------  Label Manipulation  -------
        inline const Util::NopSequence& GetLabel() const { return m_threads[m_cur_thread].next_label; }
        inline Util::NopSequence& GetLabel() { return m_threads[m_cur_thread].next_label; }
        void readLabel(Head& head, Util::NopSequence& label);
        
        void FindLabelStart(Head& head, Head& default_pos, bool mark_executed);
        void FindLabelForward(Head& head, Head& default_pos, bool mark_executed);
        void FindLabelBackward(Head& head, Head& default_pos, bool mark_executed);
        void FindNopSequenceStart(Head& head, Head& default_pos, bool mark_executed);
        void FindNopSequenceForward(Head& head, Head& default_pos, bool mark_executed);
        void FindNopSequenceBackward(Head& head, Head& default_pos, bool mark_executed);
        inline const Util::NopSequence& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
        inline const Util::NopSequence& GetReadSequence() const { return m_threads[m_cur_thread].read_seq; }
        inline Util::NopSequence& GetReadLabel() { return m_threads[m_cur_thread].read_label; }
        inline Util::NopSequence& GetReadSequence() { return m_threads[m_cur_thread].read_seq; }
        
        
        // --------  Thread Manipulation  -------
        void threadCreate(const Util::NopSequence& thread_label, const Head& start_pos);
        
        
        // ---------- Instruction Helpers -----------
        int FindModifiedRegister(int default_register, bool accept_immediate = false);
        int FindModifiedNextRegister(int default_register, bool accept_immediate = false);
        int FindModifiedPreviousRegister(int default_register, bool accept_immediate = false);
        int FindModifiedHead(int default_head);
        int FindNextRegister(int base_reg);
        int FindUpstreamModifiedRegister(int offset, int default_register);
        
        int calcCopiedSize(const int parent_size, const int child_size);
        
        inline Head& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
        inline Head& getIP() { return m_threads[m_cur_thread].heads[hIP]; }
        
        inline int getRegister(Context& ctx, int reg_id);
        inline DataValue getRegisterData(Context& ctx, int reg_id);
        
        
        // --------  Division Support  -------
        bool Divide_Main(Context& ctx, int mem_space, int position, double mut_multiplier=1);
        
        
        // ---------- Utility Functions -----------
        inline void setRegister(int reg_num, int value, bool from_env = false);
        inline void setRegister(int reg_num, int value, const DataValue& src);
        inline void setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2);
        void checkWaitingThreads(int cur_thread, int reg_num);
        
        void ReadInst(Instruction in_inst);
        
        
        // ---------- Instruction Library -----------
        
        bool Inst_Nop(Context& ctx);
        
        // Multi-threading
        bool Inst_RegulatePause(Context& ctx);
        bool Inst_RegulateResume(Context& ctx);
        bool Inst_RegulateReset(Context& ctx);
        bool Inst_WaitCondition_Equal(Context& ctx);
        bool Inst_WaitCondition_Less(Context& ctx);
        bool Inst_WaitCondition_Greater(Context& ctx);
        bool Inst_Yield(Context& ctx);
        
        // Flow-Control
        bool Inst_SetMemory(Context& ctx);
        bool Inst_MoveHead(Context& ctx);
        bool Inst_JumpHead(Context& ctx);
        bool Inst_GetHead(Context& ctx);
        bool Inst_Search_Label_S(Context& ctx);
        bool Inst_Search_Label_D(Context& ctx);
        bool Inst_Search_Seq_D(Context& ctx);
        bool Inst_Label(Context& ctx);
        
        // Standard Conditionals
        bool Inst_IfNEqu(Context& ctx);
        bool Inst_IfLess(Context& ctx);
        bool Inst_IfNotZero(Context& ctx);
        bool Inst_IfEqualZero(Context& ctx);
        bool Inst_IfGreaterThanZero(Context& ctx);
        bool Inst_IfLessThanZero(Context& ctx);
        
        // Single-Argument Math
        bool Inst_ShiftR(Context& ctx);
        bool Inst_ShiftL(Context& ctx);
        bool Inst_Inc(Context& ctx);
        bool Inst_Dec(Context& ctx);
        
        // Double Argument Math
        bool Inst_Add(Context& ctx);
        bool Inst_Sub(Context& ctx);
        bool Inst_Nand(Context& ctx);
        
        bool Inst_Mult(Context& ctx);
        bool Inst_Div(Context& ctx);
        bool Inst_Mod(Context& ctx);
        
        // Values
        bool Inst_Zero(Context& ctx);
        bool Inst_One(Context& ctx);
        bool Inst_MaxInt(Context& ctx);
        bool Inst_Rand(Context& ctx);
        
        int Val_Zero(Context& ctx);
        int Val_One(Context& ctx);
        int Val_MaxInt(Context& ctx);
        int Val_Rand(Context& ctx);
        
        // Stack Operations
        bool Inst_Pop(Context& ctx);
        bool Inst_Push(Context& ctx);
        bool Inst_PopAll(Context& ctx);
        bool Inst_PushAll(Context& ctx);
        bool Inst_SwitchStack(Context& ctx);
        bool Inst_Swap(Context& ctx);
        
        // I/O
        bool Inst_TaskInput(Context& ctx);
        bool Inst_TaskOutput(Context& ctx);
        
        // Reproductive Operation
        bool Inst_HeadRead(Context& ctx);
        bool Inst_HeadWrite(Context& ctx);
        bool Inst_HeadCopy(Context& ctx);
        bool Inst_DivideMemory(Context& ctx);
        bool Inst_DidCopyLabel(Context& ctx);
        
        bool Inst_Repro(Context& ctx);
        
        bool Inst_Die(Context& ctx);
        
        // Movement and Navigation
        bool Inst_Move(Context& ctx);
        bool Inst_GetNorthOffset(Context& ctx);
        
        // Rotation
        bool Inst_RotateX(Context& ctx);
        bool Inst_RotateOrgID(Context& ctx);
        bool Inst_RotateAwayOrgID(Context& ctx);
        
        // Resource and Topography Sensing
        bool Inst_SetForageTarget(Context& ctx);
        bool Inst_SetForageTargetOnce(Context& ctx);
        bool Inst_SetRandForageTargetOnce(Context& ctx);
        bool Inst_GetForageTarget(Context& ctx);
        
        bool Inst_SenseResourceID(Context& ctx);
        bool Inst_SenseNest(Context& ctx);
        bool Inst_SenseFacedHabitat(Context& ctx);
        bool Inst_LookAheadEX(Context& ctx);
        bool Inst_LookAgainEX(Context& ctx);
        
        bool Inst_Eat(Context& ctx);
        
        // Collection
        bool Inst_CollectSpecific(Context& ctx);
        bool Inst_GetResStored(Context& ctx);
        
        // Org Interactions
        bool Inst_GetFacedOrgID(Context& ctx);
        
        bool Inst_TeachOffspring(Context& ctx);
        bool Inst_LearnParent(Context& ctx);
        
        // Predator-Prey Instructions
        bool Inst_AttackPrey(Context& ctx);
        
        // Control-type Instructions
        bool Inst_ScrambleReg(Context& ctx);
        
      private:
        static GP8InstLib* initInstLib();
        
        // ---------- Some Instruction Helpers -----------
        bool DoLookAheadEX(Context& ctx, bool use_ft = false);
        bool DoLookAgainEX(Context& ctx, bool use_ft = false);
        
      private:
        class GP8Inst : public InstLib::Entry
        {
        private:
          const InstMethod m_function;
          const unsigned int m_hw_units;
          const ImmMethod m_imm_method;
          
        public:
          GP8Inst(const Apto::String& name, InstMethod function, InstructionClass _class, unsigned int flags,
                  const Apto::String& desc, unsigned int hw_units, ImmMethod imm_method = NULL)
          : Entry(name, _class, flags, desc)
          , m_function(function), m_hw_units(hw_units), m_imm_method(imm_method)
          {
          }
          
          InstMethod Function() const { return m_function; }
          unsigned int HWUnits() const { return m_hw_units; }
          ImmMethod ImmediateMethod() const { return m_imm_method; }
        };
        
        
        class GP8InstLib : public InstLib
        {
        private:
          const GP8Inst* m_entries;
          Apto::String* m_nopmod_names;
          const int* m_nopmods;
          const InstMethod* m_functions;
          const unsigned int* m_hw_units;
          const ImmMethod* m_imm_methods;
          
        public:
          GP8InstLib(int size, const GP8Inst* entries, Apto::String* nopmod_names, const int* nopmods, const InstMethod* functions,
                     const unsigned int* hw_units, const ImmMethod* imm_methods, int def, int null_inst)
          : InstLib(size, def, null_inst), m_entries(entries), m_nopmod_names(nopmod_names), m_nopmods(nopmods)
          , m_functions(functions), m_hw_units(hw_units), m_imm_methods(imm_methods)
          {
            for (int i = 0; i < m_size; i++) m_namemap.Set((const char*)m_entries[i].Name(), i);
          }
          
          const InstMethod* Functions() const { return m_functions; }
          const unsigned int* HWUnits() const { return m_hw_units; }
          const ImmMethod* ImmediateMethods() const { return m_imm_methods; }
          
          const Entry& Get(int i) const { assert(i < m_size); return m_entries[i]; }
          
          const Apto::String& GetNopName(const unsigned int idx) { return m_nopmod_names[idx]; }
          int GetNopMod(const unsigned int idx) { return m_nopmods[idx]; }
        };
        
        
        class ThreadLabelIterator
        {
        private:
          GP8* m_hw;
          const Util::NopSequence m_label;
          bool m_is_specific;
          int m_next_idx;
          int m_cur_id;
          
        public:
          ThreadLabelIterator(GP8* hw, const Util::NopSequence& label, bool specific = true)
          : m_hw(hw), m_label(label), m_is_specific(specific), m_next_idx(0), m_cur_id(-1) { ; }
          
          inline int Next()
          {
            for (; m_next_idx < m_hw->m_threads.GetSize(); m_next_idx++) {
              const Util::NopSequence& thread_label = m_hw->m_threads[m_next_idx].next_label;
              if ((m_is_specific && m_label == thread_label) || thread_label.Contains(m_label)) {
                m_cur_id = m_next_idx;
                m_next_idx++;
                return m_cur_id;
              }
            }
            m_cur_id = -1;
            return m_cur_id;
          }
          
          inline int Get() const { return m_cur_id; }
          inline void Reset() { m_next_idx = 0; m_cur_id = -1; }
        };
        
      };
      
      
      inline GP8::DataValue& GP8::DataValue::operator=(const DataValue& i)
      {
        value = i.value;
        originated = i.originated;
        from_env = i.from_env;
        oldest_component = i.oldest_component;
        env_component = i.env_component;
        return *this;
      }
      
      
      
      
      inline void GP8::Head::Adjust()
      {
        const int mem_size = GetMemory().GetSize();
        
        // If we are still in range, stop here!
        if (m_pos >= 0 && m_pos < mem_size) return;
        
        // If the memory is gone, just stick it at the begining of its parent.
        if (mem_size == 0 || m_pos < 0) {
          m_pos = 0;
          return;
        }
        
        // position back at the begining of the memory as necessary.
        if (m_pos < (2 * mem_size)) m_pos -= mem_size;
        else m_pos %= mem_size;
      }
      
      
      inline bool GP8::Head::operator==(const Head& rhs) const
      {
        return m_hw == rhs.m_hw && m_pos == rhs.m_pos && m_ms == rhs.m_ms && m_is_gene == rhs.m_is_gene;
      }
      
      inline Instruction GP8::Head::PrevInst()
      {
        return (AtFront()) ? GetMemory()[GetMemory().GetSize() - 1] : GetMemory()[m_pos - 1];
      }
      
      inline Instruction GP8::Head::NextInst()
      {
        return (AtEnd()) ? m_hw->m_instset->InstError() : GetMemory()[m_pos + 1];
      }
      
      
      
      
      inline int GP8::getRegister(Context& ctx, int reg_id)
      {
        if (reg_id >= NUM_REGISTERS) {
          return (this->*(m_imm_methods[reg_id]))(ctx);
        }
        return m_threads[m_cur_thread].reg[reg_id].value;
      }
      
      inline GP8::DataValue GP8::getRegisterData(Context& ctx, int reg_id)
      {
        if (reg_id >= NUM_REGISTERS) {
          DataValue dv;
          dv.originated = m_cycle_count;
          dv.value = (this->*(m_imm_methods[reg_id]))(ctx);
          return dv;
        }
        return m_threads[m_cur_thread].reg[reg_id];
      }
      
      
      inline GP8::DataValue GP8::stackPop()
      {
        if (m_threads[m_cur_thread].cur_stack == 0) {
          return m_threads[m_cur_thread].stack.Pop();
        } else {
          return m_global_stack.Pop();
        }
      }
      
      inline GP8::Stack& GP8::getStack(int stack_id)
      {
        if (stack_id == 0) {
          return m_threads[m_cur_thread].stack;
        } else {
          return m_global_stack;
        }
      }
      
      inline void GP8::switchStack()
      {
        m_threads[m_cur_thread].cur_stack++;
        if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
      }
      
      
      inline void GP8::setRegister(int reg_num, int value, bool from_env)
      {
        if (reg_num > NUM_REGISTERS) return;
        
        DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
        dest.value = value;
        dest.from_env = from_env;
        dest.originated = m_cycle_count;
        dest.oldest_component = m_cycle_count;
        dest.env_component = from_env;
        if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
      }
      
      inline void GP8::setRegister(int reg_num, int value, const DataValue& src)
      {
        if (reg_num > NUM_REGISTERS) return;
        
        DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
        dest.value = value;
        dest.from_env = false;
        dest.originated = m_cycle_count;
        dest.oldest_component = src.oldest_component;
        dest.env_component = src.env_component;
        if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
      }
      
      inline void GP8::setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2)
      {
        if (reg_num > NUM_REGISTERS) return;
        
        DataValue& dest = m_threads[m_cur_thread].reg[reg_num];
        dest.value = value;
        dest.from_env = false;
        dest.originated = m_cycle_count;
        dest.oldest_component = (op1.oldest_component < op2.oldest_component) ? op1.oldest_component : op2.oldest_component;
        dest.env_component = (op1.env_component || op2.env_component);
        if (m_waiting_threads) checkWaitingThreads(m_cur_thread, reg_num);
      }
      
    };
  };
};

#endif
