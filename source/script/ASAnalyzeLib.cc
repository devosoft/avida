/*
 *  ASAnalyzeLib.cc
 *  Avida
 *
 *  Created by David Bryson on 10/20/08.
 *  Copyright 2008 Michigan State University. All rights reserved.
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

#include "ASAnalyzeLib.h"

#include "ASAvidaNativeObjects.h"

#include "cASCPPParameter_NativeObjectSupport.h"
#include "cASFunction.h"
#include "cASLibrary.h"
#include "cASNativeObject.h"

#include "cAnalyzeGenotype.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cHardwareManager.h"
#include "cWorld.h"


class cWorld;


namespace ASAnalyzeLib {
  
  cAnalyzeGenotype* LoadOrganismWithInstSet(cWorld* world, const cString& filename, cInstSet* inst_set)
  {
    cDriverManager::Status().NotifyComment(cString("Loading: ") + filename);
    
    cGenome genome(cGenomeUtil::LoadGenome(filename, *inst_set));
    
    // Construct the new genotype..
    cAnalyzeGenotype* genotype = new cAnalyzeGenotype(world, genome, *inst_set);
    
    cString genomename(filename);
    // Determine the organism's original name -- strip off directory...
    while (genomename.Find('/') != -1) genomename.Pop('/');
    while (genomename.Find('\\') != -1) genomename.Pop('\\');
    genomename.Replace(".gen", "");  // Remove the .gen from the filename.
    genotype->SetName(genomename);
    
    return genotype;
  }
  
  
  cAnalyzeGenotype* LoadOrganism(cWorld* world, const cString& filename)
  {
    return LoadOrganismWithInstSet(world, filename, &world->GetHardwareManager().GetInstSet());
  }
  


  cAnalyzeGenotype* LoadSequenceWithInstSet(cWorld* world, const cString& seq, cInstSet* inst_set)
  {
    cDriverManager::Status().NotifyComment(cString("Loading: ") + seq);
    return new cAnalyzeGenotype(world, seq, *inst_set);
  }
  
  
  cAnalyzeGenotype* LoadSequence(cWorld* world, const cString& seq)
  {
    cDriverManager::Status().NotifyComment(cString("Loading: ") + seq);
    return new cAnalyzeGenotype(world, seq, world->GetHardwareManager().GetInstSet());
  }
  
  
};


void RegisterASAnalyzeLib(cASLibrary* lib)
{
#define BIND_FUNCTION(CLASS, NAME, METHOD, SIGNATURE) \
  tASNativeObject<CLASS>::RegisterMethod(new tASNativeObjectBoundFunction<CLASS, SIGNATURE>(&ASAnalyzeLib::METHOD), NAME);
#define REGISTER_FUNCTION(NAME, SIGNATURE) lib->RegisterFunction(new tASFunction<SIGNATURE>(&ASAnalyzeLib::NAME, #NAME));
  

  BIND_FUNCTION(cWorld, "LoadOrganism", LoadOrganism, cAnalyzeGenotype* (const cString&));
  BIND_FUNCTION(cWorld, "LoadOrganismWithInstSet", LoadOrganismWithInstSet, cAnalyzeGenotype* (const cString&, cInstSet*));
  BIND_FUNCTION(cWorld, "LoadSequence", LoadSequence, cAnalyzeGenotype* (const cString&));
  BIND_FUNCTION(cWorld, "LoadSequenceWithInstSet", LoadSequenceWithInstSet, cAnalyzeGenotype* (const cString&, cInstSet*));


#undef BIND_FUNCTION
#undef REGISTER_FUNCTION
}
