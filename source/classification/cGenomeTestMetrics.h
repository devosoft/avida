/*
 *  cGenomeTestMetrics.h
 *  Avida
 *
 *  Created by David Bryson on 8/13/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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
