//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cStatsScreen.h"

#include "cClassificationManager.h"
#include "cEnvironment.h"
#include "cGenotype.h"
#include "cPopulation.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cTaskEntry.h"

using namespace std;


void cStatsScreen::Draw()
{
  SetBoldColor(COLOR_WHITE);

  // Print(0,  0, "--- Soup Status ---");
  Print(1,  0, "Tot Births.:");
  Print(2,  0, "Breed True.:");
  Print(3,  0, "Parasites..:");
  Print(4,  0, "Energy.....:");
  Print(5,  0, "Max Fitness:");
  Print(6,  0, "Max Merit..:");

  Print(1, 23, "-- Dominant Genotype --");
  Print(2, 23, "Name........:");
  Print(3, 23, "ID..........:");
  Print(4, 23, "Species ID..:");
  Print(5, 23, "Age.........:");

  Print(8, 11, "Current    Total  Ave Age  Entropy");
  Print(9,  0, "Creatures:");
  Print(10, 0, "Genotypes:");
  Print(11, 0, "Threshold:");
  Print(12, 0, "Species..:");

  Print(1, 61, "Dominant  Average");
  Print(2, 50, "Fitness..:");
  Print(3, 50, "Merit....:");
  Print(4, 50, "Gestation:");
  Print(5, 50, "Size.....:");
  Print(6, 50, "Copy Size:");
  Print(7, 50, "Exec Size:");
  Print(8, 50, "Abundance:");
  Print(9, 50, "Births...:");
  Print(10, 50, "BirthRate:");
  Print(11, 50, "TreeDepth:");
  Print(12, 50, "Gen. Ave.:");


  int task_num = task_offset;
  const cTaskLib & task_lib = m_world->GetEnvironment().GetTaskLib();
  for (int col_id = 3; task_num < info.GetWorld().GetNumTasks(); col_id += 20) {
    if (col_id + 16 > Width()) break;
    for (int row_id = 15;
	 row_id < 15 + task_rows && task_num < info.GetWorld().GetNumTasks();
	 row_id++) {
      Print(row_id, col_id, ".........:");
      Print(row_id, col_id, "%s", static_cast<const char*>(task_lib.GetTask(task_num).GetName()));
      task_num++;
    }
  }

  SetColor(COLOR_WHITE);

  Box(0, 14, Width(), task_rows + 2);

  if (task_num < info.GetWorld().GetNumTasks() || task_offset != 0) {
    SetBoldColor(COLOR_WHITE);
    Print(15 + task_rows, Width() - 20, " [<-] More [->] ");
    SetBoldColor(COLOR_CYAN);
    Print(15 + task_rows, Width() - 18, "<-");
    Print(15 + task_rows, Width() - 8, "->");
  }

  Update();
}

void cStatsScreen::Update()
{
  cGenotype * best_gen = m_world->GetClassificationManager().GetBestGenotype();

  SetBoldColor(COLOR_CYAN);

  cStats& stats = m_world->GetStats();

  Print(1, 13, "%7d",   stats.GetNumBirths());
  Print(2, 13, "%7d",   stats.GetBreedTrue());
  Print(3, 13, "%7d",   stats.GetNumParasites());
  PrintDouble(4, 13, stats.GetEnergy());
  PrintDouble(5, 13, stats.GetMaxFitness());
  PrintDouble(6, 13, stats.GetMaxMerit());

  Print(9,  13, "%5d", stats.GetNumCreatures());
  Print(10, 13, "%5d", stats.GetNumGenotypes());
  Print(11, 13, "%5d", stats.GetNumThreshold());
  Print(12, 13, "%5d", stats.GetNumSpecies());

  Print(2, 37, "%s",  static_cast<const char*>(best_gen->GetName()));
  Print(3, 37, "%9d", best_gen->GetID());
  Print(4, 37, "%9d", (best_gen->GetSpecies()) ?
	(best_gen->GetSpecies()->GetID()) : -1);
  Print(5, 37, "%9d", stats.GetUpdate() - best_gen->GetUpdateBorn());

  PrintDouble(9,  20, (double) stats.GetTotCreatures());
  PrintDouble(10, 20, (double) stats.GetTotGenotypes());
  PrintDouble(11, 20, (double) stats.GetTotThreshold());
  PrintDouble(12, 20, (double) stats.GetTotSpecies());

  PrintDouble(9,  29, stats.GetAveCreatureAge());
  PrintDouble(10, 29, stats.GetAveGenotypeAge());
  PrintDouble(11, 29, stats.GetAveThresholdAge());
  PrintDouble(12, 29, stats.GetAveSpeciesAge());

  PrintDouble(9,  38, log((double) stats.GetNumCreatures()));
  PrintDouble(10, 38, stats.GetEntropy());
  PrintDouble(12, 38, stats.GetSpeciesEntropy());

  PrintDouble(2, 62, best_gen->GetFitness());
  PrintDouble(3, 62, best_gen->GetMerit());
  PrintDouble(4, 62, best_gen->GetGestationTime());
  Print(5, 62, "%7d", best_gen->GetLength());
  PrintDouble(6, 62, best_gen->GetCopiedSize());
  PrintDouble(7, 62, best_gen->GetExecutedSize());
  Print(8, 62, "%7d", best_gen->GetNumOrganisms());
  Print(9, 62, "%7d", best_gen->GetThisBirths());
  if (stats.GetAveMerit() == 0) {
    PrintDouble(10, 62, 0.0);
  } else {
    PrintDouble(10, 62, ((double) info.GetConfig().AVE_TIME_SLICE.Get()) * best_gen->GetFitness() / stats.GetAveMerit());
  }
  Print(11, 62, "%7d", best_gen->GetDepth());
  // Print(12, 63, "");

  PrintDouble(2, 71, stats.GetAveFitness());
  PrintDouble(3, 71, stats.GetAveMerit());
  PrintDouble(4, 71, stats.GetAveGestation());
  PrintDouble(5, 71, stats.GetAveSize());
  PrintDouble(6, 71, stats.GetAveCopySize());
  PrintDouble(7, 71, stats.GetAveExeSize());
  PrintDouble(8, 71,
	      ((double) stats.GetNumCreatures()) /
	      ((double) stats.GetNumGenotypes()));
  // @CAO this next line should be get num births, which doesn't exist!
  PrintDouble(9, 71,
	      ((double) stats.GetNumDeaths()) /
	      ((double) stats.GetNumGenotypes()));
  if (stats.GetAveMerit() != 0) {
    PrintDouble(10, 71, ((double) info.GetConfig().AVE_TIME_SLICE.Get()) * stats.GetAveFitness() / stats.GetAveMerit());
  } else {
    PrintDouble(10, 71, 0.0);
  }
  PrintDouble(11, 71, stats.SumGenotypeDepth().Average());
  PrintDouble(12, 71, stats.SumGeneration().Average());

  // This section needs to be changed to work with new task_lib @TCC
  int task_num = task_offset;
  for (int col_id = 14; task_num < info.GetWorld().GetNumTasks(); col_id += 20) {
    if (col_id + 5 > Width()) break;
    for (int row_id = 15;
	 row_id < 15 + task_rows && task_num < info.GetWorld().GetNumTasks();
	 row_id++) {
      Print(row_id, col_id, "%4d", stats.GetTaskLastCount(task_num));
      task_num++;
    }
  }

  SetColor(COLOR_WHITE);

  Refresh();
}

void cStatsScreen::DoInput(int in_char)
{
  switch (in_char) {
  case '4':
  case KEY_LEFT:
    if (task_offset > 0) {
      task_offset -= 5;
      Draw();
    }
    break;
  case '6':
  case KEY_RIGHT:
    if (task_rows * task_cols + task_offset < info.GetWorld().GetNumTasks()) {
      task_offset += 5;
      Draw();
    }
    break;
  default:
    // g_debug.Warning("Unknown Command (#%d): [%c]", in_char, in_char);
    break;
  }
}

