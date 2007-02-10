/*
 *  cGenotype_TestData.h
 *  Avida
 *
 *  Called "genotype_test_data.hh" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#ifndef cGenotype_TestData_h
#define cGenotype_TestData_h

class cGenotype_TestData {
public:
  cGenotype_TestData() : fitness(-1) { ; }
  ~cGenotype_TestData() { ; }

  bool is_viable;

  double fitness;
  double merit;
  int gestation_time;
  int executed_size;
  int copied_size;
  double colony_fitness;
  int generations;
};


#ifdef ENABLE_UNIT_TESTS
namespace nGenotype_TestData {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
