/*
 *  LandscapeActions.cc
 *  Avida
 *
 *  Created by David on 4/10/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "LandscapeActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyze.h"
#include "cAnalyzeGenotype.h"
#include "tAnalyzeJob.h"
#include "cClassificationManager.h"
#include "cGenotype.h"
#include "cGenotypeBatch.h"
#include "cHardwareManager.h"
#include "cLandscape.h"
#include "cStats.h"
#include "cString.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tSmartArray.h"
#include "defs.h"


class cActionAnalyzeLandscape : public cAction
{
private:
  cString m_filename;
  int m_trials;
  int m_min_found;
  int m_max_trials;
  int m_max_dist;
  
public:
  cActionAnalyzeLandscape(cWorld* world, const cString& args)
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
  
  const cString GetDescription()
  {
    return "AnalyzeLandscape [filename='land-analyze.dat'] [int trials=1000] [int min_found=0] [int max_trials=0] [int max_dist=10]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    tArray<tList<cLandscape> > batches(m_max_dist);
    tArray<int> depths;
    
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Performing landscape analysis on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Performing landscape analysis...");
      }

      cAnalyzeGenotype* genotype = NULL;
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      while (genotype = batch_it.Next()) {
        LoadGenome(batches, genotype->GetGenome());
        depths.Push(genotype->GetDepth());
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Performing landscape analysis...");

      cGenotype* genotype = m_world->GetClassificationManager().GetBestGenotype();
      LoadGenome(batches, genotype->GetGenome());
      depths.Push(genotype->GetDepth());
      update = m_world->GetStats().GetUpdate();
    }
    
    m_world->GetAnalyze().GetJobQueue().Execute();

    cDataFile& outfile = m_world->GetDataFile(m_filename);
    outfile.WriteComment("Landscape analysis.  Distance results are grouped by update/depth.");
    for (int i = 0; i < depths.GetSize(); i++) {
      for (int dist = 1; dist <= batches.GetSize(); dist++) {
        land = batches[dist - 1].Pop();

        outfile.Write(update, "update");
        outfile.Write(depths[i], "tree depth");
        outfile.Write(dist, "distance");
        outfile.Write(land->GetProbDead(), "fractional mutations lethal");
        outfile.Write(land->GetProbNeg(), "fractional mutations detrimental");
        outfile.Write(land->GetProbNeut(), "fractional mutations neutral");
        outfile.Write(land->GetProbPos(), "fractional mutations beneficial");
        outfile.Write(land->GetNumTrials(), "number of trials");
        outfile.Write(land->GetNumFound(), "number found");
        outfile.Write(land->GetAveFitness(), "average fitness");
        outfile.Write(land->GetAveSqrFitness(), "average sqr fitness");
        outfile.Endl();
        
        delete land;
      }
    }
  }
  
private:
  void LoadGenome(tArray<tList<cLandscape> >& batches, const cGenome& genome)
  {
    cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();

    for (int dist = batches.GetSize(); dist >= 1; dist--) {
      cLandscape* land = new cLandscape(m_world, genome, inst_set);
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


class cActionFullLandscape : public cAction
{
private:
  cString m_filename;
  int m_dist;
  tList<cLandscape> m_batch;
  
public:
  cActionFullLandscape(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("land-full.dat"), m_dist(1)
  {
      cString largs(args);
      if (largs.GetSize()) m_filename = largs.PopWord();
      if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
  }
  
  const cString GetDescription()
  {
    return "FullLandscape [filename='land-full.dat'] [int distance=1]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Full Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Full Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::Process));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Full Landscaping...");
      
      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetDistance(m_dist);
      land->Process(ctx);
      update = m_world->GetStats().GetUpdate();      
    }
    
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    while (land = m_batch.Pop()) {
      land->PrintStats(outfile, update);
      delete land;
    }
  }
};


class cActionRandomLandscape : public cAction
{
private:
  cString m_filename;
  int m_dist;
  int m_trials;
  tList<cLandscape> m_batch;

public:
  cActionRandomLandscape(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("land-random.dat"), m_dist(1), m_trials(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_dist = largs.PopWord().AsInt();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
  }
  
  const cString GetDescription()
  {
    return "RandomLandscape [filename='land-random.dat'] [int distance=1] [int trials=0]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Random Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Random Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        land->SetDistance(m_dist);
        land->SetTrials(m_trials);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::RandomProcess));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Random Landscaping...");

      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetDistance(m_dist);
      land->SetTrials(m_trials);
      land->RandomProcess(ctx);
      update = m_world->GetStats().GetUpdate();      
    }
    
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    while (land = m_batch.Pop()) {
      land->PrintStats(outfile, update);
      delete land;
    }
  }
};


class cActionSampleLandscape : public cAction
{
private:
  cString m_filename;
  int m_trials;
  tList<cLandscape> m_batch;
  
public:
  cActionSampleLandscape(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("land-sample.dat"), m_trials(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
    if (m_trials == 0) m_trials = m_world->GetHardwareManager().GetInstSet().GetSize() - 1;
  }
  
  const cString GetDescription()
  {
    return "SampleLandscape [filename='land-sample.dat'] [int trials=0]";
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Sample Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Sample Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        land->SetTrials(m_trials);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::SampleProcess));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Sample Landscaping...");

      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetTrials(m_trials);
      land->SampleProcess(ctx);
      update = m_world->GetStats().GetUpdate();      
    }
    
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    while (land = m_batch.Pop()) {
      land->PrintStats(outfile, update);
      delete land;
    }
  }
};


void RegisterLandscapeActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionAnalyzeLandscape>("AnalyzeLandscape");
  action_lib->Register<cActionFullLandscape>("FullLandscape");
  action_lib->Register<cActionRandomLandscape>("RandomLandscape");
  action_lib->Register<cActionSampleLandscape>("SampleLandscape");
}
