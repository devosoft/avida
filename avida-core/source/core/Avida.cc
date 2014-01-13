/*
 *  core/Avida.cc
 *  avida-core
 *
 *  Created by David on 4/17/11.
 *  Copyright 2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#include "avida/Avida.h"

#include "apto/platform.h"
#include "avida/core/GlobalObject.h"
#include "avida/private/systematics/Genotype.h"

#include "AvidaTools.h"
#include "cActionLibrary.h"
#include "cAnalyzeGenotype.h"
#include "cHardwareManager.h"
#include "cModularityAnalysis.h"
#include "cOrganism.h"

#include <csignal>


static void exitAvida(int exit_code) {
  signal(SIGINT, SIG_IGN);          // Ignore all future interupts.
  exit(exit_code);
}


void Avida::Initialize()
{
  Apto::Platform::Initialize();
  
  // Catch Interrupt making sure to close appropriately
  signal(SIGINT, exitAvida);
  
  GlobalObjectManager::Initialize();
  
  cActionLibrary::Initialize();
  cAnalyzeGenotype::Initialize();
  cHardwareManager::Initialize();
  cModularityAnalysis::Initialize();
  cOrganism::Initialize();
  
  Systematics::Genotype::Initialize();
}
