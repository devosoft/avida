/*
 *  cEventManager.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cEventManager.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyzeUtil.h"
#include "cAvidaContext.h"
#include "avida.h"
#include "cClassificationManager.h"
#include "cEnvironment.h"
#include "cEvent.h"
#include "cGenotype.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstUtil.h"
#include "cLandscape.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cResource.h"
#include "cStats.h"
#include "cStringUtil.h"
#include "cTestCPU.h"
#include "cTestUtil.h"
#include "cTools.h"
#include "cWorld.h"
#include "cWorldDriver.h"

#include <ctype.h>           // for isdigit
#include <iostream>

using namespace std;


///// zero_muts /////

/**
* This event will set all mutation rates to zero...
 **/


class cEvent_zero_muts : public cEvent {
private:
public:
  const cString GetName() const { return "zero_muts"; }
  const cString GetDescription() const { return "zero_muts"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// zero_muts /////
  void Process(){
    for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
      m_world->GetPopulation().GetCell(i).MutationRates().Clear();
    }
  }
};

///// mod_copy_mut /////

/**
**/


class cEvent_mod_copy_mut : public cEvent {
private:
  double cmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_copy_mut"; }
  const cString GetDescription() const { return "mod_copy_mut  <double cmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_copy_mut /////
  void Process(){
    const double new_cmut = m_world->GetConfig().COPY_MUT_PROB.Get() + cmut_inc;
    if (cell < 0) {   // cell == -1  -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetCopyMutProb(new_cmut);
      }
      m_world->GetConfig().COPY_MUT_PROB.Set(new_cmut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetCopyMutProb(new_cmut);
    }
  }
};

///// mod_div_mut /////

/**
**/


class cEvent_mod_div_mut : public cEvent {
private:
  double dmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_div_mut"; }
  const cString GetDescription() const { return "mod_div_mut  <double dmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    dmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_div_mut /////
  void Process(){
    const double new_div_mut = m_world->GetConfig().DIV_MUT_PROB.Get() + dmut_inc;
    if (cell < 0) {   // cell == -1  -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetDivMutProb(new_div_mut);
      }
      m_world->GetConfig().DIV_MUT_PROB.Set(new_div_mut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetDivMutProb(new_div_mut);
    }
  }
};


///// mod_point_mut /////

/**
**/


class cEvent_mod_point_mut : public cEvent {
private:
  double pmut_inc;
  int cell;
public:
    const cString GetName() const { return "mod_point_mut"; }
  const cString GetDescription() const { return "mod_point_mut  <double pmut_inc> [int cell=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    pmut_inc = args.PopWord().AsDouble();
    if (args == "") cell=-1; else cell=args.PopWord().AsInt();
  }
  ///// mod_point_mut /////
  void Process(){
    const double new_pmut = m_world->GetConfig().POINT_MUT_PROB.Get() + pmut_inc;
    if (cell < 0) {   // cell == -1   -->  all
      for (int i = 0; i < m_world->GetPopulation().GetSize(); i++) {
        m_world->GetPopulation().GetCell(i).MutationRates().SetPointMutProb(new_pmut);
      }
      m_world->GetConfig().POINT_MUT_PROB.Set(new_pmut);
    } else {
      m_world->GetPopulation().GetCell(cell).MutationRates().SetPointMutProb(new_pmut);
    }
  }
};

///// test_dom /////

/**
**/


class cEvent_test_dom : public cEvent {
private:
public:
  const cString GetName() const { return "test_dom"; }
  const cString GetDescription() const { return "test_dom"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args; }
  ///// test_dom /////
  void Process(){
    cGenome & genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
    cAnalyzeUtil::TestGenome(m_world, genome, m_world->GetHardwareManager().GetInstSet(),
                             m_world->GetDataFileOFStream("dom-test.dat"), m_world->GetStats().GetUpdate());
  }
};


///// task_snapshot /////

/**
**/


class cEvent_task_snapshot : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "task_snapshot"; }
  const cString GetDescription() const { return "task_snapshot  [string filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename = "";
    else filename = args.PopWord();
  }
  ///// task_snapshot /////
  void Process(){
    if (filename == "") filename.Set("tasks_%d.dat",m_world->GetStats().GetUpdate());
    cAnalyzeUtil::TaskSnapshot(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_viable_tasks_data /////

/**
**/


class cEvent_print_viable_tasks_data : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_viable_tasks_data"; }
  const cString GetDescription() const { return "print_viable_tasks_data  [string filename=\"viable_tasks.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename="viable_tasks.dat"; else filename=args.PopWord();
  }
  ///// print_viable_tasks_data /////
  void Process(){
    cAnalyzeUtil::PrintViableTasksData(m_world, m_world->GetDataFileOFStream(filename));
  }
};


///// compete_demes /////

/**
* Compete all of the demes using a basic genetic algorithm approach. Fitness
 * of each deme is determined differently depending on the competition_type: 
 * 0: deme fitness = 1 (control, random deme selection)
 * 1: deme fitness = number of births since last competition (default) 
 * 2: deme fitness = average organism fitness at the current update
 * 3: deme fitness = average mutation rate at the current update
 * Merit can optionally be passed in.
 **/


class cEvent_compete_demes : public cEvent {
private:
  int competition_type;
public:
  const cString GetName() const { return "compete_demes"; }
  const cString GetDescription() const { return "compete_demes  [int competition_type=1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") competition_type=1; else competition_type=args.PopWord().AsInt();
  }
  ///// compete_demes /////
  void Process(){
    m_world->GetPopulation().CompeteDemes(competition_type);
  }
};

///// reset_demes /////

/**
* Designed to serve as a control for the compete_demes. Each deme is 
 * copied into itself and the parameters reset. 
 **/


class cEvent_reset_demes : public cEvent {
private:
public:
  const cString GetName() const { return "reset_demes"; }
  const cString GetDescription() const { return "reset_demes"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// reset_demes /////
  void Process(){
    m_world->GetPopulation().ResetDemes();
  }
};

///// print_deme_stats /////

/**
* Print stats about individual demes
 **/


class cEvent_print_deme_stats : public cEvent {
private:
public:
  const cString GetName() const { return "print_deme_stats"; }
  const cString GetDescription() const { return "print_deme_stats"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// print_deme_stats /////
  void Process(){
    m_world->GetPopulation().PrintDemeStats();
  }
};

///// copy_deme /////

/**
* Takes two numbers as arguments and copies the contents of the first deme
 * listed into the second.
 **/


class cEvent_copy_deme : public cEvent {
private:
  int deme1_id;
  int deme2_id;
public:
    const cString GetName() const { return "copy_deme"; }
  const cString GetDescription() const { return "copy_deme  <int deme1_id> <int deme2_id>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    deme1_id = args.PopWord().AsInt();
    deme2_id = args.PopWord().AsInt();
  }
  ///// copy_deme /////
  void Process(){
    m_world->GetPopulation().CopyDeme(deme1_id, deme2_id);
  }
};

///// calc_consensus /////

/**
* Calculates the consensus sequence.
 *
 * Parameters:
 * lines saved (integer) default: 0
   *    ???
   **/


class cEvent_calc_consensus : public cEvent {
private:
  int lines_saved;
public:
  const cString GetName() const { return "calc_consensus"; }
  const cString GetDescription() const { return "calc_consensus  [int lines_saved=0]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") lines_saved=0; else lines_saved=args.PopWord().AsInt();
  }
  ///// calc_consensus /////
  void Process(){
    cAnalyzeUtil::CalcConsensus(m_world, lines_saved);
  }
};

///// test_size_change_robustness /////

/**
**/


class cEvent_test_size_change_robustness : public cEvent {
private:
  int num_trials;
  cString filename;
public:
    const cString GetName() const { return "test_size_change_robustness"; }
  const cString GetDescription() const { return "test_size_change_robustness  [int num_trials=100] [string filename=\"size_change.dat\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") num_trials=100; else num_trials=args.PopWord().AsInt();
    if (args == "") filename="size_change.dat"; else filename=args.PopWord();
  }
  ///// test_size_change_robustness /////
  void Process(){
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    cAnalyzeUtil::TestInsSizeChangeRobustness(m_world, fp,
                                              m_world->GetHardwareManager().GetInstSet(),
                                              m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                                              num_trials, m_world->GetStats().GetUpdate());
  }
};

///// test_threads /////

/**
**/


class cEvent_test_threads : public cEvent {
private:
public:
  const cString GetName() const { return "test_threads"; }
  const cString GetDescription() const { return "test_threads"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  
  ///// test_threads /////
  void Process(){
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->TestThreads(ctx, m_world->GetClassificationManager().GetBestGenotype()->GetGenome());
    delete testcpu;
  }
};

///// print_threads /////

/**
**/


class cEvent_print_threads : public cEvent {
private:
public:
  const cString GetName() const { return "print_threads"; }
  const cString GetDescription() const { return "print_threads"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// print_threads /////
  void Process(){
    cAvidaContext& ctx = m_world->GetDefaultContext();

    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();
    testcpu->PrintThreads(ctx, m_world->GetClassificationManager().GetBestGenotype()->GetGenome() );
    delete testcpu;
  }
};

///// dump_fitness_grid /////

/**
* Writes out all fitness values of the organisms currently in the
 * population.
 *
 * The output file is called "fgrid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_fitness_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_fitness_grid"; }
  const cString GetDescription() const { return "dump_fitness_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_fitness_grid /////
  void Process(){
    cString filename;
    filename.Set("fgrid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        double fitness = (cell.IsOccupied()) ?
          cell.GetOrganism()->GetGenotype()->GetFitness() : 0.0;
        fp << fitness << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_genotype_grid /////

/**
* Writes out all genotype id values of the organisms currently in the
 * population.
 *
 * The output file is called "idgrid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_genotype_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_genotype_grid"; }
  const cString GetDescription() const { return "dump_genotype_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_genotype_grid /////
  void Process(){
    cString filename;
    filename.Set("idgrid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int id = (cell.IsOccupied()) ?
          cell.GetOrganism()->GetGenotype()->GetID() : -1;
        fp << id << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_task_grid /////

/**
* Writes out a grid of tasks done by each organism
 * Tasks are encoded as a binary string first, and then converted into a
 * base 10 number 
 **/


class cEvent_dump_task_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_task_grid"; }
  const cString GetDescription() const { return "dump_task_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_task_grid /////
  void Process(){
    cString filename;
    filename.Set("task_grid_%d.dat", m_world->GetStats().GetUpdate());
    cAnalyzeUtil::TaskGrid(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_donor_grid /////

/**
* Writes out the grid of donor organisms in the population
 * 
 * The output file is called "donor_grid.*.out", where '*' is replaced by the
 * number of the current update.
 **/   


class cEvent_dump_donor_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_donor_grid"; }
  const cString GetDescription() const { return "dump_donor_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  
  ///// dump_donor_grid /////
  void Process(){
    cString filename;
    filename.Set("donor_grid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {  
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int donor = cell.IsOccupied() ?  
          cell.GetOrganism()->GetPhenotype().IsDonorLast() : -1;    
        fp << donor << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// dump_receiver_grid /////

/**
* Writes out the grid of organisms which received merit in the population
 *
 * The output file is called "receiver_grid.*.out", where '*' is replaced by the
 * number of the current update.
 **/


class cEvent_dump_receiver_grid : public cEvent {
private:
public:
  const cString GetName() const { return "dump_receiver_grid"; }
  const cString GetDescription() const { return "dump_receiver_grid"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
  }
  ///// dump_receiver_grid /////
  void Process(){
    cString filename;
    filename.Set("receiver_grid.%05d.out", m_world->GetStats().GetUpdate());
    ofstream& fp = m_world->GetDataFileOFStream(filename);
    for (int i = 0; i < m_world->GetPopulation().GetWorldX(); i++) {
      for (int j = 0; j < m_world->GetPopulation().GetWorldY(); j++) {
        cPopulationCell & cell = m_world->GetPopulation().GetCell(j*m_world->GetPopulation().GetWorldX()+i);
        int receiver = cell.IsOccupied() ?
          cell.GetOrganism()->GetPhenotype().IsReceiver() : -1;
        fp << receiver << " ";
      }
      fp << endl;
    }
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// print_tree_depths /////

/**
* Reconstruction of phylogenetic trees.
 **/


class cEvent_print_tree_depths : public cEvent {
private:
  cString filename;
public:
  const cString GetName() const { return "print_tree_depths"; }
  const cString GetDescription() const { return "print_tree_depths  [string filename=\"\"]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") filename=""; else filename=args.PopWord();
  }
  ///// print_tree_depths /////
  void Process(){
    if (filename == "") filename.Set("tree_depth.%d.dat", m_world->GetStats().GetUpdate());
    cAnalyzeUtil::PrintTreeDepths(m_world, m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

///// sever_grid_col /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  col_id:  indicates the number of columns to the left of the cut.
 *           default (or -1) = cut population in half
 *  min_row: First row to start cutting from
 *           default = 0
 *  max_row: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_sever_grid_col : public cEvent {
private:
  int col_id;
  int min_row;
  int max_row;
public:
    const cString GetName() const { return "sever_grid_col"; }
  const cString GetDescription() const { return "sever_grid_col  [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") col_id=-1; else col_id=args.PopWord().AsInt();
    if (args == "") min_row=0; else min_row=args.PopWord().AsInt();
    if (args == "") max_row=-1; else max_row=args.PopWord().AsInt();
  }
  ///// sever_grid_col /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (col_id == -1) col_id = world_x / 2;
    if (max_row == -1) max_row = world_y;
    if (col_id < 0 || col_id >= world_x) {
      cerr << "Event Error: Column ID " << col_id
      << " out of range for sever_grid_col" << endl;
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = min_row; row_id < max_row; row_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA0 = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA1 = GridNeighbor(idA, world_x, world_y,  0,  1);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y, -1,  1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB0));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB1));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA0));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA1));
    }
  }
};

///// sever_grid_row /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  row_id:  indicates the number of rows above the cut.
 *           default (or -1) = cut population in half
 *  min_col: First row to start cutting from
 *           default = 0
 *  max_col: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_sever_grid_row : public cEvent {
private:
  int row_id;
  int min_col;
  int max_col;
public:
    const cString GetName() const { return "sever_grid_row"; }
  const cString GetDescription() const { return "sever_grid_row  [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") row_id=-1; else row_id=args.PopWord().AsInt();
    if (args == "") min_col=0; else min_col=args.PopWord().AsInt();
    if (args == "") max_col=-1; else max_col=args.PopWord().AsInt();
  }
  ///// sever_grid_row /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (row_id == -1) row_id = world_y / 2;
    if (max_col == -1) max_col = world_x;
    if (row_id < 0 || row_id >= world_y) {
      cerr << "Event Error: Row ID " << row_id
      << " out of range for sever_grid_row" << endl;
      return;
    }
    // Loop through all of the cols and make the cut on each...
    for (int col_id = min_col; col_id < max_col; col_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);
      int idA0 = GridNeighbor(idA, world_x, world_y, -1,  0);
      int idA1 = GridNeighbor(idA, world_x, world_y,  1,  0);
      int idB0 = GridNeighbor(idA, world_x, world_y, -1, -1);
      int idB1 = GridNeighbor(idA, world_x, world_y,  1, -1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB0));
      cellA_list.Remove(&m_world->GetPopulation().GetCell(idB1));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA0));
      cellB_list.Remove(&m_world->GetPopulation().GetCell(idA1));
    }
  }
};

///// join_grid_col /////

/**
* Join the connections between cells along a column in an avida grid.
 * Arguments:
 *  col_id:  indicates the number of columns to the left of the joining.
 *           default (or -1) = join population halves.
 *  min_row: First row to start joining from
 *           default = 0
 *  max_row: Last row to join to
 *           default (or -1) = last row in population.
 **/


class cEvent_join_grid_col : public cEvent {
private:
  int col_id;
  int min_row;
  int max_row;
public:
    const cString GetName() const { return "join_grid_col"; }
  const cString GetDescription() const { return "join_grid_col  [int col_id=-1] [int min_row=0] [int max_row=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") col_id=-1; else col_id=args.PopWord().AsInt();
    if (args == "") min_row=0; else min_row=args.PopWord().AsInt();
    if (args == "") max_row=-1; else max_row=args.PopWord().AsInt();
  }
  ///// join_grid_col /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (col_id == -1) col_id = world_x / 2;
    if (max_row == -1) max_row = world_y;
    if (col_id < 0 || col_id >= world_x) {
      cerr << "Event Error: Column ID " << col_id
      << " out of range for join_grid_col" << endl;
      return;
    }
    // Loop through all of the rows and make the cut on each...
    for (int row_id = min_row; row_id < max_row; row_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y, -1,  0);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell & cellA0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0, -1));
      cPopulationCell & cellA1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  0,  1));
      cPopulationCell & cellB0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell & cellB1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  1));
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};

///// join_grid_row /////

/**
* Remove the connections between cells along a column in an avida grid.
 * Arguments:
 *  row_id:  indicates the number of rows abovef the cut.
 *           default (or -1) = cut population in half
 *  min_col: First row to start cutting from
 *           default = 0
 *  max_col: Last row to cut to
 *           default (or -1) = last row in population.
 **/


class cEvent_join_grid_row : public cEvent {
private:
  int row_id;
  int min_col;
  int max_col;
public:
    const cString GetName() const { return "join_grid_row"; }
  const cString GetDescription() const { return "join_grid_row  [int row_id=-1] [int min_col=0] [int max_col=-1]"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    if (args == "") row_id=-1; else row_id=args.PopWord().AsInt();
    if (args == "") min_col=0; else min_col=args.PopWord().AsInt();
    if (args == "") max_col=-1; else max_col=args.PopWord().AsInt();
  }
  ///// join_grid_row /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (row_id == -1) row_id = world_y / 2;
    if (max_col == -1) max_col = world_x;
    if (row_id < 0 || row_id >= world_y) {
      cerr << "Event Error: Row ID " << row_id
      << " out of range for join_grid_row" << endl;
      return;
    }
    // Loop through all of the cols and make the cut on each...
    for (int col_id = min_col; col_id < max_col; col_id++) {
      int idA = row_id * world_x + col_id;
      int idB  = GridNeighbor(idA, world_x, world_y,  0, -1);
      cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
      cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
      cPopulationCell & cellA0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1,  0));
      cPopulationCell & cellA1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1,  0));
      cPopulationCell & cellB0 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y, -1, -1));
      cPopulationCell & cellB1 =
        m_world->GetPopulation().GetCell(GridNeighbor(idA, world_x, world_y,  1, -1));
      tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
      tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
      if (cellA_list.FindPtr(&cellB)  == NULL) cellA_list.Push(&cellB);
      if (cellA_list.FindPtr(&cellB0) == NULL) cellA_list.Push(&cellB0);
      if (cellA_list.FindPtr(&cellB1) == NULL) cellA_list.Push(&cellB1);
      if (cellB_list.FindPtr(&cellA)  == NULL) cellB_list.Push(&cellA);
      if (cellB_list.FindPtr(&cellA0) == NULL) cellB_list.Push(&cellA0);
      if (cellB_list.FindPtr(&cellA1) == NULL) cellB_list.Push(&cellA1);
    }
  }
};

///// connect_cells /////

/**
* Connects a pair of specified cells.
 * Arguments:
 *  cellA_x, cellA_y, cellB_x, cellB_y
 **/


class cEvent_connect_cells : public cEvent {
private:
  int cellA_x;
  int cellA_y;
  int cellB_x;
  int cellB_y;
public:
    const cString GetName() const { return "connect_cells"; }
  const cString GetDescription() const { return "connect_cells  <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cellA_x = args.PopWord().AsInt();
    cellA_y = args.PopWord().AsInt();
    cellB_x = args.PopWord().AsInt();
    cellB_y = args.PopWord().AsInt();
  }
  ///// connect_cells /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (cellA_x < 0 || cellA_x >= world_x ||
        cellA_y < 0 || cellA_y >= world_y ||
        cellB_x < 0 || cellB_x >= world_x ||
        cellB_y < 0 || cellB_y >= world_y) {
      cerr << "Event 'connect_cells' cell out of range." << endl;
      return;
    }
    int idA = cellA_y * world_x + cellA_x;
    int idB = cellB_y * world_x + cellB_x;
    cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
    tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
    cellA_list.PushRear(&cellB);
    cellB_list.PushRear(&cellA);
  }
};

///// disconnect_cells /////

/**
* Connects a pair of specified cells.
 * Arguments:
 *  cellA_x, cellA_y, cellB_x, cellB_y
 **/


class cEvent_disconnect_cells : public cEvent {
private:
  int cellA_x;
  int cellA_y;
  int cellB_x;
  int cellB_y;
public:
    const cString GetName() const { return "disconnect_cells"; }
  const cString GetDescription() const { return "disconnect_cells  <int cellA_x> <int cellA_y> <int cellB_x> <int cellB_y>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    cellA_x = args.PopWord().AsInt();
    cellA_y = args.PopWord().AsInt();
    cellB_x = args.PopWord().AsInt();
    cellB_y = args.PopWord().AsInt();
  }
  ///// disconnect_cells /////
  void Process(){
    const int world_x = m_world->GetPopulation().GetWorldX();
    const int world_y = m_world->GetPopulation().GetWorldY();
    if (cellA_x < 0 || cellA_x >= world_x ||
        cellA_y < 0 || cellA_y >= world_y ||
        cellB_x < 0 || cellB_x >= world_x ||
        cellB_y < 0 || cellB_y >= world_y) {
      cerr << "Event 'connect_cells' cell out of range." << endl;
      return;
    }
    int idA = cellA_y * world_x + cellA_x;
    int idB = cellB_y * world_x + cellB_x;
    cPopulationCell & cellA = m_world->GetPopulation().GetCell(idA);
    cPopulationCell & cellB = m_world->GetPopulation().GetCell(idB);
    tList<cPopulationCell> & cellA_list = cellA.ConnectionList();
    tList<cPopulationCell> & cellB_list = cellB.ConnectionList();
    cellA_list.Remove(&cellB);
    cellB_list.Remove(&cellA);
  }
};

///// inject_resource /////

/**
* Inject (add) a specified amount of a specified resource.
 **/


class cEvent_inject_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "inject_resource"; }
  const cString GetDescription() const { return "inject_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// inject_resource /////
  void Process(){
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    m_world->GetPopulation().UpdateResource(res_id, res_count);
  }
};

///// set_resource /////

/**
* Set the resource amount to a specific level
 **/


class cEvent_set_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "set_resource"; }
  const cString GetDescription() const { return "set_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// set_resource /////
  void Process(){
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    cResource * found_resource = res_lib.GetResource(res_name);
    if (found_resource != NULL) {
      m_world->GetPopulation().SetResource(found_resource->GetID(), res_count);
    }
  }
};

///// inject_scaled_resource /////

/**
* Inject (add) a specified amount of a specified resource, scaled by
 * the current average merit divided by the average time slice.
 **/


class cEvent_inject_scaled_resource : public cEvent {
private:
  cString res_name;
  double res_count;
public:
    const cString GetName() const { return "inject_scaled_resource"; }
  const cString GetDescription() const { return "inject_scaled_resource  <cString res_name> <double res_count>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_count = args.PopWord().AsDouble();
  }
  ///// inject_scaled_resource /////
  void Process(){
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if ( ave_merit <= 0 )
      ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    m_world->GetPopulation().UpdateResource(res_id, res_count/ave_merit);
  }
};


///// outflow_scaled_resource /////

/**
* Removes a specified percentage of a specified resource, scaled by
 * the current average merit divided by the average time slice.
 **/
class cEvent_outflow_scaled_resource : public cEvent {
private:
  cString res_name;
  double res_perc;
public:
    const cString GetName() const { return "outflow_scaled_resource"; }
  const cString GetDescription() const { return "outflow_scaled_resource  <cString res_name> <double res_perc>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    res_name = args.PopWord();
    res_perc = args.PopWord().AsDouble();
  }
  void Process()
  {
    double ave_merit = m_world->GetStats().SumMerit().Average();
    if ( ave_merit <= 0 )
      ave_merit = 1; // make sure that we don't get NAN's or negative numbers
    ave_merit /= m_world->GetConfig().AVE_TIME_SLICE.Get();
    cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
    int res_id = res_lib.GetResource(res_name)->GetID();
    double res_level = m_world->GetPopulation().GetResource(res_id);
    // a quick calculation shows that this formula guarantees that
    // the equilibrium level when resource is not used is independent
    // of the average merit
    double scaled_perc = 1/(1+ave_merit*(1-res_perc)/res_perc);
    res_level -= res_level*scaled_perc;
    m_world->GetPopulation().SetResource(res_id, res_level);
  }
};


///// set_reaction_value /////

/**
* Set the value associated with a reaction to a specific level
 **/
class cEvent_set_reaction_value : public cEvent {
private:
  cString reaction_name;
  double reaction_value;
public:
    const cString GetName() const { return "set_reaction_value"; }
  const cString GetDescription() const { return "set_reaction_value  <cString reaction_name> <double reaction_value>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    reaction_value = args.PopWord().AsDouble();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionValue(reaction_name, reaction_value);
  }
};


///// set_reaction_value_mult /////

/**
* Change the value of the reaction by multiplying it with the imput number
 **/
class cEvent_set_reaction_value_mult : public cEvent {
private:
  cString reaction_name;
  double value_mult;
public:
    const cString GetName() const { return "set_reaction_value_mult"; }
  const cString GetDescription() const { return "set_reaction_value_mult  <cString reaction_name> <double value_mult>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    value_mult = args.PopWord().AsDouble();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionValueMult(reaction_name, value_mult);
  }
};


///// set_reaction_inst /////

/**
* Change the instruction triggered by the task
 **/
class cEvent_set_reaction_inst : public cEvent {
private:
  cString reaction_name;
  cString inst_name;
public:
    const cString GetName() const { return "set_reaction_inst"; }
  const cString GetDescription() const { return "set_reaction_inst <cString reaction_name> <cString inst_name>"; }
  
  void Configure(cWorld* world, const cString& in_args)
  {
    m_world = world;
    m_args = in_args;
    cString args(in_args);
    reaction_name = args.PopWord();
    inst_name = args.PopWord();
  }
  void Process()
  {
    m_world->GetEnvironment().SetReactionInst(reaction_name, inst_name);
  }
};

class cEventAction : public cEvent
{
private:
  cAction* m_action;

public:
  cEventAction(cWorld* world, cAction* action, const cString& args)
    : m_action(action) { Configure(world, args); }
  ~cEventAction() { delete m_action; }
  
  const cString GetName() const { return "action wrapper"; }
  const cString GetDescription() const { return "action wrapper - description not available"; }
  
  void Configure(cWorld* world, const cString& in_args) { m_world = world; m_args = in_args; }
  void Process()
  {
    cAvidaContext& ctx = m_world->GetDefaultContext();
    m_action->Process(ctx);
  }
};



#define REGISTER(EVENT_NAME) Register<cEvent_ ## EVENT_NAME>(#EVENT_NAME)

cEventManager::cEventManager(cWorld* world) : m_world(world)
{
  REGISTER(zero_muts);
  REGISTER(mod_copy_mut);
  REGISTER(mod_div_mut);
  REGISTER(mod_point_mut);
  REGISTER(test_dom);

  REGISTER(task_snapshot);
  REGISTER(print_viable_tasks_data);
  
  REGISTER(compete_demes);
  REGISTER(reset_demes);
  REGISTER(print_deme_stats);
  REGISTER(copy_deme);
  
  REGISTER(calc_consensus);
  REGISTER(test_size_change_robustness);
  REGISTER(test_threads);
  REGISTER(print_threads);
  REGISTER(dump_fitness_grid);
  REGISTER(dump_genotype_grid);
  REGISTER(dump_task_grid);
  REGISTER(dump_donor_grid);
  REGISTER(dump_receiver_grid);
  REGISTER(print_tree_depths);
  REGISTER(sever_grid_col);
  REGISTER(sever_grid_row);
  REGISTER(join_grid_col);
  REGISTER(join_grid_row);
  REGISTER(connect_cells);
  REGISTER(disconnect_cells);
  REGISTER(inject_resource);
  REGISTER(set_resource);
  REGISTER(inject_scaled_resource);
  REGISTER(outflow_scaled_resource);
  REGISTER(set_reaction_value);
  REGISTER(set_reaction_value_mult);
  REGISTER(set_reaction_inst);
}

cEvent* cEventManager::ConstructEvent(const cString name, const cString& args)
{
  cEvent* event = Create(name);
  
  if (event != NULL) {
    event->Configure(m_world, args);
  } else {
    cAction* action = m_world->GetActionLibrary().Create(name, m_world, args);
    if (action != NULL) event = new cEventAction(m_world, action, args);
  }
  
  return event;
}

void cEventManager::PrintAllEventDescriptions()
{
  tArray<cEvent*> events;
  CreateAll(events);
  
  for (int i = 0; i < events.GetSize(); i++) {
    cout << events[i]->GetDescription() << endl;
    delete events[i];
  }
}
