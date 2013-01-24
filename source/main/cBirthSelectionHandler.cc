/*
 *  cBirthSelectionHandler.cc
 *  Avida
 *
 *  Created by David Bryson on 4/1/09.
 *  Copyright 2009-2011 Michigan State University. All rights reserved.
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

#include "cBirthSelectionHandler.h"

cBirthSelectionHandler::~cBirthSelectionHandler() { ; }


int cBirthSelectionHandler::GetWaitingOffspringNumber(int which_mating_type)
{
  (void)which_mating_type;
  return -1;
}


void cBirthSelectionHandler::GetWaitingOffspringTaskData(int task_id, float results_array[])
{
  (void)task_id;
  results_array[0] = results_array[1] = results_array[2] = -1;
}


void cBirthSelectionHandler::PrintBirthChamber(const cString&)
{
  return;
}
