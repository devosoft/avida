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
  // Population events --> Population Actions
  REGISTER(join_grid_col);
  REGISTER(join_grid_row);
  REGISTER(connect_cells);
  REGISTER(disconnect_cells);
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
