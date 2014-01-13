/*
 *  util/NopSequence.h
 *  avida-core
 *
 *  Copyright 1999-2013 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef AvidaUtilNopSequence_h
#define AvidaUtilNopSequence_h

#include "avida/core/Types.h"


namespace Avida {
  namespace Util {
    
    class NopSequence
    {
    private:
      const int m_max_size;
      int m_cur_size;
      char* m_nops;
      
    public:
      LIB_EXPORT inline NopSequence(int max_size = 10) : m_max_size(max_size), m_cur_size(0), m_nops(new char[max_size]) { ; }
      LIB_EXPORT inline NopSequence(const NopSequence& seq);
      LIB_EXPORT inline ~NopSequence() { delete m_nops; }

      LIB_EXPORT inline int MaxSize() const { return m_max_size; }
      LIB_EXPORT inline int Size() const { return m_cur_size; }

      LIB_EXPORT inline bool operator==(const NopSequence& other_seq) const;
      LIB_EXPORT inline bool operator!=(const NopSequence& other_seq) const { return !(operator==(other_seq)); }
      LIB_EXPORT inline char operator[](int position) const { return m_nops[position]; }
      LIB_EXPORT inline NopSequence& operator=(const NopSequence& seq);
      
      LIB_EXPORT void ReadString(const Apto::String& str);
      
      LIB_EXPORT int FindSubsequence(const NopSequence& sub_seq) const;
      LIB_EXPORT inline bool Contains(const NopSequence& sub_seq) const { return (FindSubsequence(sub_seq) >= 0); }
      
      LIB_EXPORT inline void Clear() { m_cur_size = 0; }
      LIB_EXPORT inline void AddNop(int nop_num);
      LIB_EXPORT inline void Rotate(const int rot, const int base);
      
      LIB_EXPORT inline Apto::String AsString() const;
      
      LIB_EXPORT int AsInt(const int base) const;
      LIB_EXPORT int AsIntGreyCode(const int base) const;
      LIB_EXPORT int AsIntDirect(const int base) const;
      LIB_EXPORT int AsIntUnique(const int base) const;
      LIB_EXPORT int AsIntAdditivePolynomial(const int base) const;
      LIB_EXPORT int AsIntFib(const int base) const;
      LIB_EXPORT int AsIntPolynomialCoefficent(const int base) const;
    };
    
    
    inline NopSequence::NopSequence(const NopSequence& other_seq)
    : m_max_size(other_seq.m_max_size), m_cur_size(other_seq.m_cur_size), m_nops(new char[m_max_size])
    {
      for (int i = 0; i < m_cur_size; i++) m_nops[i] = other_seq.m_nops[i];
    }
    
    
    inline void NopSequence::AddNop(int nop_num)
    {
      if (m_cur_size < m_max_size) {
        m_nops[m_cur_size++] = static_cast<char>(nop_num);
      }
    }
    
    inline void NopSequence::Rotate(const int rot, const int base)
    {
      for (int i = 0; i < m_cur_size; i++) {
        m_nops[i] += rot;
        if (m_nops[i] >= base) m_nops[i] -= base;
      }
    }
    
    
    inline Apto::String NopSequence::AsString() const
    {
      Apto::String out_string;
      for (int i = 0; i < m_cur_size; i++) out_string += m_nops[i] + 'A';
      return out_string;
    }
    
    
    inline bool NopSequence::operator==(const NopSequence& other_seq) const
    {
      if (m_cur_size != other_seq.m_cur_size) return false;
      
      for (int i = 0; i < m_cur_size; i++) if (m_nops[i] != other_seq.m_nops[i]) return false;
      
      return true;
    }

  };
};

#endif
