//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef POPULATION_CELL_HH
#define POPULATION_CELL_HH

#include <fstream>

#ifndef MUTATION_RATES_HH
#include "cMutationRates.h"
#endif
#ifndef TARRAY_HH
#include "tArray.h"
#endif
#ifndef TLIST_HH
#include "tList.h"
#endif

class cPopulation;
class cOrganism;
template <class T> class tList; // aggregate
class cPopulationCell;
class cMutationRates; // aggregate
template <class T> class tArray; // aggregate

class cPopulationCell {
  friend class cPopulation;
private:
  cWorld* m_world;
  cOrganism* organism;                    // The occupent of this cell.
  tList<cPopulationCell> connection_list;  // A list of neighboring cells.
  cMutationRates* mutation_rates;           // Mutation rates at this cell.
  tArray<int> input_array;                 // Environmental Inputs...
  int cur_input;                           // Next input to give organism.

  int cell_id;           // Unique id for position of cell in population.
  int organism_count;    // Total number of orgs to ever inhabit this cell.

private:  // Organism changes should only occur through population...
  void InsertOrganism(cOrganism & new_org);
  cOrganism * RemoveOrganism();

public:
  cPopulationCell();
  cPopulationCell(const cPopulationCell & in_cell);
  ~cPopulationCell() { ; }

  void operator=(const cPopulationCell & in_cell);

  void Setup(cWorld* world, int in_id, const cMutationRates & in_rates);
  void Rotate(cPopulationCell & new_facing);

  cOrganism * GetOrganism() const { return organism; }
  tList<cPopulationCell> & ConnectionList() { return connection_list; }
  const cMutationRates & MutationRates() const { return *mutation_rates; }
  cMutationRates & MutationRates() { return *mutation_rates; }
  int GetInput();
  int GetInput(int);
  int GetInputAt(int & input_pointer);

  int GetID() const { return cell_id; }
  int GetOrganismCount() const { return organism_count; }

  bool IsOccupied() const { return organism != NULL; }

  bool OK();

  bool SaveState(std::ofstream & fp);
  bool LoadState(std::ifstream & fp);
};

#endif
