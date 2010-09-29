//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cEnvironmentScreen.h"

#include "cEnvironment.h"
#include "cPopulation.h"
#include "cReaction.h"
#include "cReactionProcess.h"
#include "cResource.h"
#include "cStats.h"
#include "cPopulationCell.h"
#include "cOrganism.h"

using namespace std;

void cEnvironmentScreen::Draw(cAvidaContext& ctx)
{
  SetBoldColor(COLOR_WHITE);
  Print(1, 54, "[ ]               [ ]");
  if (info.GetPauseLevel()) {
    Print(3, 54, "Un-[ ]ause");
    Print(4, 54, "[ ]ext Update");
  } else {
    Print(3, 54, "[ ]ause   ");
    Print(4, 54, "             ");
  }
  
  SetBoldColor(COLOR_CYAN);
  Print(1, 55, "<");
  Print(1, 73, ">");
  Print(5, 54, "[        ]");
  if (info.GetPauseLevel()) {
    Print(3, 58, "P");
    Print(4, 55, "N");
  } else {
    Print(3, 55, "P");
  }
  Print(5, 55, "UP, DOWN");
  
  if(mode==ENVIRONMENT_MODE_RESOURCE)
    DrawResource();
  else
    DrawReaction();
  Update(ctx);
}

void cEnvironmentScreen::DrawResource()
{
  const cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
  
  SetBoldColor(COLOR_WHITE);
  
  Print(0, 0, " --Name-- ");
  Print(0, 12, "--Inflow--");
  Print(0, 24, "--Outflow--");
  Print(0, 37, "--Quantity--");
  
  Print(res_lib.GetSize()+3, 2, "Reactions associated with Resource ");
  Print(res_lib.GetSize()+4, 2, "--Name--  --Num Rxns (last update)--  --");
  
  Print(6, 54, "Next Resource");
  
  SetBoldColor(COLOR_CYAN);
  for(int i=0; i<res_lib.GetSize(); i++)
  {
    Print(i+1, 1, res_lib.GetResource(i)->GetName());
    Print(i+1, 12, "%7.2f", res_lib.GetResource(i)->GetInflow());
    Print(i+1, 24, "%7.2f", res_lib.GetResource(i)->GetOutflow());
  }
  if (info.GetPauseLevel()) {
    Print(3, 58, "P");
    Print(4, 55, "N");
  } else {
    Print(3, 55, "P");
  }
  Print(5, 55, "UP, DOWN");
  
  
  SetColor(COLOR_WHITE);
  Box(res_lib.GetSize()+2, 0, Height()-res_lib.GetSize()-2, Width(), true);
  
  SetBoldColor(COLOR_YELLOW);
  Print(1, 58, "Resource View");
}

void cEnvironmentScreen::DrawReaction()
{
  const cReactionLib & rxn_lib = m_world->GetEnvironment().GetReactionLib();
  
  SetBoldColor(COLOR_WHITE);
  
  Print(0, 0, " --Name-- ");
  //Print(0, 12, "--Inflow--");
  //Print(0, 24, "--Outflow--");
  //Print(0, 37, "--Quantity--");
  
  Print(rxn_lib.GetSize()+3, 2, "Resources associated with Reaction ");
  Print(rxn_lib.GetSize()+4, 2, "--Name--  --Inflow--  --Outflow--  --Quantity--");
  
  Print(6, 54, "Next Reaction");
  
  SetBoldColor(COLOR_CYAN);
  for(int i=0; i<rxn_lib.GetSize(); i++)
  {
    Print(i+1, 1, rxn_lib.GetReaction(i)->GetName());
    //Print(i+1, 12, "%7.2f", res_lib.GetResource(i)->GetInflow());
    //Print(i+1, 24, "%7.2f", res_lib.GetResource(i)->GetOutflow());
  }
  
  SetColor(COLOR_WHITE);
  Box(rxn_lib.GetSize()+2, 0, Height()-rxn_lib.GetSize()-2, Width(), true);
  
  SetBoldColor(COLOR_YELLOW);
  Print(1, 58, "Reaction View");
}

void cEnvironmentScreen::Update(cAvidaContext& ctx)
{
  if(mode==ENVIRONMENT_MODE_RESOURCE)
    UpdateResource();
  else
    UpdateReaction();
  Refresh();
}

void cEnvironmentScreen::UpdateResource()
{
  const cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
  const cReactionLib & rxn_lib = m_world->GetEnvironment().GetReactionLib();
  const int num_resources = m_world->GetPopulation().GetResources().GetSize();
  
  // If there are no resources, then we have nothing to update.
  if (num_resources == 0) return;
  
  // Update the quantity of each resource.
  SetBoldColor(COLOR_CYAN);
  for(int i = 0; i < num_resources; i++)
  {
    Print(i+1, 40, "%7.2f", m_world->GetPopulation().GetResources()[i]);
  }
  
  // Highlight the current resource in blue.
  SetBoldColor(COLOR_BLUE);
  Print(res_selection+1, 1, res_lib.GetResource(res_selection)->GetName());
  Print(res_selection+1, 12, "%7.2f", res_lib.GetResource(res_selection)->GetInflow());
  Print(res_selection+1, 24, "%7.2f", res_lib.GetResource(res_selection)->GetOutflow());
  Print(res_selection+1, 40, "%7.2f", m_world->GetPopulation().GetResources()[res_selection]);
  
  // Print all of the information about the reaction(s) associated with
  // current resource.
  
  int offset=0;
  SetBoldColor(COLOR_CYAN);
  for(int i=0; i<rxn_lib.GetSize(); i++) {
    for(int j=0; j<rxn_lib.GetReaction(i)->GetProcesses().GetSize(); j++) {
      if(rxn_lib.GetReaction(i)->GetProcesses().GetPos(j)->GetResource() != NULL)
      {
        cout << "BDB part 1 " << rxn_lib.GetReaction(i)->GetProcesses().GetPos(j)->GetResource()->GetName() << endl;
        cout << "BDB part 2 " << res_lib.GetResource(res_selection)->GetName() << endl;
        if(rxn_lib.GetReaction(i)->GetProcesses().GetPos(j)->GetResource()->GetName() ==
           res_lib.GetResource(res_selection)->GetName()) {
          int reactions = 0;
          for(int k=0; k<m_world->GetPopulation().GetSize(); ++k) {
            cPopulationCell& cell = m_world->GetPopulation().GetCell(k);
            if(cell.IsOccupied()) {
              const tArray<int>& org_rx = cell.GetOrganism()->GetPhenotype().GetLastReactionCount();
              reactions += org_rx[i];
            }
          }
          
          Print(num_resources + 5 + offset, 2, "%-10s %7d",
                static_cast<const char*>(rxn_lib.GetReaction(i)->GetName()),
                reactions);
          offset++;
        }
      }
    }
  }
  
  // Print the name of the current resource at the bottom of the screen.
  SetBoldColor(COLOR_WHITE);
  Print(res_lib.GetSize()+3, 37, "%s", static_cast<const char*>(res_lib.GetResource(res_selection)->GetName()));
  Print(res_lib.GetSize()+3, res_lib.GetResource(res_selection)->GetName().GetSize()+37, ":");
  Print(res_lib.GetSize()+3, res_lib.GetResource(res_selection)->GetName().GetSize()+38, "        ");
  
}

void cEnvironmentScreen::UpdateReaction()
{
  const cReactionLib & rxn_lib = m_world->GetEnvironment().GetReactionLib();
  const cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
  const int num_reactions = m_world->GetStats().GetReactions().GetSize();
  
  // If we have no reactions, stop right here.
  if (num_reactions == 0) return;
  
  // Find the sum of the reactions
  
  tArray<int> reactions(num_reactions);
  reactions.SetAll(0);
  
  for(int i=0; i<m_world->GetPopulation().GetSize(); ++i) {
    cPopulationCell& cell = m_world->GetPopulation().GetCell(i);
    if(cell.IsOccupied()) {
      const tArray<int>& org_rx = cell.GetOrganism()->GetPhenotype().GetLastReactionCount();
      for(int j=0; j<num_reactions; ++j) {
        reactions[j] += org_rx[j];
      }
    }
  }
  
  // For each reaction, print how often it was performed.
  SetBoldColor(COLOR_CYAN);
  for(int i = 0; i < num_reactions; i++) {
    Print(i+1, 40, "%7d", reactions[i]);
  }
  
  // Highlight the selected reaction.
  SetBoldColor(COLOR_BLUE);
  Print(rxn_selection+1, 1, rxn_lib.GetReaction(rxn_selection)->GetName());
  Print(rxn_selection+1, 40, "%7d", reactions[rxn_selection]);
  
  
  // Update header on reaction section.
  SetBoldColor(COLOR_WHITE);
  Print(rxn_lib.GetSize()+3, 37, "%-s", static_cast<const char*>(rxn_lib.GetReaction(rxn_selection)->GetName()));
  Print(rxn_lib.GetSize()+3, rxn_lib.GetReaction(rxn_selection)->GetName().GetSize()+37, ":");
  Print(rxn_lib.GetSize()+3, rxn_lib.GetReaction(rxn_selection)->GetName().GetSize()+38, "        ");
  
  // Print information about each resource associated with active reaction.
  int offset=0;
  SetBoldColor(COLOR_CYAN);
  const int num_processes = rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetSize();
  for(int i = 0; i < num_processes; i++) {
    cResource * cur_resource =
    rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetPos(i)->GetResource();
    
    // Ignore all processes that are not associated with resources.
    if (cur_resource == NULL) continue;
    
    // Print info about this resource.
    Print(m_world->GetStats().GetReactions().GetSize()+5+offset, 2,
          "%-10s", static_cast<const char*>(cur_resource->GetName()));
    Print(m_world->GetStats().GetReactions().GetSize()+5+offset, 13, "%7.2f",
          cur_resource->GetInflow());
    Print(m_world->GetStats().GetReactions().GetSize()+5+offset, 25, "%7.2f",
          cur_resource->GetOutflow());
    for(int j=0; j < res_lib.GetSize(); j++) {
      if (res_lib.GetResource(j)->GetName() == cur_resource->GetName()) {
        Print(m_world->GetStats().GetReactions().GetSize()+5+offset, 40, "%7.2f",
              m_world->GetPopulation().GetResources()[j]);
      }
    }
    offset++;
  }
}

void cEnvironmentScreen::DoInput(cAvidaContext& ctx, int in_char)
{
  int last_selection;
  const cResourceLib & res_lib = m_world->GetEnvironment().GetResourceLib();
  const cReactionLib & rxn_lib = m_world->GetEnvironment().GetReactionLib();
  SetBoldColor(COLOR_CYAN);
  
  switch (in_char) {
    case KEY_DOWN:
      if(mode==ENVIRONMENT_MODE_RESOURCE ) {
        const int num_resources = m_world->GetPopulation().GetResources().GetSize();
        if (num_resources > 0) {
          last_selection=res_selection;
          res_selection++;
          res_selection %= num_resources;
          
          Print(last_selection+1, 1, res_lib.GetResource(last_selection)->GetName());
          Print(last_selection+1, 12, "%7.2f", res_lib.GetResource(last_selection)->GetInflow());
          Print(last_selection+1, 24, "%7.2f", res_lib.GetResource(last_selection)->GetOutflow());
        }
      }
      else { // ENVIRONMENT_MODE_REACTION
        const int num_reactions = m_world->GetStats().GetReactions().GetSize();
        if (num_reactions > 0) {
          last_selection = rxn_selection;
          rxn_selection++;
          rxn_selection %= num_reactions;
          
          Print(last_selection+1, 1, rxn_lib.GetReaction(last_selection)->GetName());
          //Print(last_selection+1, 12, "%7.2f", rxn_lib.GetReaction(last_selection)->GetInflow());
          //Print(last_selection+1, 24, "%7.2f", rxn_lib.GetReaction(last_selection)->GetOutflow());
        }
      }
      
      Update(ctx);
      break;
    case KEY_UP:
      if(mode == ENVIRONMENT_MODE_RESOURCE) {
        const int num_resources = m_world->GetPopulation().GetResources().GetSize();
        if (num_resources > 0) {
          last_selection = res_selection;
          res_selection--;
          if(res_selection < 0) res_selection = num_resources - 1;
          
          Print(last_selection+1, 1, res_lib.GetResource(last_selection)->GetName());
          Print(last_selection+1, 12, "%7.2f", res_lib.GetResource(last_selection)->GetInflow());
          Print(last_selection+1, 24, "%7.2f", res_lib.GetResource(last_selection)->GetOutflow());
        }
      }
      else { // ENVIRONMENT_MODE_REACTIONS
        const int num_reactions = m_world->GetStats().GetReactions().GetSize();
        if (num_reactions > 0) {
          last_selection=rxn_selection;
          rxn_selection--;
          if (rxn_selection < 0) rxn_selection = num_reactions - 1;
          
          Print(last_selection+1, 1, rxn_lib.GetReaction(last_selection)->GetName());
          //Print(last_selection+1, 12, "%7.2f", rxn_lib.GetReaction(last_selection)->GetInflow());
          //Print(last_selection+1, 24, "%7.2f", rxn_lib.GetReaction(last_selection)->GetOutflow());
        }
      }
      
      Update(ctx);
      break;
    case '>':
    case '.':
    case '<':
    case ',':
      if(mode==ENVIRONMENT_MODE_RESOURCE)
        mode=ENVIRONMENT_MODE_REACTION;
      else
        mode=ENVIRONMENT_MODE_RESOURCE;
      Clear();
      Draw(ctx);
      break;
  }
}

