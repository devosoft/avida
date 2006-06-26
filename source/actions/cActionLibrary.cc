/*
 *  cActionLibrary.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cActionLibrary.h"

#include "LandscapeActions.h"
#include "PopulationActions.h"
#include "PrintActions.h"
#include "SaveLoadActions.h"


cActionLibrary* cActionLibrary::ConstructDefaultActionLibrary()
{
  cActionLibrary* actlib = new cActionLibrary();

  RegisterLandscapeActions(actlib);
  RegisterPopulationActions(actlib);
  RegisterPrintActions(actlib);
  RegisterSaveLoadActions(actlib);
  
  return actlib;
}
