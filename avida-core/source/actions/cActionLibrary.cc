/*
 *  cActionLibrary.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
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


Apto::String cActionLibrary::DescribeAll() const
{
  Apto::String ret("");
  
  for (Apto::Map<Apto::String, ClassDescFunction>::ConstIterator it = m_desc_funcs.Begin(); it.Next();) {
    ret = ret + it.Get()->Value1() + " - " + (*it.Get()->Value2())() + "\n";
  }
  
  return ret;
}
