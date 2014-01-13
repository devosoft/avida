//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cHistScreen.h"

#include <fstream>

#include "avida/core/Properties.h"
#include "avida/systematics/Arbiter.h"
#include "avida/systematics/Manager.h"


#include "cViewInfo.h"
#include "cWorld.h"

using namespace std;


void cHistScreen::PrintGenotype(Systematics::GroupPtr in_gen, int in_pos, int max_stars, int star_size)
{
  SetBoldColor(COLOR_CYAN);
  PrintDouble(in_pos, 0, Apto::StrAs(in_gen->Properties().Get("fitness").StringValue()));
  
  SetBoldColor(COLOR_WHITE);
  Print(in_pos, 8, "%s: ", static_cast<const char*>(in_gen->Properties().Get("name").StringValue()));
  
  int cur_num = in_gen->NumUnits();
  int cur_stars = cur_num / star_size;
  if (cur_num % star_size) cur_stars++;
  
  // Set the color for this bar.
  Apto::SmartPtr<sGenotypeViewInfo> view_info = in_gen->GetData<sGenotypeViewInfo>();
  if (!view_info) {
    view_info = Apto::SmartPtr<sGenotypeViewInfo>(new sGenotypeViewInfo);
    in_gen->AttachData(view_info);
  }
  SetSymbolColor(view_info->symbol);
  
  // Draw the bar.
  int i;
  for (i = 0; i < cur_stars; i++) Print(CHAR_BULLET);
  
  // Draw the spaces following the bar.
  while (i++ < max_stars) Print(' ');
  
  // Display the true length of the bar (highlighted)
  SetBoldColor(COLOR_WHITE);
  Print(in_pos, Width() - 8, " %5d", cur_num);
  
  // Reset the color to normal
  SetColor(COLOR_WHITE);
}

//void cHistScreen::PrintSpecies(cSpecies * in_species, int in_pos, int max_num)
//{
//  SetBoldColor(COLOR_WHITE);
//  Print(in_pos, 0, "        sp-%06d: ", in_species->GetID());
//
//  int max_stars = Width() - 28;
//  int star_size = (max_num / max_stars);
//  if (max_num % max_stars) star_size++;
//
//  int cur_num = in_species->GetNumOrganisms();
//  int cur_stars = cur_num / star_size;
//  if (cur_num % star_size) cur_stars++;
//
//  // Set the color for this bar.
//  SetSymbolColor(in_species->GetSymbol());
//
//  // Draw the bar.
//  int i;
//  for (i = 0; i < cur_stars; i++) {
//    Print(in_species->GetSymbol());
//  }
//
//  // Draw the spaces following the bar.
//  while (i++ < max_stars) Print(' ');
//
//  // Display the true length of the bar (highlighted)
//  SetBoldColor(COLOR_WHITE);
//  Print(in_pos, Width() - 8, " %5d", cur_num);
//
//  // Reset the color to normal
//  SetColor(COLOR_WHITE);
//}


void cHistScreen::Draw(cAvidaContext& ctx)
{
  SetBoldColor(COLOR_WHITE);
  Print(1,  0, "Fitness Name");
  Print(1, 19, "Histogram: [ ]");
  Print(1, 53, "[ ]");
  SetBoldColor(COLOR_CYAN);
  Print(1, 31, '<');
  Print(1, 54, '>');
  
  Update(ctx);
}

void cHistScreen::Update(cAvidaContext& ctx)
{
  const int max_stars = Width() - 28;
  int max_num = 0, star_size = 0;
  
  switch(mode) {
    case HIST_GENOTYPE:
      Systematics::ManagerPtr classmgr = Systematics::Manager::Of(m_world->GetNewWorld());
      Systematics::Arbiter::IteratorPtr it = classmgr->ArbiterForRole("genotype")->Begin();
      Systematics::GroupPtr bg = it->Next();
      if (bg) max_num = bg->NumUnits();
      star_size = (max_num / max_stars);
      if (max_num % max_stars) star_size++;
      
      SetBoldColor(COLOR_WHITE);
      Print(1,  34, "Genotype Abundance");
      
      // Print out top NUM_SYMBOL genotypes in fixed order.
      for (int i = 0; i < info.GetNumSymbols(); i++) {
        if (info.GetGenotype(i)) {
          PrintGenotype(info.GetGenotype(i), i + 2, max_stars, star_size);
        }
        else {
          Move(i + 2, 0);
          ClearToEOL();
        }
      }
      
      SetBoldColor(COLOR_WHITE);
      if (star_size == 1) {
        Print(info.GetNumSymbols() + 3, 0, "Each '#' = %d Organism   ", star_size);
      } else {
        Print(info.GetNumSymbols() + 3, 0, "Each '#' = %d Organisms  ", star_size);
      }
      ClearToEOL();
      
      break;
  }
  
  ClearToBot();
  Refresh();
}

void cHistScreen::DoInput(cAvidaContext& ctx, int in_char)
{
  switch(in_char) {
    case '<':
    case ',':
      ++mode %= NUM_HIST;
      Update(ctx);
      break;
    case '>':
    case '.':
      mode += NUM_HIST;
      --mode %= NUM_HIST;
      Update(ctx);
      break;
  }
}


