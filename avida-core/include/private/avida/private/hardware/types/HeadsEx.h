/*
 *  hardware/types/HeadsEX.h
 *  avida-core
 *
 *  Created by David on 1/9/14.
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

#ifndef AvidaHardwareTypesHeadsEX_h
#define AvidaHardwareTypesHeadsEX_h

#include "avida/hardware/InstArchCPU.h"
#include "avida/hardware/InstLib.h"
#include "avida/hardware/InstMemSpace.h"
#include "avida/util/NopSequence.h"


namespace Avida {
  namespace Hardware {
    namespace Types {
      
      // Hardware::InstArchCPU
      // --------------------------------------------------------------------------------------------------------------
      
      class HeadsEX : public InstArchCPU
      {
      private:
        typedef bool (Heads::*InstMethod)(Context& ctx);
        
      private:
        // --------  Structure Constants  --------
        static const int NUM_REGISTERS = 8;
        static const int NUM_HEADS = NUM_REGISTERS < 4 ? 4 : NUM_REGISTERS;
        enum tRegisters { rAX = 0, rBX, rCX, rDX, rEX, rFX, rGX, rHX, rIX, rJX, rKX, rLX, rMX, rNX, rOX, rPX};
        enum { HEAD_IP = 0, HEAD_READ, HEAD_WRITE, HEAD_FLOW };
        static const int NUM_NOPS = NUM_REGISTERS;
        
        
        // --------  Static Variables  --------
        static StaticTableInstLib<cHardwareExperimental::tMethod>* s_inst_slib;
        static StaticTableInstLib<cHardwareExperimental::tMethod>* initInstLib(void);
        
        
        // --------  Define Internal Data Structures  --------
        struct DataValue
        {
          int value;
          
          // Actual age of this value
          unsigned int originated:14;
          unsigned int from_env:1;
          unsigned int from_sensor:1;
          
          // Age of the oldest component used to create this value
          unsigned int oldest_component:14;
          unsigned int env_component:1;
          unsigned int sensor_component:1;
          
          inline DataValue() { Clear(); }
          inline void Clear() { value = 0; originated = 0; from_env = 0, from_sensor = 0, oldest_component = 0; env_component = 0, sensor_component = 0; }
          inline DataValue& operator=(const DataValue& i);
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
          DataValue reg[NUM_REGISTERS];
          Head heads[NUM_HEADS];
          Stack stack;
          unsigned char cur_stack;              // 0 = local stack, 1 = global stack.
          unsigned char cur_head;
          
          struct {
            bool reading_label:1;
            bool reading_seq:1;
            bool active:1;
            bool wait_greater:1;
            bool wait_equal:1;
            bool wait_less:1;
            unsigned int wait_reg:4;
            unsigned int wait_dst:4;
          };
          int wait_value;
          
          Util::NopSequence read_label;
          Util::NopSequence read_seq;
          Util::NopSequence next_label;
          
          inline cLocalThread() { ; }
          cLocalThread(cHardwareExperimental* in_hardware, int in_id = -1) { Reset(in_hardware, in_id); }
          ~cLocalThread() { ; }
          
          void operator=(const cLocalThread& in_thread);
          void Reset(cHardwareExperimental* in_hardware, int in_id);
          inline int GetID() const { return m_id; }
          inline void SetID(int in_id) { m_id = in_id; }
        };
        
        
        // --------  Member Variables  --------
        const tMethod* m_functions;
        
        InstMemSpace m_memory;          // Memory...
        Stack m_global_stack;     // A stack that all threads share.
        
        Apto::Array<cLocalThread, Apto::ManagedPointer> m_threads;
        int m_thread_id_chart;
        int m_cur_thread;
        
        int m_use_avatar;
        bool m_from_sensor;
        
        struct {
          unsigned int m_cycle_count:16;
          unsigned int m_last_output:16;
        };
        
        // Flags
        struct {
          bool m_mal_active:1;         // Has an allocate occured since last divide?
          bool m_advance_ip:1;         // Should the IP advance after this instruction?
          bool m_spec_die:1;
          
          bool m_thread_slicing_parallel:1;
          bool m_no_cpu_cycle_time:1;
          
          bool m_slip_read_head:1;
          
          unsigned int m_waiting_threads:4;
        };
        
      public:
        
        
        static InstLib* InstructionLibrary() { return s_inst_slib; }
        
        
      private:
        
        static HeadsInstLib* getInstLib() { return s_inst_slib; }

        bool executeInst(Context& ctx, const Instruction& cur_inst);
        void internalReset();
        void internalResetOnFailedDivide();

        // --------  Stack Manipulation  --------
        inline DataValue stackPop();
        inline Stack& getStack(int stack_id);
        inline void switchStack();

        // --------  Head Manipulation (including IP)  --------
        void adjustHeads();

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
        bool forkThread(); // Adds a new thread based off of m_cur_thread.
        bool threadCreate(const Head& start_pos); // Adds a new thread starting at the flow head
        bool exitThread(); // Kill the current thread!

        // ---------- Instruction Helpers -----------
        int FindModifiedRegister(int default_register);
        int FindModifiedNextRegister(int default_register);
        int FindModifiedPreviousRegister(int default_register);
        int FindModifiedHead(int default_head);
        int FindNextRegister(int base_reg);
        
        bool Allocate_Necro(const int new_size);
        bool Allocate_Random(Context& ctx, const int old_size, const int new_size);
        bool Allocate_Default(const int new_size);
        bool Allocate_Main(Context& ctx, const int allocated_size);
        
        int calcCopiedSize(const int parent_size, const int child_size);

        inline Head& getHead(int head_id) { return m_threads[m_cur_thread].heads[head_id];}
        inline Head& getIP() { return m_threads[m_cur_thread].heads[hIP]; }

        // --------  Division Support  -------
        bool Divide_Main(Context& ctx, const int divide_point, const int extra_lines=0, double mut_multiplier=1);
        
        
        // ---------- Utility Functions -----------
        inline void setRegister(int reg_num, int value, bool from_env = false);
        inline void setRegister(int reg_num, int value, const DataValue& src);
        inline void setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2);
        void checkWaitingThreads(int cur_thread, int reg_num);
        
        void ReadInst(Instruction in_inst);

        // ---------- Instruction Library -----------
        
        bool Inst_Nop(Context& ctx);
        
        // Multi-threading
        bool Inst_ForkThread(Context& ctx);
        bool Inst_ThreadCreate(Context& ctx);
        bool Inst_ExitThread(Context& ctx);
        bool Inst_IdThread(Context& ctx);
        
        // Flow Control
        bool Inst_IfNEqu(Context& ctx);
        bool Inst_IfLess(Context& ctx);
        bool Inst_IfNotZero(Context& ctx);
        bool Inst_IfEqualZero(Context& ctx);
        bool Inst_IfGreaterThanZero(Context& ctx);
        bool Inst_IfLessThanZero(Context& ctx);
        bool Inst_IfGtrX(Context& ctx);
        bool Inst_IfEquX(Context& ctx);
        bool Inst_IfConsensus(Context& ctx);
        bool Inst_IfConsensus24(Context& ctx);
        bool Inst_IfLessConsensus(Context& ctx);
        bool Inst_IfLessConsensus24(Context& ctx);
        bool Inst_IfStackGreater(Context& ctx);
        bool Inst_IfNest(Context& ctx);
        bool Inst_Label(Context& ctx);
        
        // Stack and Register Operations
        bool Inst_Pop(Context& ctx);
        bool Inst_Push(Context& ctx);
        bool Inst_PopAll(Context& ctx);
        bool Inst_PushAll(Context& ctx);
        bool Inst_SwitchStack(Context& ctx);
        bool Inst_SwapStackTop(Context& ctx);
        bool Inst_Swap(Context& ctx);
        
        // Single-Argument Math
        bool Inst_ShiftR(Context& ctx);
        bool Inst_ShiftL(Context& ctx);
        bool Inst_Inc(Context& ctx);
        bool Inst_Dec(Context& ctx);
        bool Inst_Zero(Context& ctx);
        bool Inst_One(Context& ctx);
        bool Inst_Rand(Context& ctx);
        bool Inst_Mult100(Context& ctx);
        
        // Double Argument Math
        bool Inst_Add(Context& ctx);
        bool Inst_Sub(Context& ctx);
        bool Inst_Mult(Context& ctx);
        bool Inst_Div(Context& ctx);
        bool Inst_Mod(Context& ctx);
        bool Inst_Nand(Context& ctx);
        
        // I/O and Sensory
        bool Inst_TaskIO(Context& ctx);
        bool Inst_TaskInput(Context& ctx);
        bool Inst_TaskOutput(Context& ctx);
        bool Inst_TaskOutputZero(Context& ctx);
        
        // Head-based Instructions
        bool Inst_HeadAlloc(Context& ctx);
        bool Inst_MoveHead(Context& ctx);
        bool Inst_MoveHeadIfNEqu(Context& ctx);
        bool Inst_MoveHeadIfLess(Context& ctx);
        bool Inst_Goto(Context& ctx);
        bool Inst_GotoIfNEqu(Context& ctx);
        bool Inst_GotoIfLess(Context& ctx);
        bool Inst_GotoConsensus(Context& ctx);
        bool Inst_GotoConsensus24(Context& ctx);
        bool Inst_JumpHead(Context& ctx);
        bool Inst_GetHead(Context& ctx);
        bool Inst_IfCopiedCompLabel(Context& ctx);
        bool Inst_IfCopiedDirectLabel(Context& ctx);
        bool Inst_IfCopiedCompSeq(Context& ctx);
        bool Inst_IfCopiedDirectSeq(Context& ctx);
        bool Inst_HeadDivide(Context& ctx);
        bool Inst_HeadDivideSex(Context& ctx);
        bool Inst_HeadRead(Context& ctx);
        bool Inst_HeadWrite(Context& ctx);
        bool Inst_HeadCopy(Context& ctx);
        bool Inst_Search_Label_Comp_S(Context& ctx);
        bool Inst_Search_Label_Comp_F(Context& ctx);
        bool Inst_Search_Label_Comp_B(Context& ctx);
        bool Inst_Search_Label_Direct_S(Context& ctx);
        bool Inst_Search_Label_Direct_F(Context& ctx);
        bool Inst_Search_Label_Direct_B(Context& ctx);
        bool Inst_Search_Seq_Comp_S(Context& ctx);
        bool Inst_Search_Seq_Comp_F(Context& ctx);
        bool Inst_Search_Seq_Comp_B(Context& ctx);
        bool Inst_Search_Seq_Direct_S(Context& ctx);
        bool Inst_Search_Seq_Direct_F(Context& ctx);
        bool Inst_Search_Seq_Direct_B(Context& ctx);
        bool Inst_SetFlow(Context& ctx);
        
        // Thread Execution Control
        bool Inst_WaitCondition_Equal(Context& ctx);
        bool Inst_WaitCondition_Less(Context& ctx);
        bool Inst_WaitCondition_Greater(Context& ctx);
        
        
        // Bit Consensus
        bool Inst_BitConsensus(Context& ctx);
        bool Inst_BitConsensus24(Context& ctx);
        
        // Replication
        bool Inst_Repro(Context& ctx);
        bool Inst_Die(Context& ctx);
        
        // Movement and Navigation
        bool Inst_Move(Context& ctx);
        bool Inst_JuvMove(Context& ctx);
        bool Inst_GetCellPosition(Context& ctx);
        bool Inst_GetCellPositionX(Context& ctx);
        bool Inst_GetCellPositionY(Context& ctx);
        bool Inst_GetNorthOffset(Context& ctx);
        bool Inst_GetPositionOffset(Context& ctx);
        bool Inst_GetNortherly(Context& ctx);
        bool Inst_GetEasterly(Context& ctx);
        bool Inst_ZeroEasterly(Context& ctx);
        bool Inst_ZeroNortherly(Context& ctx);
        bool Inst_ZeroPosOffset(Context& ctx);
        
        // Rotation
        bool Inst_RotateLeftOne(Context& ctx);
        bool Inst_RotateRightOne(Context& ctx);
        bool Inst_RotateUphill(Context& ctx);
        bool Inst_RotateUpFtHill(Context& ctx);
        bool Inst_RotateHome(Context& ctx);
        bool Inst_RotateUnoccupiedCell(Context& ctx);
        bool Inst_RotateX(Context& ctx);
        bool Inst_RotateDir(Context& ctx);
        bool Inst_RotateOrgID(Context& ctx);
        bool Inst_RotateAwayOrgID(Context& ctx);
        
        
        // Resource and Topography Sensing
        bool Inst_SenseResourceID(Context& ctx);
        bool Inst_SenseResQuant(Context& ctx);
        bool Inst_SenseNest(Context& ctx);
        bool Inst_SenseResDiff(Context& ctx);
        bool Inst_SenseFacedHabitat(Context& ctx);
        bool Inst_LookAhead(Context& ctx);
        bool Inst_LookAheadIntercept(Context& ctx);
        bool Inst_LookAround(Context& ctx);
        bool Inst_LookAroundIntercept(Context& ctx);
        bool Inst_LookFT(Context& ctx);
        bool Inst_LookAroundFT(Context& ctx);
        bool Inst_SetForageTarget(Context& ctx);
        bool Inst_SetForageTargetOnce(Context& ctx);
        bool Inst_SetRandForageTargetOnce(Context& ctx);
        bool Inst_SetRandPFTOnce(Context& ctx);
        bool Inst_GetForageTarget(Context& ctx);
        bool Inst_ShowForageTarget(Context& ctx);
        bool Inst_GetLocOrgDensity(Context& ctx);
        bool Inst_GetFacedOrgDensity(Context& ctx);
        
        bool DoActualCollect(Context& ctx, int bin_used, bool unit);
        bool FakeActualCollect(Context& ctx, int bin_used, bool unit);
        bool Inst_CollectEdible(Context& ctx);
        bool Inst_CollectSpecific(Context& ctx);
        bool Inst_DepositResource(Context& ctx);
        bool Inst_DepositSpecific(Context& ctx);
        bool Inst_DepositAllAsSpecific(Context& ctx);
        bool Inst_NopDepositResource(Context& ctx);
        bool Inst_NopDepositSpecific(Context& ctx);
        bool Inst_NopDepositAllAsSpecific(Context& ctx);
        bool Inst_Nop2DepositAllAsSpecific(Context& ctx);
        bool Inst_NopCollectEdible(Context& ctx);
        bool Inst_Nop2CollectEdible(Context& ctx);
        bool Inst_GetResStored(Context& ctx);
        bool Inst_GetSpecificStored(Context& ctx);
        
        // Org Interactions
        bool Inst_GetFacedOrgID(Context& ctx);
        bool Inst_AttackPrey(Context& ctx);
        bool Inst_AttackPreyGroup(Context& ctx);
        bool Inst_AttackPreyShare(Context& ctx);
        bool Inst_AttackPreyNoShare(Context& ctx);
        bool Inst_AttackPreyFakeShare(Context& ctx);
        bool Inst_AttackPreyFakeGroupShare(Context& ctx);
        bool Inst_AttackPreyGroupShare(Context& ctx);
        bool Inst_AttackSpecPrey(Context& ctx);
        bool Inst_AttackPreyArea(Context& ctx);
        bool Inst_AttackFTPrey(Context& ctx);
        bool Inst_AttackPoisonPrey(Context& ctx);
        bool Inst_AttackPoisonFTPrey(Context& ctx);
        bool Inst_AttackPoisonFTPreyGenetic(Context& ctx);
        bool Inst_AttackPoisonFTMixedPrey(Context& ctx);
        bool Inst_FightMeritOrg(Context& ctx);
        bool Inst_FightBonusOrg(Context& ctx);
        bool Inst_GetMeritFightOdds(Context& ctx);
        bool Inst_FightOrg(Context& ctx); 
        bool Inst_AttackPred(Context& ctx); 
        bool Inst_KillPred(Context& ctx); 
        bool Inst_FightPred(Context& ctx); 
        bool Inst_MarkCell(Context& ctx); 
        bool Inst_MarkGroupCell(Context& ctx); 
        bool Inst_MarkPredCell(Context& ctx); 
        bool Inst_ReadFacedCell(Context& ctx); 
        bool Inst_ReadFacedPredCell(Context& ctx); 
        bool Inst_TeachOffspring(Context& ctx);
        bool Inst_LearnParent(Context& ctx);
        
        bool Inst_SetGuard(Context& ctx);
        bool Inst_SetGuardOnce(Context& ctx);
        bool Inst_GetNumGuards(Context& ctx);
        bool Inst_GetNumJuvs(Context& ctx);
        
        bool Inst_ActivateDisplay(Context& ctx);
        bool Inst_UpdateDisplay(Context& ctx);
        bool Inst_ModifyDisplay(Context& ctx);
        bool Inst_ReadLastSeenDisplay(Context& ctx);
        bool Inst_KillDisplay(Context& ctx);
        
        bool Inst_ModifySimpDisplay(Context& ctx);
        bool Inst_ReadLastSimpDisplay(Context& ctx);
        
        // Control-type Instructions
        bool Inst_ScrambleReg(Context& ctx);

        bool Inst_GetFacedEditDistance(Context& ctx);
        
        
      private:
        static HeadsEXInstLib* initInstLib();
        
        // ---------- Some Instruction Helpers -----------
        bool DoLookAheadEX(Context& ctx, bool use_ft = false);
        bool DoLookAgainEX(Context& ctx, bool use_ft = false);
        
      private:
        class HeadsEXInst : public InstLib::Entry
        {
        private:
          const InstMethod m_function;
          
        public:
          HeadsEXInst(const Apto::String& name, InstMethod function, InstructionClass _class, unsigned int flags,
                    const Apto::String& desc)
          : Entry(name, _class, flags, desc), m_function(function)
          {
          }
          
          InstMethod Function() const { return m_function; }
        };
        
        
        class HeadsEXInstLib : public InstLib
        {
        private:
          const HeadsEXInst* m_entries;
          Apto::String* m_nopmod_names;
          const int* m_nopmods;
          const InstMethod* m_functions;
          
        public:
          HeadsEXInstLib(int size, const HeadsEXInst* entries, Apto::String* nopmod_names, const int* nopmods,
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
        
        
      inline HeadsEX::DataValue& HeadsEX::DataValue::operator=(const DataValue& i)
      {
        value = i.value;
        originated = i.originated;
        from_env = i.from_env;
        oldest_component = i.oldest_component;
        env_component = i.env_component;
        return *this;
      }
      
      
      
      
      inline void HeadsEX::Head::Adjust()
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
      
      
      inline bool HeadsEX::Head::operator==(const Head& rhs) const
      {
        return m_hw == rhs.m_hw && m_pos == rhs.m_pos && m_ms == rhs.m_ms && m_is_gene == rhs.m_is_gene;
      }
      
      inline Instruction HeadsEX::Head::PrevInst()
      {
        return (AtFront()) ? GetMemory()[GetMemory().GetSize() - 1] : GetMemory()[m_pos - 1];
      }
      
      inline Instruction HeadsEX::Head::NextInst()
      {
        return (AtEnd()) ? m_hw->m_instset->InstError() : GetMemory()[m_pos + 1];
      }
      
      
      
      
      inline int HeadsEX::getRegister(Context& ctx, int reg_id)
      {
        if (reg_id >= NUM_REGISTERS) {
          return (this->*(m_imm_methods[reg_id]))(ctx);
        }
        return m_threads[m_cur_thread].reg[reg_id].value;
      }
      
      inline HeadsEX::DataValue HeadsEX::getRegisterData(Context& ctx, int reg_id)
      {
        if (reg_id >= NUM_REGISTERS) {
          DataValue dv;
          dv.originated = m_cycle_count;
          dv.value = (this->*(m_imm_methods[reg_id]))(ctx);
          return dv;
        }
        return m_threads[m_cur_thread].reg[reg_id];
      }
      
      
      inline HeadsEX::DataValue HeadsEX::stackPop()
      {
        if (m_threads[m_cur_thread].cur_stack == 0) {
          return m_threads[m_cur_thread].stack.Pop();
        } else {
          return m_global_stack.Pop();
        }
      }
      
      inline HeadsEX::Stack& HeadsEX::getStack(int stack_id)
      {
        if (stack_id == 0) {
          return m_threads[m_cur_thread].stack;
        } else {
          return m_global_stack;
        }
      }
      
      inline void HeadsEX::switchStack()
      {
        m_threads[m_cur_thread].cur_stack++;
        if (m_threads[m_cur_thread].cur_stack > 1) m_threads[m_cur_thread].cur_stack = 0;
      }
      
      
      inline void HeadsEX::setRegister(int reg_num, int value, bool from_env)
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
      
      inline void HeadsEX::setRegister(int reg_num, int value, const DataValue& src)
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
      
      inline void HeadsEX::setRegister(int reg_num, int value, const DataValue& op1, const DataValue& op2)
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

