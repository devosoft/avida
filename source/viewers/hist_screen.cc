//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "genebank.hh"
#include "genotype.hh"
#include "species.hh"

#include "hist_screen.hh"


using namespace std;


void cHistScreen::PrintGenotype(cGenotype * in_gen, int in_pos,
				       int max_num)
{
  SetBoldColor(COLOR_CYAN);
  PrintFitness(in_pos, 0, in_gen->GetFitness());

  SetBoldColor(COLOR_WHITE);
  Print(in_pos, 8, "%s: ", in_gen->GetName()());

  int max_stars = Width() - 28;
  int star_size = (max_num / max_stars);
  if (max_num % max_stars) star_size++;

  int cur_num = in_gen->GetNumOrganisms();
  int cur_stars = cur_num / star_size;
  if (cur_num % star_size) cur_stars++;

  // Set the color for this bar.
  SetSymbolColor(in_gen->GetSymbol());

  // Draw the bar.
  int i;
  for (i = 0; i < cur_stars; i++) {
    cSpecies * cur_species = in_gen->GetSpecies();
    if (cur_species != NULL) {
      Print(in_gen->GetSpecies()->GetSymbol());
    } else {
      Print(CHAR_BULLET);
    }
  }

  // Draw the spaces following the bar.
  while (i++ < max_stars) Print(' ');

  // Display the true length of the bar (highlighted)
  SetBoldColor(COLOR_WHITE);
  Print(in_pos, Width() - 8, " %5d", cur_num);

  // Reset the color to normal
  SetColor(COLOR_WHITE);
}

void cHistScreen::PrintSpecies(cSpecies * in_species, int in_pos, int max_num)
{
  //  SetBoldColor(COLOR_CYAN);
  //  if (in_gen->GetGestationTime())
  //    PrintFitness(in_pos, 0, in_gen->GetFitness());
  //  else Print(in_pos, 0, "    0.0");

  SetBoldColor(COLOR_WHITE);
  Print(in_pos, 0, "        sp-%06d: ", in_species->GetID());

  int max_stars = Width() - 28;
  int star_size = (max_num / max_stars);
  if (max_num % max_stars) star_size++;

  int cur_num = in_species->GetNumOrganisms();
  int cur_stars = cur_num / star_size;
  if (cur_num % star_size) cur_stars++;

  // Set the color for this bar.
  SetSymbolColor(in_species->GetSymbol());

  // Draw the bar.
  int i;
  for (i = 0; i < cur_stars; i++) {
    Print(in_species->GetSymbol());
  }

  // Draw the spaces following the bar.
  while (i++ < max_stars) Print(' ');

  // Display the true length of the bar (highlighted)
  SetBoldColor(COLOR_WHITE);
  Print(in_pos, Width() - 8, " %5d", cur_num);

  // Reset the color to normal
  SetColor(COLOR_WHITE);
}


void cHistScreen::Draw()
{
  SetBoldColor(COLOR_WHITE);
  Print(1,  0, "Fitness Name");
  Print(1, 19, "Histogram: [ ]");
  Print(1, 53, "[ ]");
  SetBoldColor(COLOR_CYAN);
  Print(1, 31, '<');
  Print(1, 54, '>');

  Update();
}

void cHistScreen::Update()
{
  int max_num = 0;
  int i;

  switch(mode) {
  case HIST_GENOTYPE:
    max_num = info.GetGenebank().GetBestGenotype()->GetNumOrganisms();
    SetBoldColor(COLOR_WHITE);
    Print(1,  34, "Genotype Abundance");
    // Print out top NUM_SYMBOL genotypes in fixed order.
    for (i = 0; i < info.GetNumSymbols(); i++) {
      if (info.GetGenotype(i)) {
	PrintGenotype(info.GetGenotype(i), i + 2, max_num);
      }
      else {
	Move(i + 2, 0);
	ClearToEOL();
      }
    }
    break;
  case HIST_SPECIES:
    max_num = 0;
    for (i = 0; i < NUM_SYMBOLS; i++) {
      if (info.GetSpecies(i) && info.GetSpecies(i)->GetNumOrganisms()
	  > max_num)
	max_num = info.GetSpecies(i)->GetNumOrganisms();
    }

    SetBoldColor(COLOR_WHITE);
    Print(1,  34, "Species Abundance");

    // Print out top number of symbols species in fixed order.
    for (i = 0; i < info.GetNumSymbols(); i++) {
      if (info.GetSpecies(i)) {
	PrintSpecies(info.GetSpecies(i), i + 2, max_num);
      }
      else {
	Move(i + 2, 0);
	ClearToEOL();
      }
    }
    break;
  }

  ClearToBot();
  Refresh();
}

void cHistScreen::DoInput(int in_char)
{
  switch(in_char) {
  case '<':
  case ',':
    ++mode %= NUM_HIST;
    Update();
    break;
  case '>':
  case '.':
    mode += NUM_HIST;
    --mode %= NUM_HIST;
    Update();
    break;
  }
}


