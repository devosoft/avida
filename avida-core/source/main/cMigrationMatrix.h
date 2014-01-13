/*
 *  cMigrationMatrix.h
 *  Avida
 *
 *  Copyright 2012 Michigan State University. All rights reserved.
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


#ifndef cMigrationMatrix_h
#define cMigrationMatrix_h

#include "cAvidaContext.h"
#include "cString.h"

namespace Avida {
  class Feedback;
};

using namespace Avida;

class cMigrationMatrix
{
public:
  cMigrationMatrix();
  ~cMigrationMatrix();
  
  int GetOffspringCountAt(int from_deme_id, int to_deme_id);
  int GetParasiteCountAt(int from_deme_id, int to_deme_id);
  
  bool AlterConnectionWeight(const int from_deme_id, const int to_deme_id, const double alter_amount);
  int GetProbabilisticDemeID(const int from_deme_id, Apto::Random& p_rng,bool p_is_parasite_migration);
  bool Load(const int num_demes, const cString& filename, const cString& working_dir,bool p_count_parasites, bool p_count_offspring, bool p_is_reload, Feedback& feedback);
  void Print();
  void ResetParasiteCounts();
  void ResetOffspringCounts();
  
private:
  Apto::Array< Apto::Array<double, Apto::Smart>, Apto::Smart > m_migration_matrix;
  Apto::Array<double> m_row_connectivity_sums;
  Apto::Array< Apto::Array<int, Apto::Smart>, Apto::Smart > m_parasite_migration_counts;
  Apto::Array< Apto::Array<int, Apto::Smart>, Apto::Smart >  m_offspring_migration_counts;
};

#endif
