/*
 *  cActionLibrary.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cActionLibrary.h"

#include "DriverActions.h"
#include "EnvironmentActions.h"
#include "LandscapeActions.h"
#include "PopulationActions.h"
#include "PrintActions.h"
#include "SaveLoadActions.h"

#include "tDMSingleton.h"


cActionLibrary* cActionLibrary::buildDefaultActionLibrary()
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


void cActionLibrary::Initialize()
{
  tDMSingleton<cActionLibrary>::Initialize(&cActionLibrary::buildDefaultActionLibrary);
}

cActionLibrary& cActionLibrary::GetInstance()
{
  return tDMSingleton<cActionLibrary>::GetInstance();
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
