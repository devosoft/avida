//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          // 
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cSymbolUtil.h"

#include <fstream>

#include "cGenotype.h"
#include "cOrganism.h"
#include "cPopulationCell.h"
#include "cSpecies.h"
#include "cHardwareBase.h"

using namespace std;


char cSymbolUtil::GetBasicSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  return organism.GetGenotype()->GetSymbol();
}

char cSymbolUtil::GetSpeciesSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  cSpecies * cur_species = organism.GetGenotype()->GetSpecies();
  if (cur_species == NULL) return '.';    // no species
  return cur_species->GetSymbol();        // symbol!
}

char cSymbolUtil::GetModifiedSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  const bool modifier = organism.GetPhenotype().IsModifier();
  const bool modified = organism.GetPhenotype().IsModified();
  
  // 'I' = Injector     'H' = Host (Injected into)
  // 'B' = Both         '-' = Neither
  
  if (modifier == true && modified == true)  return 'B';
  if (modifier == true) return 'I'-6;
  if (modified == true) return 'H'-6;
  return '-';
}

char cSymbolUtil::GetResourceSymbol(const cPopulationCell & cell)
{
  // @CAO Not Implemented yet...
  if (cell.IsOccupied() == false) return ' ';
  return '.';
}

char cSymbolUtil::GetAgeSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  const int age = organism.GetPhenotype().GetAge();
  if (age < 0) return '-';
  if (age < 10) return (char) ('0' + age);
  if (age < 20) return 'X';
  if (age < 80) return 'L';
  if (age < 200) return 'C';
  
  return '+';
}

char cSymbolUtil::GetBreedSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  if (organism.GetPhenotype().ParentTrue() == true) return '*';
  return '-';
}

char cSymbolUtil::GetParasiteSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  if (organism.GetNumParasites()) return 'A';
  return '-';
}

char cSymbolUtil::GetMutSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  if (organism.GetPhenotype().IsMutated() == true) return '*';
  return '-';
}

char cSymbolUtil::GetThreadSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  return '0' + cell.GetOrganism()->GetHardware().GetNumThreads();
}

char cSymbolUtil::GetLineageSymbol(const cPopulationCell & cell)
{
  if (cell.IsOccupied() == false) return ' ';
  const cOrganism & organism = *(cell.GetOrganism());
  
  return 'A' + (organism.GetLineageLabel() % 12);
}

