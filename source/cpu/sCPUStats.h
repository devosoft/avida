//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef CPU_STATS_HH
#define CPU_STATS_HH

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


  void Setup(int num_instructions) {
    (void) num_instructions;
    mut_stats.Clear();
  }

  void Clear() {  // Called on any New Creature
    mut_stats.Clear();
  }
};

#endif
