/*
 *  sCPUStats.h
 *  Avida
 *
 *  Called "cpu_stats.hh" prior to 11/30/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
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

#ifndef sCPUStats_h
#define sCPUStats_h

/**
 * Class to facilitate passing information from CPU to Stats.
 **/

class sCPUStats {
public:
  class sMutationStats {
  public:
    // Counts of mutations INHERITED FROM PARENT
    int point_mut_count;
    int copy_mut_count;
    int insert_mut_count;
    int delete_mut_count;
    int div_mut_count;
    // Counts of Mutated _Lines_
    int point_mut_line_count;   // In PARENT
    int parent_mut_line_count;  // In PARENT
    int copy_mut_line_count;    // In CHILD
    // Theses happen by divide, so they should be 0 or 1
    int divide_mut_count;
    int divide_insert_mut_count;
    int divide_delete_mut_count;
    // Other info needed for mutation calculations
    int copies_exec;

    void Clear() {
      point_mut_count = 0;
      copy_mut_count = 0;
      insert_mut_count = 0;
      point_mut_line_count = 0;
      parent_mut_line_count = 0;
      copy_mut_line_count = 0;
      delete_mut_count = 0;
      div_mut_count=0;
      divide_mut_count = 0;
      divide_insert_mut_count = 0;
      divide_delete_mut_count = 0;
      copies_exec = 0;
    }
  };

  // Contiually Set
  sMutationStats mut_stats;

  void Setup() { mut_stats.Clear(); }

  void Clear() {  // Called on any New Creature
    mut_stats.Clear();
  }
};

#endif
