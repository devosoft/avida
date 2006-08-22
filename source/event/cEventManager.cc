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
