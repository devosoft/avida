//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cZoomScreen.h"

#include "cEnvironment.h"
#include "cHardwareBase.h"
#include "cHardwareCPU.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStringUtil.h"

#include "nHardware.h"

#include "cView.h"
#include "cMenuWindow.h"


using namespace std;


/////////////////////
//  The Zoom Screen
/////////////////////

cZoomScreen::cZoomScreen(int y_size, int x_size, int y_start, int x_start, cViewInfo& in_info, cPopulation& in_pop)
: cScreen(y_size, x_size, y_start, x_start, in_info), population(in_pop)
{
  memory_offset = 0;
  parasite_zoom = false;
  mode = ZOOM_MODE_STATS;
  //map_mode = MAP_BASIC;
  inst_view_mode = true;
  active_section = ZOOM_SECTION_MEMORY;
  task_offset = 0;
  //hardware_type = info.GetActiveCell()->GetOrganism()->GetHardware().GetType();
  cur_stack=0;
  cur_mem_space=0;
  cur_view_thread=0;
  
  //map_mode = MAP_BASIC;
  mini_center_id = 0;
  map_x_size = population.GetWorldX();
  map_y_size = population.GetWorldY();
}




void cZoomScreen::Draw(cAvidaContext& ctx)
{
  // Draw the options block which is on all screens.
  
  SetBoldColor(COLOR_WHITE);
  PrintOption(OPTIONS_Y, OPTIONS_X, "[<]                [>]");
  if (info.GetPauseLevel()) {
    PrintOption(OPTIONS_Y+2, OPTIONS_X+2, "Un-[P]ause");
    PrintOption(OPTIONS_Y+3, OPTIONS_X+2, "[N]ext Update");
    PrintOption(OPTIONS_Y+4, OPTIONS_X+2, "[Space] Next Inst");
  } else {
    PrintOption(OPTIONS_Y+2, OPTIONS_X+2, "[P]ause   ");
    Print(OPTIONS_Y+3, OPTIONS_X+2, "             ");
    Print(OPTIONS_Y+4, OPTIONS_X+2, "                 ");
  }
  
  // Redirect to the proper Draw() method.
  if (mode == ZOOM_MODE_CPU)
  {
    if(info.GetConfig().HARDWARE_TYPE.Get() == HARDWARE_TYPE_CPU_ORIGINAL) 
      DrawCPU_Original();
    else
      DrawCPU_SMT();
  }
  else if (mode == ZOOM_MODE_STATS) DrawStats();
  else if (mode == ZOOM_MODE_GENOTYPE) DrawGenotype();
  
  Update(ctx);
  Refresh();
}

void cZoomScreen::DrawStats()
{
  // Creature Status
  
  SetBoldColor(COLOR_WHITE);
  
  Print(1, 0, "GenotypeID:");
  Print(2, 0, "Geno Name.:");
  Print(3, 0, "Species.ID:");
  
  Print(5, 0, "Fitness...:");
  Print(6, 0, "Gestation.:");
  Print(7, 0, "Cur Energy:");
  Print(8, 0, "Str Energy:");
  Print(9, 0, "CPU Speed.:");
  Print(10, 0, "Cur Merit.:");
  Print(11, 0, "GenomeSize:");
  Print(12, 0, "Mem Size..:");
  Print(13, 0, "Faults....:");
  
  Print(1,  27, "Location..:");
  
  Print(4,  27, "Generation:");
  Print(5,  27, "Age.......:");
  Print(6,  27, "Executed..:");
  Print(7,  27, "LastDivide:");
  Print(8,  27, "Offspring.:");
  
  Print(10, 27, "Thread:");
  Print(11, 27, "IP....:");
  
  if(info.GetConfig().HARDWARE_TYPE.Get() == HARDWARE_TYPE_CPU_ORIGINAL) {
    Print(12, 27, "AX....:");
    Print(13, 27, "BX....:");
    Print(14, 27, "CX....:");
    Print(15, 27, "Stack.:");
    Print(16, 27, "---- Memory ----");
  } else {
    Print(12, 27, "Stack AX:");
    Print(13, 27, "Stack BX:");
    Print(14, 27, "Stack CX:");
    Print(15, 27, "Stack DX:");
    Print(16, 27, "---- Memory ----");
  }
  
  Print(CPU_FLAGS_Y, CPU_FLAGS_X, "---- Flags ----");
  
  // --== Options ==--
  SetBoldColor(COLOR_YELLOW);
  Print(OPTIONS_Y, OPTIONS_X+4, "CPU Stats Zoom");
  
  // --== Tasks ==--
  SetColor(COLOR_WHITE);
  Box(TASK_Y, TASK_X, Height()-TASK_Y, 30, true);
  
  
  int task_num = task_offset;
  int col_num = 0;
  const cEnvironment& environment = info.GetWorld().GetEnvironment();
  const int num_tasks = environment.GetNumTasks();
  for (int cur_col = TASK_X + 2; task_num < num_tasks; cur_col += 14) {
    for (int cur_row = TASK_Y + 1; cur_row < Height() - 1 && task_num < num_tasks; cur_row++) {
      Print(cur_row, cur_col, "........:");
      Print(cur_row, cur_col, "%s", static_cast<const char*>(environment.GetTask(task_num).GetName()));
      task_num++;
    }
    col_num++;
    if (col_num == 2) break;
  }
  
  if (task_num < num_tasks || task_offset != 0) {
    SetBoldColor(COLOR_WHITE);
    PrintOption(Height()-1, Width() - 23, " [<-] More [->] ");
  }
}

void cZoomScreen::DrawCPU_Original()
{
  SetColor(COLOR_WHITE);
  
  // --== Registers ==--
  Box(REG_Y, REG_X, 7, 19, true);
  Print(REG_Y + 1, REG_X + 2, "Registers");
  HLine(REG_Y + 2, REG_X, 19);
  
  SetBoldColor(COLOR_WHITE);
  Print(REG_Y + 3, REG_X + 2, "AX:");
  Print(REG_Y + 4, REG_X + 2, "BX:");
  Print(REG_Y + 5, REG_X + 2, "CX:");
  SetColor(COLOR_WHITE);
  
  // --== Inputs ==--
  Box(INPUT_Y, INPUT_X, 7, 16, true);
  Print(INPUT_Y + 1, INPUT_X + 2, "Inputs");
  HLine(INPUT_Y + 2, INPUT_X, 16);
  
  // --== Mini-Map ==--
  Box(MINI_MAP_Y, MINI_MAP_X, 3, 17, true);
  Print(MINI_MAP_Y + 1, MINI_MAP_X + 2, "Mini-Map");
  // HLine(MINI_MAP_Y + 2, MINI_MAP_X, 19);
  
  SetBoldColor(COLOR_WHITE);
  PrintOption(MINI_MAP_Y + 11, MINI_MAP_X,  "[-]           [+]");
  SetColor(COLOR_WHITE);
  
  // --== Memory ==--
  Box(MEMORY_Y, MEMORY_X, 5 + MEMORY_PRE_SIZE + MEMORY_POST_SIZE, 36, true);
  Print(MEMORY_Y + 1, MEMORY_X + 2,  "Memory:");
  HLine(MEMORY_Y + 2, MEMORY_X, 36);
  
  // --== Stack ==--
  Box(STACK_Y, STACK_X, 7, 15, true);
  HLine(STACK_Y + 2, STACK_X, 15);
  
  // --== Options ==--
  SetBoldColor(COLOR_YELLOW);
  Print(OPTIONS_Y, OPTIONS_X+4,    "Component Zoom");
  SetBoldColor(COLOR_WHITE);
  
  if (info.GetPauseLevel()) {
    PrintOption(OPTIONS_Y+5, OPTIONS_X+2, "[Arrows] Scroll");
  } else {
    Print(OPTIONS_Y+5, OPTIONS_X+2, "               ");
  }
  PrintOption(OPTIONS_Y+6, OPTIONS_X+2, "[Enter] View/Edit");
  PrintOption(OPTIONS_Y+7, OPTIONS_X+2, "[TAB] Shift Active");
  
  if(info.GetConfig().MAX_CPU_THREADS.Get() >1) {
    PrintOption(OPTIONS_Y+8, OPTIONS_X+2, "Next [T]hread");
  }
  
  
  // Highlight the active section...
  SetActiveSection(active_section);
  
  // Add on a bunch of special characters to smooth the view out...
  Print(INPUT_Y,     INPUT_X, CHAR_TTEE);
  Print(INPUT_Y + 2, INPUT_X, CHAR_PLUS);
  Print(INPUT_Y + 6, INPUT_X, CHAR_BTEE);
  
  Print(STACK_Y,     STACK_X, CHAR_TTEE);
  Print(STACK_Y + 2, STACK_X, CHAR_PLUS);
  Print(STACK_Y + 6, STACK_X, CHAR_BTEE);
  
  
  // A few stats on this screen...
  
  SetBoldColor(COLOR_WHITE);
  
  Print(13, 52, "Location.....:");
  Print(14, 52, "Genotype ID..:");
  Print(15, 52, "Genotype Name:");
  Print(16, 52, "Cur Energy...:");  
  Print(17, 52, "Faults.......:");
  Print(18, 52, "Offspring....:");
  Print(19, 52, "Thread.......:");
}

void cZoomScreen::DrawCPU_SMT()
{
  SetColor(COLOR_WHITE);
  
  // --== Registers ==--
  Box(REG_Y-1, REG_X, 8, 19, true);
  Print(REG_Y, REG_X + 2, "Stacks:");
  HLine(REG_Y + 1, REG_X, 19);
  
  SetBoldColor(COLOR_WHITE);
  Print(REG_Y + 2, REG_X + 2, "AX:");
  Print(REG_Y + 3, REG_X + 2, "BX:");
  Print(REG_Y + 4, REG_X + 2, "CX:");
  Print(REG_Y + 5, REG_X + 2, "DX:");
  SetColor(COLOR_WHITE);
  
  // --== Inputs ==--
  Box(INPUT_Y-1, INPUT_X, 8, 16, true);
  Print(INPUT_Y, INPUT_X + 2, "Inputs:");
  HLine(INPUT_Y+1, INPUT_X, 16);
  
  // --== Mini-Map ==--
  Box(MINI_MAP_Y, MINI_MAP_X, 3, 17, true);
  Print(MINI_MAP_Y + 1, MINI_MAP_X + 2, "Mini-Map");
  //HLine(MINI_MAP_Y + 2, MINI_MAP_X, 19);
  
  SetBoldColor(COLOR_WHITE);
  Print(MINI_MAP_Y + 11, MINI_MAP_X,  "[ ]           [ ]");
  SetBoldColor(COLOR_CYAN);
  Print(MINI_MAP_Y + 11, MINI_MAP_X + 1,  '-');
  Print(MINI_MAP_Y + 11, MINI_MAP_X + 15, '+');
  SetColor(COLOR_WHITE);
  
  // --== Memory ==--
  Box(MEMORY_Y, MEMORY_X, 5 + MEMORY_PRE_SIZE + MEMORY_POST_SIZE, 36, true);
  Print(MEMORY_Y + 1, MEMORY_X + 2,  "Memory Space");
  HLine(MEMORY_Y + 2, MEMORY_X, 36);
  
  // --== Stack ==--
  Box(STACK_Y-1, STACK_X, 8, 15, true);
  HLine(STACK_Y + 1, STACK_X, 15);
  
  // --== Options ==--
  SetBoldColor(COLOR_YELLOW);
  Print(OPTIONS_Y, OPTIONS_X+4,    "Component Zoom");
  SetBoldColor(COLOR_WHITE);
  
  //Print(OPTIONS_Y+6, OPTIONS_X+2, "[E]dit Component");
  //Print(OPTIONS_Y+7, OPTIONS_X+2, "[V]iew Component");
  Print(OPTIONS_Y+5, OPTIONS_X+2, "Next Stac[K]");
  if(info.GetConfig().MAX_CPU_THREADS.Get() >1)
    Print(OPTIONS_Y+6, OPTIONS_X+2, "Next [T]hread");
  //Print(OPTIONS_Y+9, OPTIONS_X+2, "[TAB] Shift Active");
  
  if (info.GetPauseLevel()) {
    Print(OPTIONS_Y+7, OPTIONS_X+2, "[UP, DOWN]");
    Print(OPTIONS_Y+8, OPTIONS_X+2, "Scroll Instruction");
    Print(OPTIONS_Y+9, OPTIONS_X+2, "[LEFT, RIGHT]");
    Print(OPTIONS_Y+10, OPTIONS_X+2, "Change Mem Space");
  } else {
    Print(OPTIONS_Y+7, OPTIONS_X+2, "               ");
    Print(OPTIONS_Y+8, OPTIONS_X+2, "                  ");
    Print(OPTIONS_Y+9, OPTIONS_X+2, "               ");
    Print(OPTIONS_Y+10, OPTIONS_X+2, "                 ");
  }
  
  
  SetBoldColor(COLOR_CYAN);
  Print(OPTIONS_Y+5, OPTIONS_X+12, "K");
  if(info.GetConfig().MAX_CPU_THREADS.Get() >1)
    Print(OPTIONS_Y+6, OPTIONS_X+8, "T");
  
  if (info.GetPauseLevel()) {
    Print(OPTIONS_Y+7, OPTIONS_X+3, "UP, DOWN");
    Print(OPTIONS_Y+9, OPTIONS_X+3, "LEFT, RIGHT");
  }
  
  // Highlight the active section...
  SetActiveSection(active_section);
  
  // Add on a bunch of special characters to smooth the view out...
  Print(INPUT_Y - 1, INPUT_X, CHAR_TTEE);
  Print(INPUT_Y + 1, INPUT_X, CHAR_PLUS);
  Print(INPUT_Y + 6, INPUT_X, CHAR_BTEE);
  
  Print(STACK_Y - 1, STACK_X, CHAR_TTEE);
  Print(STACK_Y + 1, STACK_X, CHAR_PLUS);
  Print(STACK_Y + 6, STACK_X, CHAR_BTEE);
  
  
  // A few stats on this screen...
  
  SetBoldColor(COLOR_WHITE);
  
  Print(13, 52, "Location.....:");
  Print(14, 52, "Genotype ID..:");
  Print(15, 52, "Genotype Name:");
  
  Print(17, 52, "Faults.......:");
  Print(18, 52, "Offspring....:");
  Print(19, 52, "Thread.......:");
}

void cZoomScreen::DrawGenotype()
{
  SetBoldColor(COLOR_YELLOW);
  Print(OPTIONS_Y, OPTIONS_X + 4, "Genotype Zoom");
  
  // Genotype status.
  
  SetBoldColor(COLOR_WHITE);
  
  Print(1, 0, "GenotypeID:");
  Print(2, 0, "Geno Name.:");
  
  Print(5, 0, "Abundance.:");
  Print(6, 0, "Length....:");
  Print(7, 0, "CopyLength:");
  Print(8, 0, "Exe Length:");
  
  Print(10, 0, "Fitness...:");
  Print(11, 0, "Gestation.:");
  Print(12, 0, "Merit.....:");
  Print(13, 0, "ReproRate.:");
  
  Print(1, 27, "Update Born:");
  Print(2, 27, "Parents....:");
  Print(4, 27, "Gene Depth.:");
  
  Print(6, 27,  "-- This Update --");
  Print(7, 27,  "Deaths.....:");
  Print(8, 27,  "Divides....:");
  Print(9, 27,  "Breed True.:");
  Print(10, 27, "Breed In...:");
  Print(11, 27, "Breed Out..:");
  
  Print(13, 27, "-- Totals --");
  Print(14, 27, "Abundance..:");
  Print(15, 27, "Divides....:");
  Print(16, 27, "Breed True.:");
  Print(17, 27, "Breed In...:");
  Print(18, 27, "Breed Out..:");
}

void cZoomScreen::Update(cAvidaContext& ctx)
{
  if (info.GetActiveCell() == NULL ||
      info.GetActiveCell()->IsOccupied() == false) return;
  
  cHardwareBase& hardware = info.GetActiveCell()->GetOrganism()->GetHardware();
  if(mode == ZOOM_MODE_CPU) UpdateCPU(hardware);
  else if (mode == ZOOM_MODE_STATS) UpdateStats(hardware);
  else if (mode == ZOOM_MODE_GENOTYPE) UpdateGenotype(ctx);
  
  Refresh();
}

void cZoomScreen::UpdateStats(cHardwareBase& hardware)
{
  if (info.GetActiveCell() == NULL ||
      info.GetActiveCell()->IsOccupied() == false) return;
  
  cBioGroup* genotype = info.GetActiveGenotype();
  cOrganism* organism = info.GetActiveCell()->GetOrganism();
  cPhenotype& phenotype = organism->GetPhenotype();
  
  SetBoldColor(COLOR_CYAN);
  
  // if there is an Active Genotype name AND it is not empty, show it
  Print(1, 12, "%9d", info.GetActiveGenotypeID());
  Print(2, 12, "%s", static_cast<const char*>(info.GetActiveName()));
  
  const cMerit cur_merit(phenotype.GetCurBonus());
  
  PrintDouble(5, 14, phenotype.GetFitness());
  Print(6, 15, "%6d ", phenotype.GetGestationTime());
  PrintDouble(7, 14, phenotype.GetStoredEnergy());
  PrintDouble(8, 14, phenotype.GetEnergyBonus());
  PrintDouble(9, 14, phenotype.GetMerit().GetDouble());
  PrintDouble(10, 14, cur_merit.GetDouble());
  Print(11, 15, "%6d ", genotype ? cGenome(genotype->GetProperty("genome").AsString()).GetSize() : 0);
  Print(12, 15, "%6d ", hardware.GetMemory().GetSize());
  
  Print(13, 15, "%6d ", phenotype.GetCurNumErrors());
  
  Print(4, 39, "%9d ", phenotype.GetGeneration());
  Print(5, 39, "%9d ", phenotype.GetAge());
  Print(6, 39, "%9d ", phenotype.GetTimeUsed());
  Print(7, 39, "%9d ", phenotype.GetGestationStart());
  Print(8, 39, "%9d ", phenotype.GetNumDivides());
  
  if (info.GetThreadLock() != -1) Print(10, 36, "LOCKED");
  else Print(10, 36, "      ");
  
  if(info.GetConfig().HARDWARE_TYPE.Get() == HARDWARE_TYPE_CPU_ORIGINAL)
    UpdateStats_CPU(hardware);
  else
    UpdateStats_SMT(hardware);
  
  if (phenotype.ParentTrue()) SetBoldColor(COLOR_CYAN);
  else SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 2, CPU_FLAGS_X + 1, "Parent True");
  
  if (phenotype.IsInjected()) SetBoldColor(COLOR_CYAN);
  else SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 3, CPU_FLAGS_X + 1, "Injected");
  
  if (organism->GetNumParasites()) SetBoldColor(COLOR_CYAN);
  else SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 4, CPU_FLAGS_X + 1, "Parasite");
  
  if (phenotype.IsMutated()) SetBoldColor(COLOR_CYAN);
  else SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 5, CPU_FLAGS_X + 1, "Mutated");
  
  if (phenotype.IsModified()) SetBoldColor(COLOR_CYAN);
  else SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 6, CPU_FLAGS_X + 1, "Modified");
  
  SetColor(COLOR_WHITE);
  
  //  @DMB - This needs to be reworked for current parasite implementation
  //  if (info.GetPauseLevel() && info.GetActiveCell() && phenotype.IsParasite()) {
  //    if (parasite_zoom == true) Print(17, 12, "[X] Host Code    ");
  //    else Print(17, 12, "[X] Parasite Code");
  //  }
  
  // Place the task information onto the screen.
  SetColor(COLOR_CYAN);
  
  int task_num = task_offset;
  int num_tasks = info.GetWorld().GetEnvironment().GetNumTasks();
  int col_num = 0;
  for (int cur_col = TASK_X + 12; task_num < num_tasks; cur_col += 14) {
    for (int cur_row = TASK_Y + 1; cur_row <= Height() - 2 && task_num < num_tasks; cur_row++) {
      if (col_num < 2) {
        Print(cur_row, cur_col, "%2d", phenotype.GetCurTaskCount()[task_num]);
      }
      task_num++;
    }
    col_num++;
  }
  
  
  
  if (info.GetActiveCell() == NULL) {
    info.SetActiveCell(&(population.GetCell(0)) );
  }
  SetBoldColor(COLOR_WHITE);
  
  // Figure out which CPU we're pointing at (useful for watching parasites)
  Print(2, 27, "Facing....:");
  
  // Show the location of the CPU we are looking at.
  SetBoldColor(COLOR_CYAN);
  Print(1, 40, "[%2d, %2d] ",
        info.GetActiveID() % population.GetWorldX(),
        info.GetActiveID() / population.GetWorldY());
  
  // Now show the location of the CPU we are facing.
  int id = info.GetActiveCell()->ConnectionList().GetFirst()->GetID();
  Print(2, 40, "[%2d, %2d] ",
        id % population.GetWorldX(), id / population.GetWorldY());
  
  
  
}

void cZoomScreen::UpdateStats_CPU(cHardwareBase& hardware)
{
  Print(10, 43, "%2d/%2d", hardware.GetCurThread() + 1,
        hardware.GetNumThreads());
  
  Print(12, 34, "%14d", hardware.GetRegister(0));
  Print(13, 34, "%14d", hardware.GetRegister(1));
  Print(14, 34, "%14d", hardware.GetRegister(2));
  Print(15, 34, "%14d", hardware.GetStack(0));
  
  cHeadCPU inst_ptr(hardware.IP());
  const cInstSet& inst_set = hardware.GetInstSet();
  
  for (int pos = 0; pos < 3; pos++) {
    // Clear the line
    Print(17+pos, 29, "                    ");
    if (inst_ptr.InMemory() == false) continue;
    if (pos == 1) SetColor(COLOR_CYAN);
    
    // Draw the current instruction.
    Print(17+pos, 29, "%s",	static_cast<const char*>(inst_set.GetName(inst_ptr.GetInst())));
    inst_ptr.Advance();
  }
  
  // Flags...
  //  if (hardware.GetMalActive()) SetBoldColor(COLOR_CYAN);
  //  else SetColor(COLOR_CYAN);
  SetColor(COLOR_CYAN);
  Print(CPU_FLAGS_Y + 1, CPU_FLAGS_X + 1, "Mem Allocated");
  
  // And print the IP.
  const cHeadCPU& active_inst_ptr = hardware.IP();
  // @CAO assume no parasites for now.
  int cur_id = info.GetActiveCell()->GetID();
  //active_inst_ptr.GetCurHardware()->GetOrganism()->GetEnvironment()->GetID();
  Print(11, 36, "%12s", static_cast<const char*>(cStringUtil::Stringf("[%2d,%2d] : %2d",
                                                                      cur_id % population.GetWorldX(), cur_id / population.GetWorldX(),
                                                                      active_inst_ptr.GetPosition())) );
}

void cZoomScreen::UpdateStats_SMT(cHardwareBase& hardware)
{
  Print(10, 43, "%2d/%2d", hardware.GetCurThread() + 1,
        hardware.GetNumThreads());
  
  Print(12, 34, "%14d", hardware.GetStack(0, 0));
  Print(13, 34, "%14d", hardware.GetStack(0, 1));
  Print(14, 34, "%14d", hardware.GetStack(0, 2));
  Print(15, 34, "%14d", hardware.GetStack(0, 3));
  
  cHeadCPU inst_ptr(hardware.IP());
  const cInstSet& inst_set = hardware.GetInstSet();
  
  for (int pos = 0; pos < 3; pos++) {
    // Clear the line
    Print(17+pos, 29, "                    ");
    if (inst_ptr.InMemory() == false) continue;
    if (pos == 1) SetColor(COLOR_CYAN);
    
    // Draw the current instruction.
    Print(17+pos, 29, "%s",	static_cast<const char*>(inst_set.GetName(inst_ptr.GetInst())));
    inst_ptr.Advance();
  }
  
  // And print the IP.
  const cHeadCPU& ip = hardware.IP();
  // @CAO assume no parasites for now.
  Print(11, 36, "%12s", static_cast<const char*>(cStringUtil::Stringf("(%2d, %2d)", ip.GetMemSpace(), ip.GetPosition())));
}

void cZoomScreen::UpdateCPU(cHardwareBase& hardware)
{
  // Place the visible section of the current memory onto the screen.
  
  SetBoldColor(COLOR_WHITE);
  if (info.GetPauseLevel() == PAUSE_OFF) {
    // If not paused, then set user desired offset to zero AND always show
    // the code that is executing (ie. do not show original Parasite code if
    // host is executing)
    memory_offset = 0;
    parasite_zoom = false;
  }
  
  Print(14, 69, "%10d", info.GetActiveGenotypeID());
  Print(15, 69, "%10s", static_cast<const char*>(info.GetActiveName()));
  
  cPhenotype& phenotype = info.GetActiveCell()->GetOrganism()->GetPhenotype();
  PrintDouble(16, 69, phenotype.GetStoredEnergy());
  PrintDouble(17, 69, phenotype.GetEnergyBonus());
  
  Print(18, 69, "%10d", phenotype.GetCurNumErrors());
  Print(19, 69, "%10d", phenotype.GetNumDivides());
  if (info.GetThreadLock() != -1) Print(20, 67, "LOCKED");
  else Print(20, 67, "      ");
  
  if(info.GetConfig().HARDWARE_TYPE.Get() == HARDWARE_TYPE_CPU_ORIGINAL)
    UpdateCPU_Original(hardware);
  else
    UpdateCPU_SMT(hardware);
  
  // Place the input buffer on the screen.
  
  SetBoldColor(COLOR_CYAN);
  Print(INPUT_Y+3, INPUT_X+2, "%12d", info.GetActiveCell()->GetInput(0));
  
  SetColor(COLOR_CYAN);
  for (int i = 1; i < info.GetActiveCell()->GetInputSize(); i++) {
    Print(INPUT_Y+3+i, INPUT_X+2, "%12d", info.GetActiveCell()->GetInput(i));
  }
  
  const cString& cur_fault = phenotype.GetFault();
  if (cur_fault.GetSize() > 0) {
    SetBoldColor(COLOR_RED);
    Print(FAULT_Y, FAULT_X, "Fault:");
    SetBoldColor(COLOR_CYAN);
    Print(FAULT_Y, FAULT_X + 7, static_cast<const char*>(cur_fault));
  } else {
    Print(FAULT_Y, FAULT_X, "                                        ");
  }
  
  SetBoldColor(COLOR_WHITE);
  
  Print(13, 70, "[%3d,%3d] ",
        info.GetActiveID() % population.GetWorldX(),
        info.GetActiveID() / population.GetWorldY());
  
  SetBoldColor(COLOR_CYAN);
  Refresh();
}

void cZoomScreen::UpdateCPU_Original(cHardwareBase& hardware)
{
  // @DMB - This is making a relatively safe assumption of hardware type, but it IS an assumption
#ifdef DEBUG
  cHardwareCPU* p_hw = dynamic_cast<cHardwareCPU*>(&hardware);
  assert(p_hw);
  cHardwareCPU& hardwareCPU = *p_hw;
#else
  cHardwareCPU& hardwareCPU = static_cast<cHardwareCPU&>(hardware);
#endif
  
  
  // Place the registers onto the screen.
  SetBoldColor(COLOR_CYAN);
  for (int i = 0; i < hardware.GetNumRegisters(); i++) {
    Print(REG_Y+3 + i, REG_X+6, "%11d", hardware.GetRegister(i));
  }
  
  // Place the active stack onto the screen.
  
  // Stack A
  // SetBoldColor(COLOR_CYAN);   // -Redundant
  SetColor(COLOR_WHITE);
  Print(STACK_Y + 1, STACK_X + 2, "Stack %c", 'A' + hardwareCPU.GetActiveStack());
  
  SetBoldColor(COLOR_CYAN);
  Print(STACK_Y+3, STACK_X + 2, "%11d", hardware.GetStack(0));
  SetColor(COLOR_CYAN);
  for (int i = 1; i <= 2; i++) {
    Print(STACK_Y+3 + i, STACK_X+2, "%11d", hardware.GetStack(i));
  }
  
  // Place the input buffer on the screen.
  
  SetBoldColor(COLOR_CYAN);
  Print(INPUT_Y+3, INPUT_X+2, "%12d", info.GetActiveCell()->GetInput(0));
  
  SetColor(COLOR_CYAN);
  for (int i = 1; i < info.GetActiveCell()->GetInputSize(); i++) {
    Print(INPUT_Y+3+i, INPUT_X+2, "%12d", info.GetActiveCell()->GetInput(i));
  }
  
  Print(19, 74, "%2d/%2d", hardware.GetCurThread() + 1,
        hardware.GetNumThreads());
  
  // This line gets the creature that is currently executing. Usually the
  // creature we are viewing, but can also be a different one (if this is a
  // parasite).
  
  const cCPUMemory& memory = parasite_zoom ? hardware.GetMemory() : hardware.IP().GetMemory();
  SetColor(COLOR_WHITE);
  Print(MEMORY_Y + 1, MEMORY_X + 9, "%4d", memory.GetSize());
  
  
  
  // Nothing past this point gets executed for empty creatures....
  if (memory.GetSize() == 0) {
    for (int i = -MEMORY_PRE_SIZE; i <= MEMORY_POST_SIZE; i++) {
      HLine(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, 36);
    }
    return;
  }
  
  int adj_inst_ptr = 0;
  int base_inst_ptr = hardware.IP().GetPosition();
  if (base_inst_ptr < 0 || parasite_zoom == true) base_inst_ptr = 0;
  
  const cInstSet& inst_set = hardware.GetInstSet();
  
  // Determine the center (must be between 0 and size - 1)
  int center_pos = (base_inst_ptr + memory_offset) % memory.GetSize();
  if (center_pos < 0) center_pos += memory.GetSize();
  
  for (int i = -MEMORY_PRE_SIZE; i <= MEMORY_POST_SIZE; i++) {
    adj_inst_ptr = (center_pos + i) % (memory.GetSize() + 1);
    if (adj_inst_ptr < 0) adj_inst_ptr += memory.GetSize() + 1;
    
    if (adj_inst_ptr == memory.GetSize()) {
      HLine(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, 36);
    } else {
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, CHAR_VLINE);
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 35, CHAR_VLINE);
      
      // If we are on the instruction about to be executed by the CPU,
      // hilight it...
      if (adj_inst_ptr == base_inst_ptr) {
        SetBoldColor(COLOR_WHITE);
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 1, "%3d: ",
              adj_inst_ptr);
        SetBoldColor(COLOR_CYAN);
      } else {
        if (i == 0) SetBoldColor(COLOR_YELLOW);
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 1, "%3d: ",
              adj_inst_ptr);
        // If we are scrolling through memory, make the current position
        // yellow.
        if (i == 0) SetColor(COLOR_YELLOW);
        else SetColor(COLOR_CYAN);
      }
      
      // Print the instruction...
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 6, "%8s ",
            static_cast<const char*>(inst_set.GetName(memory[adj_inst_ptr])));
      
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 15,
            "                    ");
      
      // Only list the "Cpy" flag or the "Inj" flag...
      if (memory.FlagCopied(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 15, "Cpy");
      }
      if (memory.FlagInjected(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 15, "Inj");
      }
      
      // Other flags we're interested in...
      if (memory.FlagMutated(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 19, "Mut");
      }
      if (memory.FlagExecuted(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 23, "Exe");
      }
      if (memory.FlagBreakpoint(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 27, "Bp");
      }
      
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_READ).GetPosition()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 30, "R");
      }
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_WRITE).GetPosition()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 31, "W");
      }
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_FLOW).GetPosition()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 32, "F");
      }
    }
    SetColor(COLOR_WHITE);
  }
  
  DrawMiniMap();
}


void cZoomScreen::UpdateCPU_SMT(cHardwareBase& hardware)
{
  if (cur_view_thread >= hardware.GetNumThreads())
  {
    cur_view_thread = 0;
    cur_mem_space = hardware.IP(cur_view_thread).GetMemSpace();
  }
  
  cHeadCPU& cur_ip = hardware.IP(cur_view_thread);
  
  // Place the stacks onto the screen.
  SetBoldColor(COLOR_CYAN);
  for (int i = 0; i < 4; i++) {
    Print(REG_Y+2 + i, REG_X+6, "%11d", hardware.GetStack(0, i, cur_view_thread));
  }
  
  // Place the active stack onto the screen.
  
  // Stack AX
  //SetBoldColor(COLOR_CYAN);   // -Redundant
  SetColor(COLOR_WHITE);
  
  Print(STACK_Y, STACK_X + 2, "Stack   :");
  Print(STACK_Y, STACK_X + 8, "%cX" , 'A' + cur_stack);
  
  //SetBoldColor(COLOR_CYAN);
  //Print(STACK_Y+2, STACK_X + 2, "%11d", hardware.GetStack(0, cur_stack));
  SetColor(COLOR_CYAN);
  for (int i = 0; i <= 3; i++) {
    Print(STACK_Y+2 + i, STACK_X+2, "%11d", hardware.GetStack(i, cur_stack, cur_view_thread));
  }
  
  Print(19, 74, "%2d/%2d", cur_view_thread + 1,
        hardware.GetNumThreads());
  
  // This line gets the creature that is currently executing. Usually the
  // creature we are viewing, but can also be a different one (if this is a
  // parasite).
  
  const cCPUMemory& memory = hardware.GetMemory(cur_mem_space);
  SetBoldColor(COLOR_BLUE);
  Print(MEMORY_Y + 1, MEMORY_X + 8, " Space ");	
  SetColor(COLOR_WHITE);
  Print(MEMORY_Y + 1, MEMORY_X + 15, "%2d", cur_mem_space);
  
  Print(MEMORY_Y + 1, MEMORY_X + 17, ":");	
  Print(MEMORY_Y + 1, MEMORY_X + 18, "%4d", memory.GetSize());
  
  
  
  // Nothing past this point gets executed for empty creatures....
  if (memory.GetSize() == 0) {
    for (int i = -MEMORY_PRE_SIZE; i <= MEMORY_POST_SIZE; i++) {
      HLine(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, 36);
    }
    return;
  }
  
  int adj_inst_ptr = 0;
  int base_inst_ptr = cur_ip.GetPosition();
  if (base_inst_ptr < 0 || parasite_zoom == true) base_inst_ptr = 0;
  
  const cInstSet& inst_set = hardware.GetInstSet();
  
  // Determine the center (must be between 0 and size - 1)
  int center_pos = (base_inst_ptr + memory_offset) % memory.GetSize();
  if (center_pos < 0) center_pos += memory.GetSize();
  
  for (int i = -MEMORY_PRE_SIZE; i <= MEMORY_POST_SIZE; i++) {
    adj_inst_ptr = (center_pos + i) % (memory.GetSize() + 1);
    if (adj_inst_ptr < 0) adj_inst_ptr += memory.GetSize() + 1;
    
    if (adj_inst_ptr == memory.GetSize()) {
      HLine(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, 36);
    } else {
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X, CHAR_VLINE);
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 35, CHAR_VLINE);
      
      // If we are on the instruction about to be executed by the CPU,
      // hilight it...
      if (adj_inst_ptr == base_inst_ptr && cur_ip.GetMemSpace() == cur_mem_space) {
        SetBoldColor(COLOR_WHITE);
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 1, "%3d: ",
              adj_inst_ptr);
        SetBoldColor(COLOR_CYAN);
      } else {
        if (i == 0) SetBoldColor(COLOR_YELLOW);
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 1, "%3d: ",
              adj_inst_ptr);
        // If we are scrolling through memory, make the current position
        // yellow.
        if (i == 0) SetColor(COLOR_YELLOW);
        else SetColor(COLOR_CYAN);
      }
      
      // Print the instruction...
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 6, "%8s ",
            static_cast<const char*>(inst_set.GetName(memory[adj_inst_ptr])));
      
      Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 15,
            "                    ");
      
      // Only list the "Cpy" flag or the "Inj" flag...
      if (memory.FlagCopied(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 16, "Cpy");
      }
      if (memory.FlagInjected(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 16, "Inj");
      }
      
      // Other flags we're interested in...
      if (memory.FlagMutated(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 20, "Mut");
      }
      if (memory.FlagExecuted(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 24, "Exe");
      }
      if (memory.FlagBreakpoint(adj_inst_ptr)) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 28, "Bp");
      }
      
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_READ, cur_view_thread).GetPosition() &&
          cur_mem_space == hardware.GetHead(nHardware::HEAD_READ, cur_view_thread).GetMemSpace()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 31, "R");
      }
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_WRITE, cur_view_thread).GetPosition() &&
          cur_mem_space == hardware.GetHead(nHardware::HEAD_WRITE, cur_view_thread).GetMemSpace()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 32, "W");
      }
      if (adj_inst_ptr == hardware.GetHead(nHardware::HEAD_FLOW, cur_view_thread).GetPosition() &&
          cur_mem_space == hardware.GetHead(nHardware::HEAD_FLOW, cur_view_thread).GetMemSpace()) {
        Print(MEMORY_Y + MEMORY_PRE_SIZE + 3 + i, MEMORY_X + 33, "F");
      }
    }
    SetColor(COLOR_WHITE);
  }
  
  DrawMiniMap();
}

void cZoomScreen::UpdateGenotype(cAvidaContext& ctx)
{
  SetBoldColor(COLOR_CYAN);
  
  Print(1, 12, "%9d", info.GetActiveGenotypeID());
  Print(2, 12, "%9s", static_cast<const char*>(info.GetActiveName()));
  
  if (info.GetActiveGenotype() != NULL) {
    cBioGroup* genotype = info.GetActiveGenotype();
    cGenomeTestMetrics* metrics = cGenomeTestMetrics::GetMetrics(ctx, genotype);
    Print(5, 12, "%9d", genotype->GetNumUnits());
    Print(6, 12, "%9d", cGenome(genotype->GetProperty("genome").AsString()).GetSize());
    PrintDouble(7, 14, metrics->GetLinesCopied());
    PrintDouble(8, 14, metrics->GetLinesExecuted());
    
    PrintDouble(10, 14, metrics->GetFitness());
    PrintDouble(11, 14, metrics->GetGestationTime());
    PrintDouble(12, 14, metrics->GetMerit());
    PrintDouble(13, 14, genotype->GetProperty("repro_rate").AsDouble());
    
    // Column 2
    Print(1, 40, "%9d", genotype->GetProperty("update_born").AsInt());
    Print(2, 40, "%9s", (const char*)(genotype->GetProperty("parents").AsString()));
    Print(3, 40, "%9d", genotype->GetDepth());
    
    Print(7, 40,  "%9d", genotype->GetProperty("recent_deaths").AsInt());
    Print(8, 40,  "%9d", genotype->GetProperty("recent_births").AsInt());
    Print(9, 40,  "%9d", genotype->GetProperty("recent_breed_true").AsInt());
    Print(10, 40, "%9d", genotype->GetProperty("recent_breed_in").AsInt());
    Print(11, 40, "%9d", genotype->GetProperty("recent_births").AsInt() - genotype->GetProperty("recent_breed_true").AsInt());
    
    Print(14, 40, "%9d", genotype->GetProperty("total_organisms").AsInt());
    Print(15, 40, "%9d", genotype->GetProperty("last_births").AsInt());
    Print(16, 40, "%9d", genotype->GetProperty("last_breed_true").AsInt());
    Print(17, 40, "%9d", genotype->GetProperty("last_breed_in").AsInt());
    Print(18, 40, "%9d", genotype->GetProperty("last_births").AsInt() - genotype->GetProperty("last_breed_true").AsInt());
  }
  else {
    Print(5, 12, "  -------");
    Print(6, 12, "  -------");
    Print(7, 12, "  -------");
    Print(8, 12, "  -------");
    
    Print(10, 12, "  -------");
    Print(11, 12, "  -------");
    Print(12, 12, "  -------");
    Print(13, 12, "  -------");
    
    // Column 2
    Print(1, 40, "  -------");
    Print(2, 40, "  -------");
    Print(3, 40, "  -------");
    Print(4, 40, "  -------");
    
    Print(7, 40,  "  -------");
    Print(8, 40,  "  -------");
    Print(9, 40,  "  -------");
    Print(10, 40, "  -------");
    Print(11, 40, "  -------");
    
    Print(14, 40, "  -------");
    Print(15, 40, "  -------");
    Print(16, 40, "  -------");
    Print(17, 40, "  -------");
    Print(18, 40, "  -------");
  }
}

void cZoomScreen::EditMemory(cAvidaContext& ctx)
{
  // Collect all of the needed variables.
  cHardwareBase& hardware = info.GetActiveCell()->GetOrganism()->GetHardware();
  const cInstSet& inst_set = hardware.GetInstSet();
  cHeadCPU edit_head(hardware.IP());
  if (parasite_zoom == true) edit_head.Set(0);
  edit_head.LoopJump(memory_offset);
  
  // Assemble first choice window.
  cMenuWindow menu1(NUM_INST_EDITS);
  menu1.SetTitle("Choose Edit method:");
  menu1.AddOption(INST_EDIT_BREAKPOINT, "Toggle [B]reakpoint");
  menu1.AddOption(INST_EDIT_JUMP_IP,    "[J]ump IP");
  menu1.AddOption(INST_EDIT_CHANGE,     "[E]dit Instruction");
  menu1.AddOption(INST_EDIT_INSERT,     "[I]nsert Instruction");
  menu1.AddOption(INST_EDIT_REMOVE,     "[D]elete Instruction");
  menu1.SetActive(INST_EDIT_CHANGE);
  int edit_method = menu1.Activate(this);
  info.GetView().Redraw();
  
  // If we need to choose a new instruction, bring up a window for it.
  
  int new_inst = 0;
  if (edit_method == INST_EDIT_CHANGE || edit_method == INST_EDIT_INSERT) {
    cMenuWindow inst_menu(inst_set.GetSize());
    inst_menu.SetTitle("Choose new instruction: ");
    for (int j = 0; j < inst_set.GetSize(); j++) {
      inst_menu.AddOption(j, static_cast<const char*>(inst_set.GetName(j)));
    }
    inst_menu.SetActive(edit_head.GetInst().GetOp());
    new_inst = inst_menu.Activate(this);
    
    info.GetView().Redraw();
    if (new_inst == -1) {
      //  info.GetView().Notify("Aborted!");
      return;
    }
  }
  
  // Finally, act on the edit method!
  switch (edit_method) {
    case INST_EDIT_BREAKPOINT:
      if (edit_head.GetMemory().FlagBreakpoint(edit_head.GetPosition()))
        edit_head.SetFlagBreakpoint();
      else
        edit_head.ClearFlagBreakpoint();
      break;
    case INST_EDIT_JUMP_IP:
      hardware.IP() = edit_head;
      memory_offset = 0;
      break;
    case INST_EDIT_CHANGE:
      edit_head.SetInst(cInstruction(new_inst));
      break;
    case INST_EDIT_INSERT:
      edit_head.InsertInst(cInstruction(new_inst));
      break;
    case INST_EDIT_REMOVE:
      edit_head.RemoveInst();
      break;
    default:
      //    info.GetView().Notify("Aborted!");
      break;
  }
  
  Update(ctx);
}

void cZoomScreen::ThreadOptions(cAvidaContext& ctx)
{
  int thread_method = THREAD_OPTIONS_VIEW;
  
  // Assemble first choice window.
  cMenuWindow menu1(NUM_THREAD_OPTIONS);
  menu1.SetTitle("Choose threading option:");
  menu1.AddOption(THREAD_OPTIONS_VIEW, "[V]iew Thread Info");
  menu1.AddOption(THREAD_OPTIONS_LOCK, "[T]oggle Thread Lock");
  menu1.SetActive(THREAD_OPTIONS_VIEW);
  thread_method = menu1.Activate(this);
  info.GetView().Redraw();
  
  // Act on the view method!
  switch (thread_method) {
    case THREAD_OPTIONS_VIEW:
      ViewThreads();
      break;
    case THREAD_OPTIONS_LOCK:
      if (info.GetThreadLock() != -1) info.SetThreadLock(-1);
      else {
        cHardwareBase& hardware = info.GetActiveCell()->GetOrganism()->GetHardware();
        info.SetThreadLock( hardware.GetCurThread() );
      }
      
      break;
    default:
      break;
  }
  
  Update(ctx);
}

void cZoomScreen::ViewInstruction()
{
  cTextWindow* window = new cTextWindow(10, 40, 7, 20);
  window->SetBoldColor(COLOR_WHITE);
  window->Box();
  
  // Place the data.
  
  cHardwareBase& hardware = info.GetActiveCell()->GetOrganism()->GetHardware();
  cHeadCPU inst_ptr(hardware.IP());
  if (parasite_zoom == true) inst_ptr.Set(0);
  inst_ptr.LoopJump(memory_offset);
  
  const cInstSet& inst_set = hardware.GetInstSet();
  
  window->SetBoldColor(COLOR_YELLOW);
  window->Print(2, 16, "%s", static_cast<const char*>(inst_set.GetName(inst_ptr.GetInst())));
  
  window->SetBoldColor(COLOR_WHITE);
  
  window->Print(2, 2, "Instruction: ");
  window->Print(4, 2, "Line Num...: ");
  window->Print(6, 2, "Redundancy.: ");
  window->Print(7, 2, "Cost.......: ");
  
  window->SetBoldColor(COLOR_CYAN);
  
  const int inst_id = inst_ptr.GetInst().GetOp();
  
  window->Print(4, 14, "%3d", inst_ptr.GetPosition());
  window->Print(6, 14, "%3d", inst_set.GetRedundancy(cInstruction(inst_id)) );
  window->Print(7, 14, "%3d", inst_set.GetCost(cInstruction(inst_id)) );
  
  if (inst_ptr.GetMemory().FlagCopied(inst_ptr.GetPosition())) window->SetBoldColor(COLOR_CYAN);
  else window->SetColor(COLOR_CYAN);
  window->Print(4, 25, "Copied");
  
  if (inst_ptr.GetMemory().FlagMutated(inst_ptr.GetPosition())) window->SetBoldColor(COLOR_CYAN);
  else window->SetColor(COLOR_CYAN);
  window->Print(5, 25, "Mutated");
  
  if (inst_ptr.GetMemory().FlagExecuted(inst_ptr.GetPosition())) window->SetBoldColor(COLOR_CYAN);
  else window->SetColor(COLOR_CYAN);
  window->Print(6, 25, "Executed");
  
  if (inst_ptr.GetMemory().FlagBreakpoint(inst_ptr.GetPosition())) window->SetBoldColor(COLOR_CYAN);
  else window->SetColor(COLOR_CYAN);
  window->Print(7, 25, "Breakpoint");
  
  
  // Print it!
  window->Refresh();
  
  // Wait for the results.
  bool finished = false;
  int cur_char;
  
  while (finished == false) {
    cur_char = GetInput();
    switch (cur_char) {
      case 'q':
      case 'Q':
      case ' ':
      case '\n':
      case '\r':
        finished = true;
        break;
    }
  }
  
  delete window;
  info.GetView().Redraw();
}

void cZoomScreen::ViewRegisters()
{
  cTextWindow* window = new cTextWindow(9, 54, 4, 13);
  cHardwareBase& hardware = info.GetActiveCell()->GetOrganism()->GetHardware();
  
  window->SetBoldColor(COLOR_WHITE);
  window->Box();
  
  window->Print(2, 2, "Registers");
  
  for (int i = 0; i < 3; i++) {
    const char reg_letter = 'A' + i;
    const int reg_value = hardware.GetRegister(i);
    window->SetBoldColor(COLOR_WHITE);
    window->Print(4+i, 2, "%cX:", reg_letter);
    window->Print(4+i, 17, '[');
    window->Print(4+i, 50, ']');
    window->SetBoldColor(COLOR_CYAN);
    window->Print(4+i, 6, "%10d", reg_value);
    window->PrintBinary(4+i, 18, reg_value);
  }
  
  // Print it!
  window->Refresh();
  
  // Wait for the results.
  bool finished = false;
  while (!finished) {
    int cur_char = GetInput();
    switch (cur_char) {
      case 'q':
      case 'Q':
      case ' ':
      case '\n':
      case '\r':
        finished = true;
        break;
    }
  }
  
  delete window;
  info.GetView().Redraw();
}


void cZoomScreen::ViewStack()
{
  cTextWindow* window = new cTextWindow(16, 50, 4, 15);
  
  // @DMB - Warning: This is making an unsafe assumption of hardware type.
  //                 While this option is not listed on the ZoomScreen for other hardware types,
  //                 it nonetheless is still possible to press the key to trigger it
#ifdef DEBUG
  cHardwareCPU* p_hw = dynamic_cast<cHardwareCPU*>(&info.GetActiveCell()->GetOrganism()->GetHardware());
  assert(p_hw);
  cHardwareCPU& hardware = *p_hw;
#else
  cHardwareCPU& hardware = static_cast<cHardwareCPU&>(info.GetActiveCell()->GetOrganism()->GetHardware());
#endif
  
  
  bool finished = false;
  int active_stack = hardware.GetActiveStack();
  
  
  while (finished == false) {
    window->SetBoldColor(COLOR_WHITE);
    window->Box();
    
    char stack_letter = 'A' + active_stack;
    window->Print(2, 5, "[<] Stack %c [>]", stack_letter);
    window->SetBoldColor(COLOR_CYAN);
    window->Print(2, 6, '<');
    window->Print(2, 18, '>');
    
    window->SetBoldColor(COLOR_WHITE);
    for (int i = 0; i < 10; i++) {
      const int stack_value = hardware.GetStack(i,active_stack);
      window->Print(4+i, 2, "%10d : ", stack_value);
      window->PrintBinary(4+i, 15, stack_value);
    }
    
    // Print it!
    window->Refresh();
    
    // Wait for the results.
    bool legal_keypress = false;
    while (!legal_keypress) {
      int cur_char = GetInput();
      switch (cur_char) {
        case ',':
        case '<':
        case '.':
        case '>':
          active_stack = 1 - active_stack;
          legal_keypress = true;
          break;
        case 'q':
        case 'Q':
        case ' ':
        case '\n':
        case '\r':
          finished = true;
          legal_keypress = true;
          break;
      }
    }
    
  }
  
  delete window;
  info.GetView().Redraw();
}


void cZoomScreen::ViewInputs()
{
  cTextWindow* window = new cTextWindow(9, 54, 4, 13);
  
  window->SetBoldColor(COLOR_WHITE);
  window->Box();
  
  window->Print(2, 2, "Inputs");
  
  for (int i = 0; i < 3; i++) {
    const int input_value = info.GetActiveCell()->GetInput(i);
    window->SetBoldColor(COLOR_WHITE);
    window->Print(4+i, 2, "%d:", i);
    window->Print(4+i, 17, '[');
    window->Print(4+i, 50, ']');
    window->SetBoldColor(COLOR_CYAN);
    window->Print(4+i, 6, "%10d", input_value);
    window->PrintBinary(4+i, 18, input_value);
  }
  
  // Print it!
  window->Refresh();
  
  // Wait for the results.
  bool finished = false;
  while (finished == false) {
    int cur_char = GetInput();
    switch (cur_char) {
      case 'q':
      case 'Q':
      case ' ':
      case '\n':
      case '\r':
        finished = true;
        break;
    }
  }
  
  delete window;
  info.GetView().Redraw();
}


void cZoomScreen::ViewThreads()
{
}

cCoords cZoomScreen::GetSectionCoords(int in_section)
{
  switch (in_section) {
    case ZOOM_SECTION_MEMORY:
      return cCoords(MEMORY_X, MEMORY_Y);
      break;
      
    case ZOOM_SECTION_REGISTERS:
      if(info.GetConfig().HARDWARE_TYPE.Get()==HARDWARE_TYPE_CPU_ORIGINAL)
        return cCoords(REG_X, REG_Y);
      else
        return cCoords(REG_X, REG_Y-1);
      break;
      
    case ZOOM_SECTION_STACK:
      if(info.GetConfig().HARDWARE_TYPE.Get()==HARDWARE_TYPE_CPU_ORIGINAL)
        return cCoords(STACK_X, STACK_Y);
      else
        return cCoords(STACK_X, STACK_Y-1);
      break;
      
    case ZOOM_SECTION_INPUTS:
      if(info.GetConfig().HARDWARE_TYPE.Get()==HARDWARE_TYPE_CPU_ORIGINAL)
        return cCoords(INPUT_X, INPUT_Y);
      else
        return cCoords(INPUT_X, INPUT_Y-1);
      break;
      
    case ZOOM_SECTION_MAP:
      return cCoords(MINI_MAP_X, MINI_MAP_Y);
      break;
  }
  
  return cCoords(0,0);
}


const char* cZoomScreen::GetSectionName(int in_section)
{
  switch (in_section) {
    case ZOOM_SECTION_MEMORY:
      if(info.GetConfig().HARDWARE_TYPE.Get()==HARDWARE_TYPE_CPU_ORIGINAL)
        return "Memory";
      else
        return "Memory Space";
      break;
      
    case ZOOM_SECTION_REGISTERS:
      if(info.GetConfig().HARDWARE_TYPE.Get()==HARDWARE_TYPE_CPU_ORIGINAL)
        return "Registers:";
      else
        return "Stacks:";
      break;
      
    case ZOOM_SECTION_STACK:
      return "Stack ";
      break;
      
    case ZOOM_SECTION_INPUTS:
      return "Inputs:";
      break;
      
    case ZOOM_SECTION_MAP:
      return "Mini-Map";
      break;
  }
  
  return "Unknown!";
}

void cZoomScreen::SetActiveSection(int in_section)
{
  if (in_section != active_section) {
    // Set the old section to be normal white
    cCoords sect_coords(GetSectionCoords(active_section));
    sect_coords.Translate(2, 1);
    SetColor(COLOR_WHITE);
    Print(sect_coords.GetY(), sect_coords.GetX(), "%s",
          GetSectionName(active_section));
    active_section = in_section;
  }
  
  cCoords sect_coords(GetSectionCoords(active_section));
  sect_coords.Translate(2, 1);
  SetBoldColor(COLOR_BLUE);
  Print(sect_coords.GetY(), sect_coords.GetX(), "%s",
        GetSectionName(active_section));
  SetColor(COLOR_WHITE);
}

void cZoomScreen::DoInput(cAvidaContext& ctx, int in_char)
{
  cHardwareBase* hardware = NULL;
  if (info.GetActiveCell()->IsOccupied()) {
    hardware = &(info.GetActiveCell()->GetOrganism()->GetHardware());
  }
  
  // First do the Mode specific io...
  
  if (mode == ZOOM_MODE_CPU      && DoInputCPU(ctx, in_char)) return;
  if (mode == ZOOM_MODE_STATS    && DoInputStats(ctx, in_char)) return;
  if (mode == ZOOM_MODE_GENOTYPE && DoInputGenotype(in_char)) return;
  
  int num_threads = 0;
  if (hardware != NULL)  num_threads = hardware->GetNumThreads();
  
  switch(in_char) {
    case 't':
    case 'T':
      if (num_threads > 1) {
        memory_offset=0;
        ++cur_view_thread%=num_threads;
        if(info.GetConfig().HARDWARE_TYPE.Get() != HARDWARE_TYPE_CPU_ORIGINAL){
          cur_mem_space = hardware->IP(cur_view_thread).GetMemSpace();
        }
        Update(ctx);
      }
      break;
    case ' ':
      memory_offset = 0;
      parasite_zoom = false;
      info.GetActiveCell()->GetOrganism()->GetPhenotype().SetFault("");
      info.EngageStepMode();
      nodelay(stdscr, true); // Don't delay for input; get to processing.
      break;
    case '>':
    case '.':
      mode++;
      if (mode == NUM_ZOOM_MODES) mode = 0;
      Clear();
      Draw(ctx);
      break;
    case '<':
    case ',':
      mode--;
      if (mode == -1) mode = NUM_ZOOM_MODES - 1;
      
      Clear();
      Draw(ctx);
      break;
      
    default:
      cerr << "Unknown Command (#"
      << (int) in_char << "): [" << in_char << "]" << endl;
      break;
  }
}


bool cZoomScreen::DoInputCPU(cAvidaContext& ctx, int in_char)
{
  switch(in_char) {
    case '2':
    case KEY_DOWN:
      if (active_section == ZOOM_SECTION_MAP) {
        mini_center_id += map_x_size;
        mini_center_id %= population.GetSize();
        cur_view_thread=0;
        if (population.GetCell(mini_center_id).IsOccupied()) {
          memory_offset = 0;
          info.SetActiveCell(&(population.GetCell(mini_center_id)));
        }
        Update(ctx);
      } else if (active_section == ZOOM_SECTION_MEMORY) {
        memory_offset++;
        Update(ctx);
      }
      break;
    case '8':
    case KEY_UP:
      if (active_section == ZOOM_SECTION_MAP) {
        mini_center_id -= map_x_size;
        cur_view_thread=0;
        if (mini_center_id < 0) mini_center_id += population.GetSize();
        if (population.GetCell(mini_center_id).IsOccupied()) {
          memory_offset = 0;
          info.SetActiveCell(&(population.GetCell(mini_center_id)));
        }
        Update(ctx);
      } else if (active_section == ZOOM_SECTION_MEMORY) {
        memory_offset--;
        Update(ctx);
      }
      break;
    case '6':
    case KEY_RIGHT:
      if (active_section == ZOOM_SECTION_MAP) {
        mini_center_id++;
        if (mini_center_id == population.GetSize()) mini_center_id = 0;
        if (population.GetCell(mini_center_id).IsOccupied()) {
          memory_offset = 0;
          info.SetActiveCell(&(population.GetCell(mini_center_id)));
        }
      }
      else if (active_section == ZOOM_SECTION_MEMORY) {
        if(info.GetConfig().HARDWARE_TYPE.Get() != HARDWARE_TYPE_CPU_ORIGINAL){
          cur_mem_space++;
          // @DMB - Should handle the extensibility of SMT Memory Spaces
          cur_mem_space %= 1;
        }
      }
      Update(ctx); 
      break;
    case '4':
    case KEY_LEFT:
      if (active_section == ZOOM_SECTION_MAP) {
        mini_center_id--;
        if (mini_center_id < 0) mini_center_id += population.GetSize();
        if (population.GetCell(mini_center_id).IsOccupied()) {
          memory_offset = 0;
          info.SetActiveCell(&(population.GetCell(mini_center_id)));
        }
      }
      else if (active_section == ZOOM_SECTION_MEMORY) {
        if(info.GetConfig().HARDWARE_TYPE.Get() != HARDWARE_TYPE_CPU_ORIGINAL){
          cur_mem_space--;
          // @DMB - Should handle the extensibility of SMT Memory Spaces
          if (cur_mem_space < 0) cur_mem_space = 0;
        }
      }
      Update(ctx);
      break;
    case 'K':
    case 'k':
      if(info.GetConfig().HARDWARE_TYPE.Get() != HARDWARE_TYPE_CPU_ORIGINAL) {
        ++cur_stack%=4;
      }
      Update(ctx);
      break;
    case '+':
    case '=':
      info.IncMapMode();
      Update(ctx);
      break;
    case '-':
    case '_':
      info.DecMapMode();
      Update(ctx);
      break;
    case '\n':
    case '\r':
      switch (active_section) {
        case ZOOM_SECTION_MEMORY:
          EditMemory(ctx);
          break;
        case ZOOM_SECTION_MAP:
          memory_offset = 0;
          info.SetActiveCell(&(population.GetCell(mini_center_id)));
          break;
        case ZOOM_SECTION_REGISTERS:
          ViewRegisters();
          break;
        case ZOOM_SECTION_STACK:
          ViewStack();
          break;
        case ZOOM_SECTION_INPUTS:
          ViewInputs();
          break;
      }
      
      Update(ctx);
      break;
    case '\t':
    {
      int next_section = active_section + 1;
      if (next_section == NUM_ZOOM_SECTIONS) next_section = 0;
      SetActiveSection(next_section);
    }
      Refresh();
      break;
      
      
    default:
      return false;
  };
  
  return true;
}

bool cZoomScreen::DoInputStats(cAvidaContext& ctx, int in_char)
{
  switch(in_char) {
    case '6':
    case KEY_RIGHT:
    {
      const int new_task_offset = task_offset + Height() - TASK_Y - 2;
      if (new_task_offset < info.GetWorld().GetEnvironment().GetNumTasks()) {
        task_offset = new_task_offset;
        Draw(ctx);
      }
    }
      break;
    case '4':
    case KEY_LEFT:
    {
      const int new_task_offset = task_offset - Height() + TASK_Y + 2;
      if (new_task_offset >= 0) {
        task_offset = new_task_offset;
        Draw(ctx);
      }
    }
      break;
      
    default:
      return false;
  }
  
  return true;
}

bool cZoomScreen::DoInputGenotype(int in_char)
{
  return false;
}

void cZoomScreen::DrawMiniMap()
{
  // if (GetSubWindow(0) == NULL) AddSubWindow(map_screen, 0);
  // map_screen->Update();
  
  // Setup the start color for the map...
  SetColor(COLOR_WHITE);
  mini_center_id = info.GetActiveCell()->GetID();
  
  const int name_x = MINI_MAP_X + 4;
  const int name_y = MINI_MAP_Y + 11;
  if (info.GetMapMode() == MAP_BASIC)           Print(name_y, name_x, "Genotypes");
  else if (info.GetMapMode() == MAP_COMBO)      Print(name_y, name_x, "  Combo  ");
  else if (info.GetMapMode() == MAP_INJECT)     Print(name_y, name_x, "Modified ");
  else if (info.GetMapMode() == MAP_RESOURCE)   Print(name_y, name_x, "Resources");
  else if (info.GetMapMode() == MAP_AGE)        Print(name_y, name_x, "   Age   ");
  else if (info.GetMapMode() == MAP_BREED_TRUE) Print(name_y, name_x, "BreedTrue");
  else if (info.GetMapMode() == MAP_PARASITE)   Print(name_y, name_x, "Parasites");
  else if (info.GetMapMode() == MAP_MUTATIONS)  Print(name_y, name_x, "Mutations");
  else if (info.GetMapMode() == MAP_THREAD)     Print(name_y, name_x, " Threads ");
  else if (info.GetMapMode() == MAP_LINEAGE)    Print(name_y, name_x, " Lineage ");
  
  
  int virtual_x = (mini_center_id % map_x_size) + map_x_size;
  int virtual_y = (mini_center_id / map_x_size) + map_y_size;
  
  info.SetupSymbolMaps(info.GetMapMode(), HasColors());
  
  for (int y = -3; y <= 3 && y < map_y_size - 3; y++) {
    Move(MINI_MAP_Y + 6 + y, MINI_MAP_X + 2);
    int cur_y = (y + virtual_y) % map_y_size;
    for (int x = -3; x <= 3 && x < map_x_size - 3; x++) {
      int cur_x = (x + virtual_x) % map_x_size;
      int index = cur_y * map_x_size + cur_x;
      
      SetSymbolColor(info.ColorSymbol(index));
      if (info.MapSymbol(index) > 0) Print(info.MapSymbol(index));
      else Print(CHAR_BULLET);
      
      // Space between columns
      for (int i = 0; i < AVIDA_MAP_X_SPACING - 1; i++)  Print(' ');
    }
  }
  SetColor(COLOR_WHITE);
  Print(MINI_MAP_Y + 6, MINI_MAP_X + 7, '[');
  Print(MINI_MAP_Y + 6, MINI_MAP_X + 9, ']');
  
  Refresh();
}

