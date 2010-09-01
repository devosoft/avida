/*
 *  cEnvReqs.h
 *  Avida
 *
 *  Created by David Bryson on 12/12/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*! Support for extensible input from the environment. */
#ifndef cEnvReqs_h
#define cEnvReqs_h

class cEnvReqs
{
private:
  int m_min_inputs;
  int m_min_outputs;
  bool m_true_rand_inputs;


public:
  cEnvReqs() : m_min_inputs(0), m_min_outputs(0), m_true_rand_inputs(false) { ; }
  
  int GetMinInputs() { return m_min_inputs; }
  void SetMinInputs(int v) { m_min_inputs = v; }
  int GetMinOutputs() { return m_min_outputs; }
  void SetMinOutputs(int v) { m_min_outputs = v; }
  void SetTrueRandInputs(bool v = true) { m_true_rand_inputs = v; }
  bool GetTrueRandInputs() { return m_true_rand_inputs; }
};

#endif
