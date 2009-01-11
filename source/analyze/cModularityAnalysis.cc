/*
 *  cModularityAnalysis.cc
 *  Avida
 *
 *  Created by David on 1/11/09.
 *  Copyright 2009 Michigan State University. All rights reserved.
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

#include "cModularityAnalysis.h"

#include "cAnalyzeGenotype.h"
#include "tDataCommandManager.h"
#include "tDataEntry.h"


void cModularityAnalysis::Initialize()
{
//  tDataCommandManager<cAnalyzeGenotype>& dcm = cAnalyzeGenotype::GetDataCommandManager();
  
}

void cModularityAnalysis::CalcFunctionalModularity(cAvidaContext& ctx)
{
  
}


cModularityAnalysis::cModularityData::cModularityData()
: tasks_done(0), insts_tasks(0), tasks_prop(0.0), ave_tasks_per_site(0.0), ave_sites_per_task(0.0), ave_prop_nonoverlap(0.0)
{
}
