/*
 *  cPopulationCell.h
 *  Avida
 *
 *  Called "pop_cell.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cPopulationCell_h
#define cPopulationCell_h

#include <fstream>

#ifndef cMutationRates_h
#include "cMutationRates.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif

class cPopulation;
class cOrganism;
class cPopulationCell;
class cWorld;

class cPopulationCell
{
  friend class cPopulation;
private:
  cWorld* m_world;
  cOrganism* organism;                    // The occupent of this cell.
  tList<cPopulationCell> connection_list;  // A list of neighboring cells.
  cMutationRates* mutation_rates;           // Mutation rates at this cell.
  tArray<int> input_array;                 // Environmental Inputs...

  int cell_id;           // Unique id for position of cell in population.
  int deme_id;           // ID of the deme that this cell is part of.
  int organism_count;    // Total number of orgs to ever inhabit this cell.


  void InsertOrganism(cOrganism & new_org);
  cOrganism* RemoveOrganism();

public:
  cPopulationCell();
  cPopulationCell(const cPopulationCell& in_cell);
  ~cPopulationCell() { ; }

  void operator=(const cPopulationCell& in_cell);

  void Setup(cWorld* world, int in_id, const cMutationRates & in_rates);
  void SetDemeID(int in_id) { deme_id = in_id; }
  void Rotate(cPopulationCell & new_facing);

  cOrganism* GetOrganism() const { return organism; }
  tList<cPopulationCell> & ConnectionList() { return connection_list; }
  cPopulationCell & GetCellFaced() { return *(connection_list.GetFirst()); }
  const cMutationRates & MutationRates() const { return *mutation_rates; }
  cMutationRates & MutationRates() { return *mutation_rates; }
  int GetInput(int);
  int GetInputAt(int & input_pointer);
  int GetInputSize() { return input_array.GetSize(); }

  int GetID() const { return cell_id; }
  int GetDemeID() const { return deme_id; }
  int GetOrganismCount() const { return organism_count; }

  bool IsOccupied() const { return organism != NULL; }

  bool OK();
};


#ifdef ENABLE_UNIT_TESTS
namespace nPopulationCell {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
