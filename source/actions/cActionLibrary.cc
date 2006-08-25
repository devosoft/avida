/*
 *  cActionLibrary.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cActionLibrary.h"

#include "DriverActions.h"
#include "EnvironmentActions.h"
#include "LandscapeActions.h"
#include "PopulationActions.h"
#include "PrintActions.h"
#include "SaveLoadActions.h"


cActionLibrary* cActionLibrary::ConstructDefaultActionLibrary()
{
  cActionLibrary* actlib = new cActionLibrary();

  RegisterDriverActions(actlib);
  RegisterEnvironmentActions(actlib);
  RegisterLandscapeActions(actlib);
  RegisterPopulationActions(actlib);
  RegisterPrintActions(actlib);
  RegisterSaveLoadActions(actlib);
  
  return actlib;
}

const cString cActionLibrary::DescribeAll() const
{
  tList<cString> names;
  tList<ClassDescFunction> funcs;
  m_desc_funcs.AsLists(names, funcs);
  
  cString ret("");
  
  tListIterator<cString> names_it(names);
  for (int i = 0; names_it.Next() != NULL; i++) {
    ret = ret + *names_it.Get() + " - " + Describe(*names_it.Get()) + "\n";
  }
  
  return ret;
}
