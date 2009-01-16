/*
 *  cMutationSteps.h
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef cMutationSteps_h
#define cMutationSteps_h

#ifndef cString_h
#include "cString.h"
#endif

#ifndef tList_h
#include "tList.h"
#endif


class cMutationStep {
public:
  virtual ~cMutationStep() {};
  virtual cMutationStep* copy() const = 0;
  virtual cString AsString() = 0;  
};

class cSubstitutionMutationStep : public cMutationStep {
public:
  int m_pos;
  char m_from;
  char m_to;
  cSubstitutionMutationStep(const cSubstitutionMutationStep& _in) : cMutationStep(), m_pos(_in.m_pos), m_from(_in.m_from), m_to(_in.m_to) {}
  void operator=(const cSubstitutionMutationStep& _in) { m_pos=_in.m_pos; m_from=_in.m_from; m_to=_in.m_to; };  
  cSubstitutionMutationStep(int _pos, char _from, char _to) { m_pos = _pos; m_from = _from;  m_to = _to; }
  ~cSubstitutionMutationStep() {};
  cMutationStep* copy() const { return new cSubstitutionMutationStep(*this); }
  cString AsString() { cString s; s.Set("M%c%i%c", m_from, m_pos, m_to); return s; }
};

class cDeletionMutationStep : public cMutationStep {
public:
  int m_pos;
  char m_from;
  cDeletionMutationStep(const cDeletionMutationStep& _in) : cMutationStep(), m_pos(_in.m_pos), m_from(_in.m_from) {}
  void operator=(const cDeletionMutationStep& _in) { m_pos=_in.m_pos; m_from=_in.m_from; };  
  cDeletionMutationStep(int _pos, char _from) { m_pos = _pos; m_from = _from; }
  ~cDeletionMutationStep() {};
  cMutationStep* copy() const { return new cDeletionMutationStep(*this); }
  cString AsString() { cString s; s.Set("D%i%c", m_pos, m_from); return s; }
};

class cInsertionMutationStep : public cMutationStep{
public:
  int m_pos;
  char m_to;
  cInsertionMutationStep(const cInsertionMutationStep& _in) : cMutationStep(), m_pos(_in.m_pos), m_to(_in.m_to) {}
  void operator=(const cInsertionMutationStep& _in) { m_pos=_in.m_pos; m_to=_in.m_to; };  
  cInsertionMutationStep(int _pos, int _to) { m_pos = _pos; m_to = _to; };
  ~cInsertionMutationStep() {};
  cMutationStep* copy() const { return new cInsertionMutationStep(*this); }
  cString AsString() { cString s; s.Set("I%i%c", m_pos, m_to); return s; }
};

class cSlipMutationStep : public cMutationStep {
public:
  int m_start;
  int m_end;
  cSlipMutationStep(const cSlipMutationStep& _in) : cMutationStep(), m_start(_in.m_start), m_end(_in.m_end) {}
  void operator=(const cSlipMutationStep& _in) { m_start=_in.m_start; m_end=_in.m_end; };
  cSlipMutationStep(int _start, int _end) { m_start = _start; m_end = _end; };
  ~cSlipMutationStep() {};
  cMutationStep* copy() const { return new cSlipMutationStep(*this); }
  cString AsString() { cString s; s.Set("S%i-%i", m_start, m_end); return s; }
};


class cMutationSteps : public tList<cMutationStep> {
private:
  cString m_loaded_string;
public:
  cMutationSteps() {};
  cMutationSteps(const cMutationSteps& in_ms);
  void operator=(const cMutationSteps& in_ms);
  ~cMutationSteps();
  void Clear();  

  void AddSubstitutionMutation(int _pos, char _from, char _to);
  void AddDeletionMutation(int _pos, char _from);
  void AddInsertionMutation(int _pos, int _to);
  void AddSlipMutation(int _start, int _end);

  void Set(const cString& in_mut_steps) { m_loaded_string = in_mut_steps; }  
    // need to implement for reading, right now, we fake it.
  cString AsString() const;
};


#endif
