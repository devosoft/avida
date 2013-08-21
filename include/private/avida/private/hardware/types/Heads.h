/*
 *  hardware/types/Heads.h
 *  avida-core
 *
 *  Created by David on 8/19/13.
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

#ifndef AvidaHardwareTypesHeads_h
#define AvidaHardwareTypesHeads_h

#include "avida/hardware/InstArchCPU.h"
#include "avida/hardware/InstLib.h"
#include "avida/hardware/InstMemSpace.h"
#include "avida/util/NopSequence.h"


namespace Avida {
  namespace Hardware {
    namespace Types {
      
      // Hardware::InstArchCPU
      // --------------------------------------------------------------------------------------------------------------
      
      class Heads : public InstArchCPU
      {
      private:
        typedef bool (Heads::*InstMethod)(Context& ctx);
        
      private:
        // --------  Structure Constants  --------
        static const int NUM_REGISTERS = 3;
        static const int NUM_HEADS = 4;
        enum tRegisters { REG_AX = 0, REG_BX, REG_CX };
        enum { HEAD_IP = 0, HEAD_READ, HEAD_WRITE, HEAD_FLOW };
        static const int NUM_NOPS = 3;
        static const int STACK_SIZE = 10;
        
        // --------  Data Structures  --------
        class HeadsInst;
        class HeadsInstLib;
        
        class Stack
        {
        private:
          int m_stack[STACK_SIZE];
          int m_sp;
          
        public:
          inline Stack() : m_sp(0) { Clear(); }
          inline Stack(const Stack& is) : m_sp(is.m_sp) { Clear(); for (int i = 0; i < STACK_SIZE; i++) m_stack[i] = is.m_stack[i]; }
          
          inline void operator=(const Stack& is) { m_sp = is.m_sp; Clear(); for (int i = 0; i < STACK_SIZE; i++) m_stack[i] = is.m_stack[i]; }
          
          inline void Push(int value) { if (--m_sp < 0) m_sp = STACK_SIZE - 1; m_stack[(int)m_sp] = value; }
          inline int Pop() { int v = m_stack[m_sp]; m_stack[m_sp] = 0; if (++m_sp == STACK_SIZE) m_sp = 0; return v; }
          inline int& Peek() { return m_stack[m_sp]; }
          inline const int& Peek() const { return m_stack[m_sp]; }
          inline const int& Get(int d = 0) const { assert(d >= 0); int p = d + m_sp; return m_stack[(p >= STACK_SIZE) ? (p - STACK_SIZE) : p]; }
          inline void Clear() { for (int i = 0; i < STACK_SIZE; i++) m_stack[i] = 0; }
        };
        
        class Head
        {
        protected:
          Heads* m_hw;
          int m_pos;
          
          void fullAdjust(int mem_size = -1);
          
        public:
          inline Head(Heads* hw = NULL, int pos = 0) : m_hw(hw), m_pos(pos) { ; }
          
          inline void Reset(Heads* hw, int pos) { m_hw = hw; m_pos = pos; }
          
          inline InstMemSpace& GetMemory() { return  m_hw->m_memory; }
          
          inline void Adjust();
          
          inline int Position() const { return m_pos; }
          
          inline void SetPosition(int pos) { m_pos = pos; Adjust(); }
          inline void Set(int pos) { m_pos = pos; Adjust(); }
          inline void Set(const Head& head) { m_pos = head.m_pos; }
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
        
        
        struct Thread
        {
        private:
          int m_id;
          
        public:
          int reg[NUM_REGISTERS];
          Head heads[NUM_HEADS];
          Stack stack;
          unsigned char cur_stack;              // 0 = local stack, 1 = global stack.
          unsigned char cur_head;
          
          Util::NopSequence read_label;
          Util::NopSequence next_label;
          
          
          inline Thread(Heads* in_hardware = NULL, int in_id = -1) { Reset(in_hardware, in_id); }
          inline ~Thread() { ; }
          
          void operator=(const Thread& in_thread);
          
          void Reset(Heads* in_hardware, int in_id);
          int GetID() const { return m_id; }
          void SetID(int in_id) { m_id = in_id; }
        };
        
        
        // --------  Static Variables  --------
        static HeadsInstLib* s_inst_slib;
        
        
        // --------  Member Variables  --------
        const InstMethod* m_functions;
        
        InstMemSpace m_memory;    // Memory...
        Stack m_global_stack;     // A stack that all threads share.
        
        Apto::Array<Thread> m_threads;
        int m_thread_id_chart;
        int m_cur_thread;
        
        // Flags...
        struct {
          bool m_mal_active:1;         // Has an allocate occured since last divide?
          bool m_advance_ip:1;         // Should the IP advance after this instruction?
          bool m_spec_die:1;
          
          bool m_thread_slicing_parallel:1;
          bool m_no_cpu_cycle_time:1;
          
          bool m_slip_read_head:1;
        };
        
      public:
        
        
        static InstLib* InstructionLibrary() { return s_inst_slib; }
        
        
      private:
        
        static HeadsInstLib* getInstLib() { return s_inst_slib; }

        bool SingleProcess_ExecuteInst(Context& ctx, const Instruction& cur_inst);
        
        // --------  Stack Manipulation...  --------
        inline void StackPush(int value);
        inline int StackPop();
        inline void StackClear();
        inline void SwitchStack();
        
        
        // --------  Head Manipulation (including IP)  --------
        inline Head& GetActiveHead() { return m_threads[m_cur_thread].heads[m_threads[m_cur_thread].cur_head]; }
        void AdjustHeads();
        
        
        // --------  Label Manipulation  -------
        const Util::NopSequence& GetLabel() const { return m_threads[m_cur_thread].next_label; }
        Util::NopSequence& GetLabel() { return m_threads[m_cur_thread].next_label; }
        void ReadLabel(int max_size = -1);
        Head FindLabel(int direction);
        int FindLabel_Forward(const Util::NopSequence & search_label, const InstructionSequence& search_genome, int pos);
        int FindLabel_Backward(const Util::NopSequence & search_label, const InstructionSequence& search_genome, int pos);
        Head FindLabel(const Util::NopSequence & in_label, int direction);
        void FindLabelInMemory(const Util::NopSequence& label, Head& search_head);
        
        const Util::NopSequence& GetReadLabel() const { return m_threads[m_cur_thread].read_label; }
        Util::NopSequence& GetReadLabel() { return m_threads[m_cur_thread].read_label; }
        
        
        // --------  Thread Manipulation  -------
        bool ForkThread(); // Adds a new thread based off of m_cur_thread.
        bool InterruptThread(int interruptType); // Create a new thread that interrupts the current thread
        bool KillThread(); // Kill the current thread!
        inline void ThreadPrev(); // Shift the current thread in use.
        
        // --------  Stack Manipulation...  --------
        inline int GetStack(int depth=0, int stack_id=-1, int in_thread=-1) const;
        inline int GetCurStack(int in_thread = -1) const;
        inline int GetNumStacks() const { return 2; }

        // ---------- Instruction Helpers -----------
        int FindModifiedRegister(int default_register);
        int FindModifiedNextRegister(int default_register);
        int FindModifiedPreviousRegister(int default_register);
        int FindModifiedHead(int default_head);
        int FindNextRegister(int base_reg);
        
        inline const Head& getHead(int head_id) const { return m_threads[m_cur_thread].heads[head_id]; }
        inline Head& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
        inline const Head& getHead(int head_id, int thread) const { return m_threads[thread].heads[head_id]; }
        inline Head& getHead(int head_id, int thread) { return m_threads[thread].heads[head_id];}
        
        inline const Head& getIP() const { return m_threads[m_cur_thread].heads[HEAD_IP]; }
        inline Head& getIP() { return m_threads[m_cur_thread].heads[HEAD_IP]; }
        inline const Head& getIP(int thread) const { return m_threads[thread].heads[HEAD_IP]; }
        inline Head& getIP(int thread) { return m_threads[thread].heads[HEAD_IP]; }
        
        
        bool Allocate_Necro(const int new_size);
        bool Allocate_Random(Context& ctx, const int old_size, const int new_size);
        bool Allocate_Default(const int new_size);
        bool Allocate_Main(Context& ctx, const int allocated_size);
        
        
        void internalReset();
        
        void internalResetOnFailedDivide();
        
        
        int calcCopiedSize(const int parent_size, const int child_size);
        
        bool Divide_Main(Context& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
        bool Divide_MainRS(Context& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
        
        bool HeadCopy_ErrorCorrect(Context& ctx, double reduction);
        bool Inst_HeadDivideMut(Context& ctx, double mut_multiplier = 1);
        
        void ReadInst(const int in_inst);
        
        int FindModifiedResource(Context& ctx, int& spec_id);
        bool DoCollect(Context& ctx, bool env_remove, bool internal_add, bool probabilistic, bool unit);
        bool DoActualCollect(Context& ctx, int bin_used, bool env_remove, bool internal_add, bool probabilistic, bool unit);


      private:
        // ---------- Instruction Library -----------
        
        bool Inst_Nop(Context& ctx);
        
        // Multi-threading
        bool Inst_ForkThread(Context& ctx);
        bool Inst_ForkThreadLabel(Context& ctx);
        bool Inst_ForkThreadLabelIf0(Context& ctx);
        bool Inst_ForkThreadLabelIfNot0(Context& ctx);
        bool Inst_KillThread(Context& ctx);
        bool Inst_ThreadID(Context& ctx);
        
        // Flow Control
        bool Inst_SetHead(Context& ctx);
        bool Inst_AdvanceHead(Context& ctx);
        bool Inst_MoveHead(Context& ctx);
        bool Inst_JumpHead(Context& ctx);
        bool Inst_GetHead(Context& ctx);
        bool Inst_SetFlow(Context& ctx);

        bool Inst_SearchF(Context& ctx);
        bool Inst_SearchB(Context& ctx);
        bool Inst_MemSize(Context& ctx);
        

        // Conditionals
        bool Inst_If0(Context& ctx);
        bool Inst_IfEqu(Context& ctx);
        bool Inst_IfNot0(Context& ctx);
        bool Inst_IfNEqu(Context& ctx);
        bool Inst_IfGtr0(Context& ctx);
        bool Inst_IfGtr(Context& ctx);
        bool Inst_IfGtrEqu0(Context& ctx);
        bool Inst_IfGtrEqu(Context& ctx);
        bool Inst_IfLess0(Context& ctx);
        bool Inst_IfLess(Context& ctx);
        bool Inst_IfLessEqu0(Context& ctx);
        bool Inst_IfLessEqu(Context& ctx);
        bool Inst_IfGtrX(Context& ctx);
        bool Inst_IfEquX(Context& ctx);
        
        // Single-Argument Math
        bool Inst_ShiftR(Context& ctx);
        bool Inst_ShiftL(Context& ctx);
        bool Inst_Inc(Context& ctx);
        bool Inst_Dec(Context& ctx);

        bool Inst_Not(Context& ctx);
        bool Inst_Neg(Context& ctx);
        bool Inst_Square(Context& ctx);
        bool Inst_Sqrt(Context& ctx);
        bool Inst_Log(Context& ctx);
        bool Inst_Log10(Context& ctx);
        
        // Double Argument Math
        bool Inst_Add(Context& ctx);
        bool Inst_Sub(Context& ctx);
        bool Inst_Mult(Context& ctx);
        bool Inst_Div(Context& ctx);
        bool Inst_Mod(Context& ctx);
        bool Inst_Nand(Context& ctx);
        bool Inst_Or(Context& ctx);
        bool Inst_Nor(Context& ctx);
        bool Inst_And(Context& ctx);
        bool Inst_Order(Context& ctx);
        bool Inst_Xor(Context& ctx);
        
        // Values
        bool Inst_SetNum(Context& ctx);
        bool Inst_ValGrey(Context& ctx);
        bool Inst_ValDir(Context& ctx);
        bool Inst_ValAddP(Context& ctx);
        bool Inst_ValFib(Context& ctx);
        bool Inst_ValPolyC(Context& ctx);
        bool Inst_AllOnes(Context& ctx);
        bool Inst_Zero(Context& ctx);
        bool Inst_One(Context& ctx);
        
        // Bit-setting instructions
        bool Inst_Setbit(Context& ctx);
        bool Inst_Clearbit(Context& ctx);
        
        // Stack and Register Operations
        bool Inst_Pop(Context& ctx);
        bool Inst_Push(Context& ctx);
        bool Inst_HeadPop(Context& ctx);
        bool Inst_HeadPush(Context& ctx);
        
        bool Inst_SwitchStack(Context& ctx);
        bool Inst_FlipStack(Context& ctx);
        bool Inst_Swap(Context& ctx);
        bool Inst_CopyReg(Context& ctx);
        bool Inst_Reset(Context& ctx);
        
        // Biological
        bool Inst_HeadAlloc(Context& ctx);
        bool Inst_HeadRead(Context& ctx);
        bool Inst_HeadWrite(Context& ctx);
        bool Inst_HeadCopy(Context& ctx);
        bool Inst_HeadSearch(Context& ctx);
        bool Inst_IfLabel(Context& ctx);
        
        bool Inst_HeadDivide(Context& ctx);
        bool Inst_HeadDivideRS(Context& ctx);
        bool Inst_HeadDivideSex(Context& ctx);
        bool Inst_HeadDivideAsex(Context& ctx);
        bool Inst_HeadDivideAsexWait(Context& ctx);
        bool Inst_HeadDivideMateSelect(Context& ctx);

        bool Inst_Repro(Context& ctx);
        bool Inst_ReproSex(Context& ctx);

        bool Inst_Sterilize(Context& ctx);
        
        bool Inst_Die(Context& ctx);
        bool Inst_Poison(Context& ctx);
        bool Inst_Suicide(Context& ctx);
        
        // I/O
        bool Inst_TaskInput(Context& ctx);
        bool Inst_TaskOutput(Context& ctx);
        bool Inst_TaskStackInput(Context& ctx);
        bool Inst_TaskStackOutput(Context& ctx);
        bool Inst_TaskIO(Context& ctx);
        
        // Resources
        bool Inst_Collect(Context& ctx);
        bool Inst_Destroy(Context& ctx);
        bool Inst_NopCollect(Context& ctx);
        bool Inst_CollectUnitProbabilistic(Context& ctx);
        bool Inst_CollectSpecific(Context& ctx);
        bool Inst_IfResources(Context& ctx);
        
        
      private:
        static HeadsInstLib* initInstLib();
        
        // ---------- Some Instruction Helpers -----------
        bool DoLookAheadEX(Context& ctx, bool use_ft = false);
        bool DoLookAgainEX(Context& ctx, bool use_ft = false);
        
      private:
        class HeadsInst : public InstLib::Entry
        {
        private:
          const InstMethod m_function;
          
        public:
          HeadsInst(const Apto::String& name, InstMethod function, InstructionClass _class, unsigned int flags,
                    const Apto::String& desc)
          : Entry(name, _class, flags, desc), m_function(function)
          {
          }
          
          InstMethod Function() const { return m_function; }
        };
        
        
        class HeadsInstLib : public InstLib
        {
        private:
          const HeadsInst* m_entries;
          Apto::String* m_nopmod_names;
          const int* m_nopmods;
          const InstMethod* m_functions;
          
        public:
          HeadsInstLib(int size, const HeadsInst* entries, Apto::String* nopmod_names, const int* nopmods,
                       const InstMethod* functions, int def, int null_inst)
          : InstLib(size, def, null_inst), m_entries(entries), m_nopmod_names(nopmod_names), m_nopmods(nopmods)
          , m_functions(functions)
          {
            for (int i = 0; i < m_size; i++) m_namemap.Set((const char*)m_entries[i].Name(), i);
          }
          
          const InstMethod* Functions() const { return m_functions; }
          
          const Entry& Get(int i) const { assert(i < m_size); return m_entries[i]; }
          
          const Apto::String& GetNopName(const unsigned int idx) { return m_nopmod_names[idx]; }
          int GetNopMod(const unsigned int idx) { return m_nopmods[idx]; }
        };
      };
      
      
      
      inline void Heads::ThreadPrev()
      {
        if (m_cur_thread == 0) m_cur_thread = m_threads.GetSize() - 1;
        else m_cur_thread--;
      }
      
      inline void Heads::StackPush(int value)
      {
        if (m_threads[m_cur_thread].cur_stack == 0) {
          m_threads[m_cur_thread].stack.Push(value);
        } else {
          m_global_stack.Push(value);
        }
      }
      
      inline int Heads::StackPop()
      {
        int pop_value;
        
        if (m_threads[m_cur_thread].cur_stack == 0) {
          pop_value = m_threads[m_cur_thread].stack.Pop();
        } else {
          pop_value = m_global_stack.Pop();
        }
        
        return pop_value;
      }
      
      inline int Heads::GetStack(int depth, int stack_id, int in_thread) const
      {
        int value = 0;
        
        if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
        
        if (stack_id == -1) stack_id = m_threads[in_thread].cur_stack;
        
        if (stack_id == 0) value = m_threads[in_thread].stack.Get(depth);
        else if (stack_id == 1) value = m_global_stack.Get(depth);
        
        return value;
      }
      
      inline int Heads::GetCurStack(int in_thread) const
      {
        if(in_thread >= m_threads.GetSize() || in_thread < 0) in_thread = m_cur_thread;
        
        return m_threads[in_thread].cur_stack;
      }
      
      
      inline void Heads::StackClear()
      {
        if (m_threads[m_cur_thread].cur_stack == 0) {
          m_threads[m_cur_thread].stack.Clear();
        } else {
          m_global_stack.Clear();
        }
      }
      
      inline void Heads::SwitchStack()
      {
        m_threads[m_cur_thread].cur_stack++;
        if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
      }

      
    };
  };
};

#endif
