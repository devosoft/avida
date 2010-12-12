//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cViewInfo.h"

#include <fstream>

#include "cBioGroupManager.h"
#include "cClassificationManager.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cOrganism.h"

#include "cSymbolUtil.h"
#include "cScreen.h"

#include "tAutoRelease.h"
#include "tIterator.h"


using namespace std;


cViewInfo::cViewInfo(cWorld* world, cView_Base* view)
: m_world(world)
, m_view(view)
{
  active_cell = NULL;
  pause_level = PAUSE_OFF;
  saved_inst_set = NULL;
  thread_lock = -1;
  step_organism_id = -1;
  map_mode=0;
  
  // Handle genotype managing...
  
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    genotype_chart[i] = NULL;
    symbol_chart[i] = (char) (i + 'A');
  }
}

void cViewInfo::AddGenChart(cBioGroup* in_gen)
{
  for (int i = 0; i < NUM_SYMBOLS; i++) {
    if (genotype_chart[i] == NULL) {
      genotype_chart[i] = in_gen;
      getViewInfo(in_gen)->symbol = symbol_chart[i];
      break;
    }
  }
}


void cViewInfo::SetupSymbolMaps(int map_mode, bool use_color)
{
  typedef char (*SymbolMethod)(const cPopulationCell & cell);
  SymbolMethod map_method = NULL;
  SymbolMethod color_method = NULL;
  
  switch (map_mode) {
    case MAP_BASIC:
      if (use_color) color_method = &cSymbolUtil::GetBasicSymbol;
      else map_method = &cSymbolUtil::GetBasicSymbol;
      break;
    case MAP_INJECT:
      if (use_color) color_method = &cSymbolUtil::GetModifiedSymbol;
      else map_method = &cSymbolUtil::GetModifiedSymbol;
      break;
    case MAP_RESOURCE:
      map_method = &cSymbolUtil::GetResourceSymbol;
      break;
    case MAP_AGE:
      map_method = &cSymbolUtil::GetAgeSymbol;
      break;
    case MAP_BREED_TRUE:
      if (use_color) color_method = &cSymbolUtil::GetBreedSymbol;
      else map_method = &cSymbolUtil::GetBreedSymbol;
      break;
    case MAP_PARASITE:
      if (use_color) color_method = &cSymbolUtil::GetParasiteSymbol;
      else map_method = &cSymbolUtil::GetParasiteSymbol;
      break;
    case MAP_MUTATIONS:
      if (use_color) color_method = &cSymbolUtil::GetMutSymbol;
      else map_method = &cSymbolUtil::GetMutSymbol;
      break;
    case MAP_THREAD:
      //if (use_color) color_method = &cSymbolUtil::GetThreadSymbol;
      map_method = &cSymbolUtil::GetThreadSymbol;
      break;
    case MAP_LINEAGE:
      if (use_color) color_method = &cSymbolUtil::GetLineageSymbol;
      else map_method = &cSymbolUtil::GetLineageSymbol;
      break;
  }
  
  const int num_cells = m_world->GetPopulation().GetSize();
  map.Resize(num_cells);
  color_map.Resize(num_cells);
  
  for (int i = 0; i < num_cells; i++) {
    if (map_method == 0) map[i] = 0;
    else map[i] = (*map_method)(m_world->GetPopulation().GetCell(i));
    
    if (color_method == 0) color_map[i] = 0;
    else color_map[i] = (*color_method)(m_world->GetPopulation().GetCell(i));
  }
  
}


void cViewInfo::UpdateSymbols()
{
  // First, clean up the genotype_chart.
  
  int i, pos;
  for (i = 0; i < NUM_SYMBOLS; i++) {
    if (genotype_chart[i]) {
      tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
      pos = -1;
      int rank = 0;
      while ((it->Next()) && i < NUM_SYMBOLS) {
        if (genotype_chart[i] == it->Get()) {
          pos = rank;
          break;
        }
        rank++;
      }
      
      if (pos < 0) genotype_chart[i] = NULL;
      if (pos >= NUM_SYMBOLS) {
        if (genotype_chart[i]->GetProperty("threshold").AsBool())
          getViewInfo(genotype_chart[i])->symbol = '+';
        else getViewInfo(genotype_chart[i])->symbol = '.';
        genotype_chart[i] = NULL;
      }
    }
  }
  
  // Now, fill in any missing spaces...
  
  tAutoRelease<tIterator<cBioGroup> > it(m_world->GetClassificationManager().GetBioGroupManager("genotype")->Iterator());
  cBioGroup* bg = it->Next();
  for (int i = 0; bg && i < SYMBOL_THRESHOLD; bg = it->Next(), i++) {
    if (!InGenChart(bg)) AddGenChart(bg);
  }
}


void cViewInfo::EngageStepMode()
{
  // Steps can only be taken through the execution of a cpu when avida is
  // paused, and focued on an active cpu.
  if ( pause_level == PAUSE_ON  &&  active_cell != NULL ) {
    pause_level = PAUSE_ADVANCE_STEP;
    SetStepOrganism( active_cell->GetID() );
  }
}

void cViewInfo::DisEngageStepMode()
{
  SetStepOrganism(-1);
}

cBioGroup* cViewInfo::GetActiveGenotype()
{
  if (active_cell != NULL && active_cell->IsOccupied()) {
    return active_cell->GetOrganism()->GetBioGroup("genotype");
  }
  
  return NULL;
}


cString cViewInfo::GetActiveName()
{
  if (GetActiveGenotype() == NULL) return cString("");
  return GetActiveGenotype()->GetProperty("name").AsString();
}

int cViewInfo::GetActiveID()
{
  if (active_cell) return active_cell->GetID();
  return -1;
}

int cViewInfo::GetActiveGenotypeID()
{
  return GetActiveGenotype() ? GetActiveGenotype()->GetID() : -1;
}

sGenotypeViewInfo* cViewInfo::getViewInfo(cBioGroup* bg)
{
  sGenotypeViewInfo* view_info = bg->GetData<sGenotypeViewInfo>();
  if (!view_info) {
    view_info = new sGenotypeViewInfo;
    bg->AttachData(view_info);
  }
  return view_info;
}
