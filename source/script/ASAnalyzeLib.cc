/*
 *  ASAnalyzeLib.cc
 *  Avida
 *
 *  Created by David Bryson on 10/20/08.
 *  Copyright 2008-2010 Michigan State University. All rights reserved.
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
#include "cSequence.h"
#include "cGenomeUtil.h"
#include "cGenotypeBatch.h"
#include "cHardwareManager.h"
#include "cInitFile.h"
#include "cResourceHistory.h"
#include "cWorld.h"

#include "tDataCommandManager.h"


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
  
  cGenotypeBatch* LoadBatchWithInstSet(cWorld* world, const cString& filename, cInstSet* inst_set)
  {
    cDriverStatusConduit& conduit = cDriverManager::Status();
    conduit.NotifyComment(cString("Loading: ") + filename);
    
    cInitFile input_file(filename);
    if (!input_file.WasOpened()) {
      tConstListIterator<cString> err_it(input_file.GetErrors());
      const cString* errstr = NULL;
      while ((errstr = err_it.Next()))  conduit.SignalError(*errstr);
      cString failstr(cStringUtil::Stringf("unable to load file: %s", *filename));
      conduit.SignalError(failstr, 1);
    }
    
    const cString filetype = input_file.GetFiletype();
    if (filetype != "genotype_data") {
      conduit.SignalError(cStringUtil::Stringf("unable to load files of type '%s'", *filetype), 1);;
    }
    
    if (world->GetVerbosity() >= VERBOSE_ON) {
      conduit.NotifyComment(cStringUtil::Stringf("Loading file of type: %s", *filetype));
    }
    
    
    // Construct a linked list of data types that can be loaded...
    tList< tDataEntryCommand<cAnalyzeGenotype> > output_list;
    tListIterator< tDataEntryCommand<cAnalyzeGenotype> > output_it(output_list);
    cAnalyzeGenotype::GetDataCommandManager().LoadCommandList(input_file.GetFormat(), output_list);
    bool id_inc = input_file.GetFormat().HasString("id");
    
    // Setup the genome...
    cSequence default_genome(1);
    int load_count = 0;
    cGenotypeBatch* batch = new cGenotypeBatch;
    
    for (int line_id = 0; line_id < input_file.GetNumLines(); line_id++) {
      cString cur_line = input_file.GetLine(line_id);
      
      cAnalyzeGenotype* genotype = new cAnalyzeGenotype(world, default_genome, *inst_set);
      
      output_it.Reset();
      tDataEntryCommand<cAnalyzeGenotype>* data_command = NULL;
      while ((data_command = output_it.Next())) data_command->SetValue(genotype, cur_line.PopWord());
      
      // Give this genotype a name.  Base it on the ID if possible.
      if (id_inc == false) genotype->SetName(cStringUtil::Stringf("org-%d", load_count++));
      else genotype->SetName(cStringUtil::Stringf("org-%d", genotype->GetID()));
      
      // Add this genotype to the proper batch.
      batch->List().PushRear(genotype);
    }
    
    // Adjust the flags on this batch
    batch->SetLineage(false);
    batch->SetAligned(false);
    
    return batch;
  }
  
  cGenotypeBatch* LoadBatch(cWorld* world, const cString& filename)
  {
    return LoadBatchWithInstSet(world, filename, &world->GetHardwareManager().GetInstSet());
  }
  
  
  cResourceHistory* LoadResourceHistory(const cString& filename)
  {
    cDriverStatusConduit& conduit = cDriverManager::Status();
    conduit.NotifyComment(cString("Loading: ") + filename);

    cResourceHistory* resources = new cResourceHistory;
    if (!resources->LoadFile(filename)) conduit.SignalError("failed to load resource file", 1);
    
    return resources;
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
  BIND_FUNCTION(cWorld, "LoadBatch", LoadBatch, cGenotypeBatch* (const cString&));
  BIND_FUNCTION(cWorld, "LoadBatchWithInstSet", LoadBatchWithInstSet, cGenotypeBatch* (const cString&, cInstSet*));

  REGISTER_FUNCTION(LoadResourceHistory, cResourceHistory* (const cString&));

#undef BIND_FUNCTION
#undef REGISTER_FUNCTION
}
