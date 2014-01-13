/*
 *  LandscapeActions.cc
 *  Avida
 *
 *  Created by David on 4/10/06.
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

#include "LandscapeActions.h"

#include "avida/Avida.h"

#include "avida/core/WorldDriver.h"
#include "avida/output/File.h"
#include "avida/systematics/Group.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "cGenotypeBatch.h"
#include "cHardwareManager.h"
#include "cInstSet.h"
#include "cLandscape.h"
#include "cMutationalNeighborhood.h"
#include "cMutationalNeighborhoodResults.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"
#include "tAnalyzeJob.h"
#include "tAnalyzeJobBatch.h"

using namespace Avida;


class cActionAnalyzeLandscape : public cAction  // @parallelized
{
private:
  cString m_filename;
  int m_trials;
  int m_min_found;
  int m_max_trials;
  int m_max_dist;
  
public:
  cActionAnalyzeLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_filename("land-analyze.dat"), m_trials(1000), m_min_found(0), m_max_trials(0), m_max_dist(10)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
    if (largs.GetSize()) m_min_found = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max_trials = largs.PopWord().AsInt();
    if (largs.GetSize()) m_max_dist = largs.PopWord().AsInt();
    if (m_max_dist <= 0) m_max_dist = 10;
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [filename='land-analyze.dat'] [int trials=1000] [int min_found=0] [int max_trials=0] [int max_dist=10]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    Apto::Array<tList<cLandscape> > batches(m_max_dist);
    Apto::Array<int> depths;
    
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Performing landscape analysis on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Performing landscape analysis...");
      }

      cAnalyzeGenotype* genotype = NULL;
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      while ((genotype = batch_it.Next())) {
        LoadGenome(batches, genotype->GetGenome());
        depths.Push(genotype->GetDepth());
      }
    } else {
//      if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
//        m_world->GetDriver().NotifyComment("Performing landscape analysis...");
//
//      cGenotype* genotype = m_world->GetClassificationManager().GetBestGenotype();
//      LoadGenome(batches, genotype->GetSequence());
//      depths.Push(genotype->GetDepth());
//      update = m_world->GetStats().GetUpdate();
    }
    
    m_world->GetAnalyze().GetJobQueue().Execute();

    Avida::Output::FilePtr df;
    if (ctx.GetAnalyzeMode()) df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    else df = Avida::Output::File::StaticWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("Landscape analysis.  Distance results are grouped by update/depth.");
    for (int i = 0; i < depths.GetSize(); i++) {
      for (int dist = 1; dist <= batches.GetSize(); dist++) {
        land = batches[dist - 1].Pop();

        df->Write(update, "update");
        df->Write(depths[i], "tree depth");
        df->Write(dist, "distance");
        df->Write(land->GetProbDead(), "fractional mutations lethal");
        df->Write(land->GetProbNeg(), "fractional mutations detrimental");
        df->Write(land->GetProbNeut(), "fractional mutations neutral");
        df->Write(land->GetProbPos(), "fractional mutations beneficial");
        df->Write(land->GetNumTrials(), "number of trials");
        df->Write(land->GetNumFound(), "number found");
        df->Write(land->GetAveFitness(), "average fitness");
        df->Write(land->GetAveSqrFitness(), "average sqr fitness");
        df->Endl();
        
        delete land;
      }
    }
  }
  
private:
  void LoadGenome(Apto::Array<tList<cLandscape> >& batches, const Genome& genome)
  {
    cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();

    for (int dist = batches.GetSize(); dist >= 1; dist--) {
      cLandscape* land = new cLandscape(m_world, genome);
      land->SetDistance(dist);
      land->SetTrials(m_trials);
      batches[dist - 1].PushRear(land);
      if (dist == 1) {
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::Process));        
      } else {
        land->SetMinFound(m_min_found);
        land->SetMaxTrials(m_max_trials);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::RandomProcess));
      }
    }
  }
};


/*
 * Precalculates landscape data for use in detail files.  The primary
 * advantage of this is that it supports multithreaded execution, whereas
 * lazy evaluation during detailing will be serialized.
 * @JEB: This also now supports setting some test CPU info, just manual inputs for now,
 * (BEWARE: resource settings that are passed are NOT HONORED yet).
*/

class cActionPrecalcLandscape : public cAction  // @parallelized
{
private:
  cCPUTestInfo m_cpu_test_info;

public:
  cActionPrecalcLandscape(cWorld* world, const cString& in_args, Feedback&) : cAction(world, in_args), m_cpu_test_info() 
  { 
    cString args(in_args); 
    cAnalyze::PopCommonCPUTestParameters(world, args, m_cpu_test_info);
  }
  
  static const cString GetDescription()
  {
    return "No Arguments";
  }
  
  void Process(cAvidaContext& ctx)
  {    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Precalculating landscape for batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Precalculating landscape...");
      }
      
      tAnalyzeJobBatch<cAnalyzeGenotype> jobbatch(m_world->GetAnalyze().GetJobQueue());
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      for (cAnalyzeGenotype* cur_genotype = batch_it.Next(); cur_genotype; cur_genotype = batch_it.Next()) {
        cur_genotype->SetCPUTestInfo(m_cpu_test_info);
        jobbatch.AddJob(cur_genotype, &cAnalyzeGenotype::CalcLandscape);
      }
      jobbatch.RunBatch();
    }
  }
};


class cActionFullLandscape : public cAction  // @parallelized
{
private:
  cString m_sfilename;
  cString m_efilename;
  cString m_cfilename;
  int m_dist;
  tList<cLandscape> m_batch;
  
public:
  cActionFullLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_sfilename("land-full.dat"), m_efilename(""), m_cfilename(""), m_dist(1)
  {
      cString largs(args);
      if (largs.GetSize()) m_sfilename = largs.PopWord();
      if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
      if (largs.GetSize()) m_efilename = largs.PopWord();
      if (largs.GetSize()) m_cfilename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-full.dat'] [int distance=1] [string entropy_file=''] [string sitecount_file='']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (m_dist < 1) {
      ctx.Driver().Feedback().Error("landscape distance must be >= 1");
      return;
    }
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Full Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Full Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        land = new cLandscape(m_world, genotype->GetGenome());
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::Process));
      }
      jobqueue.Execute();

      Avida::Output::FilePtr sf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_sfilename);
      Avida::Output::FilePtr ef;
      if (m_efilename.GetSize()) ef = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_efilename);
      Avida::Output::FilePtr cf;
      if (m_cfilename.GetSize()) cf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_cfilename);
      
      while ((land = m_batch.Pop())) {
        land->PrintStats(*sf, update);
        if (m_efilename.GetSize()) land->PrintEntropy(*ef);
        if (m_cfilename.GetSize()) land->PrintSiteCount(*cf);
        delete land;
      }
    }
  }
};


class cActionDumpLandscape : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  
public:
  cActionDumpLandscape(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename("land-dump.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-dump.dat']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    Avida::Output::FilePtr sdf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Dumping Landscape of batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Dumping Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {        
        // Create datafile for genotype landscape (${name}.land)
        cString gfn(genotype->GetName());
        gfn += ".land";
        Avida::Output::FilePtr gdf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)gfn);
        
        // Create the landscape object and process the dump
        cLandscape land(m_world, genotype->GetGenome());
        land.ProcessDump(ctx, *gdf);
        land.PrintStats(*sdf, -1);
      }
    }
  }
};


class cActionDeletionLandscape : public cAction  // @parallelized
{
private:
  cString m_sfilename;
  cString m_cfilename;
  int m_dist;
  tList<cLandscape> m_batch;
  
public:
  cActionDeletionLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_sfilename("land-del.dat"), m_cfilename(""), m_dist(1)
  {
      cString largs(args);
      if (largs.GetSize()) m_sfilename = largs.PopWord();
      if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
      if (largs.GetSize()) m_cfilename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-del.dat'] [int distance=1] [string sitecount_file='']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Deletion Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Deletion Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        land = new cLandscape(m_world, genotype->GetGenome());
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::ProcessDelete));
      }
      jobqueue.Execute();

      Avida::Output::FilePtr sf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_sfilename);
      Avida::Output::FilePtr cf;
      if (m_cfilename.GetSize()) cf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_cfilename);
      
      while ((land = m_batch.Pop())) {
        land->PrintStats(*sf, update);
        if (m_cfilename.GetSize()) land->PrintSiteCount(*cf);
        delete land;
      }
    }
  }
};


class cActionInsertionLandscape : public cAction  // @parallelized
{
private:
  cString m_sfilename;
  cString m_cfilename;
  int m_dist;
  tList<cLandscape> m_batch;
  
public:
  cActionInsertionLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_sfilename("land-ins.dat"), m_cfilename(""), m_dist(1)
  {
      cString largs(args);
      if (largs.GetSize()) m_sfilename = largs.PopWord();
      if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
      if (largs.GetSize()) m_cfilename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-ins.dat'] [int distance=1] [string sitecount_file='']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Insertion Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Insertion Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        land = new cLandscape(m_world, genotype->GetGenome());
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::ProcessInsert));
      }
      jobqueue.Execute();
    
      Avida::Output::FilePtr sf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_sfilename);
      Avida::Output::FilePtr cf;
      if (m_cfilename.GetSize()) cf = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_cfilename);
      
      while ((land = m_batch.Pop())) {
        land->PrintStats(*sf, update);
        if (m_cfilename.GetSize()) land->PrintSiteCount(*cf);
        delete land;
      }

    }
  }
};


class cActionPredictWLandscape : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  
public:
  cActionPredictWLandscape(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename("land-predict.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-predict.dat']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);

    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Predicting W Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Predicting W Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape land(m_world, genotype->GetGenome());
        land.PredictWProcess(ctx, *df);
      }
    }
  }
};


class cActionPredictNuLandscape : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  
public:
  cActionPredictNuLandscape(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename("land-predict.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-predict.dat']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);

    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Predicting Nu Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Predicting Nu Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape land(m_world, genotype->GetGenome());
        land.PredictNuProcess(ctx, *df);
      }
    }
  }
};


class cActionRandomLandscape : public cAction  // @parallelized
{
private:
  cString m_filename;
  int m_dist;
  int m_trials;
  tList<cLandscape> m_batch;

public:
  cActionRandomLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_filename("land-random.dat"), m_dist(1), m_trials(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-random.dat'] [int distance=1] [int trials=0]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Random Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Random Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome());
        land->SetDistance(m_dist);
        land->SetTrials(m_trials);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::RandomProcess));
      }
      jobqueue.Execute();
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      while ((land = m_batch.Pop())) {
        land->PrintStats(*df, update);
        delete land;
      }
    }
    
  }
};


class cActionSampleLandscape : public cAction  // @parallelized
{
private:
  cString m_filename;
  int m_trials;
  tList<cLandscape> m_batch;
  
public:
  cActionSampleLandscape(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_filename("land-sample.dat"), m_trials(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='land-sample.dat'] [int trials=0]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Sample Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Sample Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome());
        land->SetTrials(m_trials);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::SampleProcess));
      }
      jobqueue.Execute();
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      while ((land = m_batch.Pop())) {
        land->PrintStats(*df, update);
        delete land;
      }
    }
  }
};


class cActionHillClimb : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  
public:
  cActionHillClimb(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename("hillclimb.dat")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename='hillclimb.dat']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Calculating Hill Climb on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Calculating Hill Climb...");
      }
      
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape land(m_world, genotype->GetGenome());
        land.HillClimb(ctx, *df);
      }
    }
  }
};



class cActionMutationalNeighborhood : public cAction  // @parallelized
{
private:
  cString m_filename;
  int m_target;
  
  struct sBatchEntry {
    cMutationalNeighborhood* mutn;
    int depth;
    
    sBatchEntry(cMutationalNeighborhood* in_mutn, int in_depth) : mutn(in_mutn), depth(in_depth) { ; }
    ~sBatchEntry() { delete mutn; }
  };
  tList<sBatchEntry> m_batch;
  
public:
  cActionMutationalNeighborhood(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_filename("mut-neighborhood.dat"), m_target(-1)
  {
      cString largs(args);
      if (largs.GetSize()) m_filename = largs.PopWord();
      if (largs.GetSize()) m_target = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string fname='mut-neighborhood.dat'] [int target=-1]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    cMutationalNeighborhood* mutn = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Calculating Mutational Neighborhood for batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Calculating Mutational Neighborhood...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        mutn = new cMutationalNeighborhood(m_world, genotype->GetGenome(), m_target);
        m_batch.PushRear(new sBatchEntry(mutn, genotype->GetDepth()));
        jobqueue.AddJob(new tAnalyzeJob<cMutationalNeighborhood>(mutn, &cMutationalNeighborhood::Process));
      }
      jobqueue.Execute();
    }
    
    cMutationalNeighborhoodResults* results = NULL;
    sBatchEntry* entry = NULL;
    Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
    df->WriteComment("IMPORTANT: Mutational Neighborhood is *EXPERIMENTAL*");
    df->WriteComment("Output data and format is subject to change in future releases.");
    while ((entry = m_batch.Pop())) {
      results = new cMutationalNeighborhoodResults(entry->mutn);
      results->PrintStats(*df, entry->depth);
      delete results;
      delete entry;
    }
  }
};




class cActionPairTestLandscape : public cAction  // @parallelized
{
private:
  cString m_filename;
  int m_sample_size;
  tList<cLandscape> m_batch;
  
public:
  cActionPairTestLandscape(cWorld* world, const cString& args, Feedback&)
  : cAction(world, args), m_filename("land-pairs.dat"), m_sample_size(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_sample_size = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [string filename=''] [int sample_size=0]";
  }
  
  void Process(cAvidaContext& ctx)
  {    
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetVerbosity() >= VERBOSE_ON) {
        cString msg("Pair Testing Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        ctx.Driver().Feedback().Notify(msg);
      } else if (m_world->GetVerbosity() > VERBOSE_SILENT) {
        ctx.Driver().Feedback().Notify("Pair Testing Landscape...");
      }
      
      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while ((genotype = batch_it.Next())) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome());
        if (m_sample_size) {
          land->SetTrials(m_sample_size);
          jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::TestPairs));
        } else {
          jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::TestAllPairs));
        }
        m_batch.PushRear(land);
      }
      jobqueue.Execute();
      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      while ((land = m_batch.Pop())) {
        land->PrintStats(*df, update);
        delete land;
      }
    }    
  }
};


class cActionAnalyzePopulation : public cAction  // @parallelized
{
private:
  double m_sprob;
  int m_cland;
  int m_save_genotypes;
  cString m_filename;

  class cPopOrgData
  {
  private:
    cWorld* m_world;
    cOrganism* m_org;
    int m_cell;
    bool m_cland;
    cLandscape* m_land;

  public:
    cPopOrgData(cWorld* world, cOrganism* org, int cell, bool cland)
      : m_world(world), m_org(org), m_cell(cell), m_cland(cland), m_land(NULL) { ; }
    ~cPopOrgData() { delete m_land; }
    
    inline cOrganism* GetOrganism() { return m_org; }
    inline int GetCellID() { return m_cell; }
    inline void PrintLand(Avida::Output::File& df, int update) { if (m_land) m_land->PrintStats(df, update); else df.Endl(); }
    
    void Process(cAvidaContext& ctx)
    {
      if (m_org->GetTestFitness(ctx) > 0.0 && m_cland) {
        m_land = new cLandscape(m_world, m_org->GetGenome());
        m_land->SetDistance(1);
        m_land->Process(ctx);
      }
    }
  };
  
  
public:
  cActionAnalyzePopulation(cWorld* world, const cString& args, Feedback&)
    : cAction(world, args), m_sprob(1.0), m_cland(0), m_save_genotypes(0), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_sprob = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_cland = largs.PopWord().AsInt();
    if (largs.GetSize()) m_save_genotypes = largs.PopWord().AsInt();
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription()
  {
    return "Arguments: [double sample_prob=1] [int landscape=0] [int save_genotype=0] [string filename='']";
  }
  
  void Process(cAvidaContext& ctx)
  {
    if (ctx.GetAnalyzeMode()) {
      ctx.Driver().Feedback().Warning("AnalyzePopulation not currently supported in Analyze Mode.");
    } else {
      if (m_world->GetVerbosity() >= VERBOSE_DETAILS)
        ctx.Driver().Feedback().Notify("Analyzing Population...");

      const int update = m_world->GetStats().GetUpdate();

      cString filename(m_filename);
      if (filename == "") filename.Set("pop-analysis-%d.dat", update);
            
      cPopOrgData* orgdata;
      tList<cPopOrgData> batch;
 
      cPopulation& pop = m_world->GetPopulation();
      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      const double skip_prob = 1.0 - m_sprob;
      for (int i = 0; i < pop.GetSize(); i++) {
        if (pop.GetCell(i).IsOccupied() == false) continue;  // No organism...
        if (ctx.GetRandom().P(skip_prob)) continue;       // Not sampled...
        
        orgdata = new cPopOrgData(m_world, pop.GetCell(i).GetOrganism(), i, m_cland);
        batch.PushRear(orgdata);
        jobqueue.AddJob(new tAnalyzeJob<cPopOrgData>(orgdata, &cPopOrgData::Process));
      }

      Avida::Output::FilePtr df = Avida::Output::File::CreateWithPath(m_world->GetNewWorld(), (const char*)m_filename);
      cTestCPU* testcpu = (m_save_genotypes) ? m_world->GetHardwareManager().CreateTestCPU(ctx) : NULL;
      while ((orgdata = batch.Pop())) {
        cOrganism* organism = orgdata->GetOrganism();
        cPhenotype& phenotype = organism->GetPhenotype();
        
        Systematics::GroupPtr genotype = organism->SystematicsGroup("genotype");
        
        ConstInstructionSequencePtr seq;
        seq.DynamicCastFrom(organism->GetGenome().Representation());
        assert(seq);
        
        cString name;
        if ((bool)Apto::StrAs(genotype->Properties().Get("threshold"))) name = genotype->Properties().Get("name").StringValue();
        else name.Set("%03d-no_name-u%i-c%i", seq->GetSize(), update, orgdata->GetCellID());

        
        df->Write(orgdata->GetCellID(), "Cell ID");
        df->Write(name, "Organism Name");
        
        df->Write(seq->GetSize(),"Genome Length");
        df->Write(organism->GetTestFitness(ctx), "Fitness (test-cpu)");
        df->Write(phenotype.GetFitness(), "Fitness (actual)");
        df->Write(organism->GetLineageLabel(), "Lineage Label");
        df->Write(phenotype.GetNeutralMetric(), "Neutral Metric");
        orgdata->PrintLand(*df, update);
        
        // save into archive
        if (m_save_genotypes) {
          name.Set("archive/%s.org", static_cast<const char *>(name));
          testcpu->PrintGenome(ctx, organism->GetGenome(), name);
        }
      }
      delete testcpu;
    }
  }
};



void RegisterLandscapeActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionAnalyzeLandscape>("AnalyzeLandscape");
  action_lib->Register<cActionPrecalcLandscape>("PrecalcLandscape");
  action_lib->Register<cActionFullLandscape>("FullLandscape");
  action_lib->Register<cActionDumpLandscape>("DumpLandscape");
  action_lib->Register<cActionDeletionLandscape>("DeletionLandscape");
  action_lib->Register<cActionInsertionLandscape>("InsertionLandscape");
  action_lib->Register<cActionPredictWLandscape>("PredictWLandscape");
  action_lib->Register<cActionPredictNuLandscape>("PredictNuLandscape");
  action_lib->Register<cActionRandomLandscape>("RandomLandscape");
  action_lib->Register<cActionSampleLandscape>("SampleLandscape");
  action_lib->Register<cActionHillClimb>("HillClimb");
  action_lib->Register<cActionPairTestLandscape>("PairTestLandscape");
  action_lib->Register<cActionAnalyzePopulation>("AnalyzePopulation");

  action_lib->Register<cActionMutationalNeighborhood>("MutationalNeighborhood");
  
}
