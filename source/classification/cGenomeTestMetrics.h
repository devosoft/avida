/*
 *  cGenomeTestMetrics.h
 *  Avida
 *
 *  Created by David Bryson on 8/13/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cGenomeTestMetrics_h
#define cGenomeTestMetrics_h

#include <cassert>

class cAvidaContext;
class cBioGroup;


class cGenomeTestMetrics
{
private:
  bool m_is_viable;
  double m_fitness;
  double m_colony_fitness;
  double m_merit;
  int m_copied_size;
  int m_executed_size;
  int m_gestation_time;
  
  
  cGenomeTestMetrics(); // @not_implemented
  
  cGenomeTestMetrics(cAvidaContext& ctx, cBioGroup* bg);
  
public:
  bool IsViable() const { return m_is_viable; }
  double GetFitness() const { return m_fitness; }
  double GetColonyFitness() const { return m_colony_fitness; }
  double GetMerit() const { return m_merit; }
  int GetLinesCopied() const { return m_copied_size; }
  int GetLinesExecuted() const { return m_executed_size; }
  int GetGestationTime() const { return m_gestation_time; }
  
  
  static cGenomeTestMetrics* GetMetrics(cAvidaContext& ctx, cBioGroup* bg);
};

#endif
