/*
 *  cCPUMemory.h
 *  Avida
 *
 *  Called "cpu_memory.hh" prior to 11/22/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cCPUMemory_h
#define cCPUMemory_h

#ifndef cGenome_h
#include "cGenome.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif


class cCPUMemory : public cGenome
{
private:
	static const unsigned char MASK_COPIED   = 0x01;
	static const unsigned char MASK_MUTATED  = 0x02;
	static const unsigned char MASK_EXECUTED = 0x04;
	static const unsigned char MASK_BREAK    = 0x08;
	static const unsigned char MASK_POINTMUT = 0x10;
	static const unsigned char MASK_COPYMUT  = 0x20;
	static const unsigned char MASK_INJECTED = 0x40;
	static const unsigned char MASK_UNUSED   = 0x80; // unused bit
  
  tArray<unsigned char> flag_array;
  

  // A collection of sloppy instructions to perform oft-used functions that
  // will need to be cleaned up after this is run.
  void SloppyResize(int new_size);           // Set size, ignore new contents.
  void SloppyInsert(int pos, int num_lines); // Add lines, ignore new contents.

public:
  explicit cCPUMemory(int _size=1)  : cGenome(_size), flag_array(_size) { ClearFlags(); }
  cCPUMemory(const cCPUMemory& in_memory);
  cCPUMemory(const cGenome& in_genome) : cGenome(in_genome), flag_array(in_genome.GetSize()) { ; }
  cCPUMemory(const cString& in_string) : cGenome(in_string), flag_array(in_string.GetSize()) { ; }
  //! Construct a cCPUMemory object from a cInstruction range.
  cCPUMemory(cInstruction* begin, cInstruction* end) : cGenome(begin, end), flag_array(GetSize()) { ClearFlags(); }
  ~cCPUMemory() { ; }

  void operator=(const cCPUMemory& other_memory);
  void operator=(const cGenome& other_genome);
  void Copy(int to, int from);

  void Clear()
	{
		for (int i = 0; i < active_size; i++) {
			genome[i].SetOp(0);
			flag_array[i] = 0;
		}
	}
  void ClearFlags() { flag_array.SetAll(0); }
  void Reset(int new_size);     // Reset size, clearing contents...
  void Resize(int new_size);    // Reset size, save contents, init to default
  void ResizeOld(int new_size); // Reset size, save contents, init to previous

  bool FlagCopied(int pos) const     { return MASK_COPIED   & flag_array[pos]; }
  bool FlagMutated(int pos) const    { return MASK_MUTATED  & flag_array[pos]; }
  bool FlagExecuted(int pos) const   { return MASK_EXECUTED & flag_array[pos]; }
  bool FlagBreakpoint(int pos) const { return MASK_BREAK    & flag_array[pos]; }
  bool FlagPointMut(int pos) const   { return MASK_POINTMUT & flag_array[pos]; }
  bool FlagCopyMut(int pos) const    { return MASK_COPYMUT  & flag_array[pos]; }
  bool FlagInjected(int pos) const   { return MASK_INJECTED & flag_array[pos]; }
  
  void SetFlagCopied(int pos)     { flag_array[pos] |= MASK_COPIED;   }
  void SetFlagMutated(int pos)    { flag_array[pos] |= MASK_MUTATED;  }
  void SetFlagExecuted(int pos)   { flag_array[pos] |= MASK_EXECUTED; }
  void SetFlagBreakpoint(int pos) { flag_array[pos] |= MASK_BREAK;    }
  void SetFlagPointMut(int pos)   { flag_array[pos] |= MASK_POINTMUT; }
  void SetFlagCopyMut(int pos)    { flag_array[pos] |= MASK_COPYMUT;  }
  void SetFlagInjected(int pos)   { flag_array[pos] |= MASK_INJECTED; }
	
	void ClearFlagCopied(int pos)     { flag_array[pos] &= ~MASK_COPIED;   }
	void ClearFlagMutated(int pos)    { flag_array[pos] &= ~MASK_MUTATED;  }
	void ClearFlagExecuted(int pos)   { flag_array[pos] &= ~MASK_EXECUTED; }
	void ClearFlagBreakpoint(int pos) { flag_array[pos] &= ~MASK_BREAK;    }
	void ClearFlagPointMut(int pos)   { flag_array[pos] &= ~MASK_POINTMUT; }
	void ClearFlagCopyMut(int pos)    { flag_array[pos] &= ~MASK_COPYMUT;  }
  void ClearFlagInjected(int pos)   { flag_array[pos] &= ~MASK_INJECTED; }
    
  void Append(const cInstruction& in_inst) { Insert(GetSize(), in_inst); }
  void Append(const cGenome& in_genome) { Insert(GetSize(), in_genome); }
  void Insert(int pos, const cInstruction& in_inst);
  void Insert(int pos, const cGenome& in_genome);
  void Remove(int pos, int num_insts=1);
  void Replace(int pos, int num_insts, const cGenome& in_genome);
};


#ifdef ENABLE_UNIT_TESTS
namespace nCPUMemory {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
