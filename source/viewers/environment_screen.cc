//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "config.hh"
#include "environment.hh"
#include "genebank.hh"
#include "genotype.hh"
#include "population.hh"
#ifndef REACTION_HH
#include "reaction.hh"
#endif
#ifndef REACTION_PROCESS_HH
#include "reaction_process.hh"
#endif
#ifndef RESOURCE_HH
#include "resource.hh"
#endif
#include "species.hh"
#include "stats.hh"
#include "population_cell.hh"
#include "organism.hh"

#include "environment_screen.hh"


using namespace std;


void cEnvironmentScreen::Draw()
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
  Update();
}

void cEnvironmentScreen::DrawResource()
{
  const cResourceLib & res_lib = population.GetEnvironment().GetResourceLib();

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
  Box(0, res_lib.GetSize()+2, Width(), Height()-res_lib.GetSize()-2);

  SetBoldColor(COLOR_YELLOW);
  Print(1, 58, "Resource View");
}

void cEnvironmentScreen::DrawReaction()
{
  const cReactionLib & rxn_lib = population.GetEnvironment().GetReactionLib();

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
  Box(0, rxn_lib.GetSize()+2, Width(), Height()-rxn_lib.GetSize()-2);

  SetBoldColor(COLOR_YELLOW);
  Print(1, 58, "Reaction View");
}

void cEnvironmentScreen::Update()
{
  if(mode==ENVIRONMENT_MODE_RESOURCE)
    UpdateResource();
  else
    UpdateReaction();
  Refresh();
}

void cEnvironmentScreen::UpdateResource()
{
  const cResourceLib & res_lib = population.GetEnvironment().GetResourceLib();
  const cReactionLib & rxn_lib = population.GetEnvironment().GetReactionLib();

  SetBoldColor(COLOR_CYAN);
  //int j=0;
  //int last_count=0;
  for(int i=0; i<population.GetResources().GetSize(); i++)
    {
      Print(i+1, 40, "%7.2f", population.GetResources()[i]);
      //if(population.GetResources()[i]/last_count < population.GetResources()[i].
      //Print(i+2, 65, "%7.2f", population.GetResources()[i]/last_count);
    }
 
  SetBoldColor(COLOR_BLUE);
  Print(res_selection+1, 1, res_lib.GetResource(res_selection)->GetName());
  Print(res_selection+1, 12, "%7.2f", res_lib.GetResource(res_selection)->GetInflow());
  Print(res_selection+1, 24, "%7.2f", res_lib.GetResource(res_selection)->GetOutflow());
  Print(res_selection+1, 40, "%7.2f", population.GetResources()[res_selection]);
 
  int offset=0;

  SetBoldColor(COLOR_CYAN);
  for(int i=0; i<rxn_lib.GetSize(); i++)
    {
      for(int j=0; j<rxn_lib.GetReaction(i)->GetProcesses().GetSize(); j++)
	{
	  if(rxn_lib.GetReaction(i)->GetProcesses().GetPos(j)->GetResource()->GetName() == 
	     res_lib.GetResource(res_selection)->GetName())
	    {
	      Print(population.GetResources().GetSize()+5+offset, 2, 
		    rxn_lib.GetReaction(i)->GetName());
	      offset++;
	    }
	}
    }
  
  SetBoldColor(COLOR_WHITE);
  Print(res_lib.GetSize()+3, 37, "%s", res_lib.GetResource(res_selection)->GetName().GetData());
  Print(res_lib.GetSize()+3, res_lib.GetResource(res_selection)->GetName().GetSize()+37, ":");
  Print(res_lib.GetSize()+3, res_lib.GetResource(res_selection)->GetName().GetSize()+38, "        ");
    
}

void cEnvironmentScreen::UpdateReaction()
{
  const cReactionLib & rxn_lib = population.GetEnvironment().GetReactionLib();
  const cResourceLib & res_lib = population.GetEnvironment().GetResourceLib();

  SetBoldColor(COLOR_CYAN);
  //int j=0;
  //int last_count=0;
  for(int i=0; i<population.GetStats().GetReactions().GetSize(); i++)
    {
      Print(i+1, 40, "%7.2f", population.GetStats().GetReactions()[i]);
      //if(population.GetResources()[i]/last_count < population.GetResources()[i].
      //Print(i+2, 65, "%7.2f", population.GetResources()[i]/last_count);
    }
 
  SetBoldColor(COLOR_BLUE);
  Print(rxn_selection+1, 1, rxn_lib.GetReaction(rxn_selection)->GetName());
  Print(rxn_selection+1, 40, "%7.2f", population.GetStats().GetReactions()[rxn_selection]);
    

  int offset=0;

  SetBoldColor(COLOR_CYAN);
  for(int i=0; i<rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetSize(); i++)
    {
      Print(population.GetStats().GetReactions().GetSize()+5+offset, 2, 
	    rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetPos(i)->GetResource()->GetName());
      Print(population.GetStats().GetReactions().GetSize()+5+offset, 13, "%7.2f",
	    rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetPos(i)->GetResource()->GetInflow());
      Print(population.GetStats().GetReactions().GetSize()+5+offset, 25, "%7.2f",
	    rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetPos(i)->GetResource()->GetOutflow());
      for(int j=0; j<res_lib.GetSize(); j++)
	{
	  if(res_lib.GetResource(j)->GetName() ==
	     rxn_lib.GetReaction(rxn_selection)->GetProcesses().GetPos(i)->GetResource()->GetName())
	    Print(population.GetStats().GetReactions().GetSize()+5+offset, 40, "%7.2f", 
		  population.GetResources()[j]);
	}
      offset++;  
    }
  SetBoldColor(COLOR_WHITE);
  Print(rxn_lib.GetSize()+3, 37, "%s", rxn_lib.GetReaction(rxn_selection)->GetName().GetData());
  Print(rxn_lib.GetSize()+3, rxn_lib.GetReaction(rxn_selection)->GetName().GetSize()+37, ":");
  Print(rxn_lib.GetSize()+3, rxn_lib.GetReaction(rxn_selection)->GetName().GetSize()+38, "        ");
   
}

void cEnvironmentScreen::DoInput(int in_char)
{
  int last_selection;
  const cResourceLib & res_lib = population.GetEnvironment().GetResourceLib();
  const cReactionLib & rxn_lib = population.GetEnvironment().GetReactionLib();
  SetBoldColor(COLOR_CYAN);

  switch (in_char) {
  case KEY_DOWN:
    if(mode==ENVIRONMENT_MODE_RESOURCE ) {
      last_selection=res_selection;
      res_selection++;
      res_selection%=population.GetResources().GetSize();

      Print(last_selection+1, 1, res_lib.GetResource(last_selection)->GetName());
      Print(last_selection+1, 12, "%7.2f", res_lib.GetResource(last_selection)->GetInflow());
      Print(last_selection+1, 24, "%7.2f", res_lib.GetResource(last_selection)->GetOutflow());
    }
    else {
      last_selection=rxn_selection;
      rxn_selection++;
      rxn_selection%=population.GetStats().GetReactions().GetSize();

      Print(last_selection+1, 1, rxn_lib.GetReaction(last_selection)->GetName());
      //Print(last_selection+1, 12, "%7.2f", rxn_lib.GetReaction(last_selection)->GetInflow());
      //Print(last_selection+1, 24, "%7.2f", rxn_lib.GetReaction(last_selection)->GetOutflow());
    }

    Update();
    break;
  case KEY_UP:
    if(mode==ENVIRONMENT_MODE_RESOURCE) {
      last_selection=res_selection;
      res_selection--;
      if(res_selection < 0) res_selection=population.GetResources().GetSize()-1;
   
      Print(last_selection+1, 1, res_lib.GetResource(last_selection)->GetName());
      Print(last_selection+1, 12, "%7.2f", res_lib.GetResource(last_selection)->GetInflow());
      Print(last_selection+1, 24, "%7.2f", res_lib.GetResource(last_selection)->GetOutflow());
    }
    else {
      last_selection=rxn_selection;
      rxn_selection--;
      if(rxn_selection < 0) rxn_selection=population.GetStats().GetReactions().GetSize()-1;
   
      Print(last_selection+1, 1, rxn_lib.GetReaction(last_selection)->GetName());
      //Print(last_selection+1, 12, "%7.2f", rxn_lib.GetReaction(last_selection)->GetInflow());
      //Print(last_selection+1, 24, "%7.2f", rxn_lib.GetReaction(last_selection)->GetOutflow());
    }

    Update();
    break;
  case '>':
  case '<':
    if(mode==ENVIRONMENT_MODE_RESOURCE)
      mode=ENVIRONMENT_MODE_REACTION;
    else
      mode=ENVIRONMENT_MODE_RESOURCE;
    Clear();
    Draw();
    break;
    }
}

