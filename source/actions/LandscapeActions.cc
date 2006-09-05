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
#include "cMutationalNeighborhood.h"
#include "cMutationalNeighborhoodResults.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cStats.h"
#include "cString.h"
#include "cTestUtil.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "tSmartArray.h"
#include "defs.h"


class cActionAnalyzeLandscape : public cAction  // @parallelized
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
  
  static const cString GetDescription()
  {
    return "Arguments: [filename='land-analyze.dat'] [int trials=1000] [int min_found=0] [int max_trials=0] [int max_dist=10]";
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


/*
 Precalculates landscape data for use in detail files.  The primary advantage of
 this is that it supports multithreaded execution, whereas lazy evaluation during
 detailing will be serialized.
*/
class cActionPrecalcLandscape : public cAction  // @parallelized
{
public:
  cActionPrecalcLandscape(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  static const cString GetDescription()
  {
    return "No Arguments";
  }
  
  void Process(cAvidaContext& ctx)
  {    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Precalculating landscape for batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Precalculating landscape...");
      }
      
      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      for (cAnalyzeGenotype* cur_genotype = batch_it.Next(); cur_genotype; cur_genotype = batch_it.Next()) {
        jobqueue.AddJob(new tAnalyzeJob<cAnalyzeGenotype>(cur_genotype, &cAnalyzeGenotype::CalcLandscape));
      }
      jobqueue.Execute();
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
  cActionFullLandscape(cWorld* world, const cString& args)
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
        land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
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
    
    cDataFile& df = m_world->GetDataFile(m_sfilename);
    while (land = m_batch.Pop()) {
      land->PrintStats(df, update);
      if (m_efilename.GetSize()) land->PrintEntropy(m_world->GetDataFile(m_efilename));
      if (m_cfilename.GetSize()) land->PrintSiteCount(m_world->GetDataFile(m_cfilename));
      delete land;
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
  cActionDeletionLandscape(cWorld* world, const cString& args)
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
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Deletion Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Deletion Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::ProcessDelete));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Deletion Landscaping...");
      
      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetDistance(m_dist);
      land->ProcessDelete(ctx);
      update = m_world->GetStats().GetUpdate();      
    }
    
    cDataFile& df = m_world->GetDataFile(m_sfilename);
    while (land = m_batch.Pop()) {
      land->PrintStats(df, update);
      if (m_cfilename.GetSize()) land->PrintSiteCount(m_world->GetDataFile(m_cfilename));
      delete land;
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
  cActionInsertionLandscape(cWorld* world, const cString& args)
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
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Insertion Landscaping batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Insertion Landscapping...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        land->SetDistance(m_dist);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::ProcessInsert));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Insertion Landscaping...");
      
      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetDistance(m_dist);
      land->ProcessInsert(ctx);
      update = m_world->GetStats().GetUpdate();      
    }
    
    cDataFile& df = m_world->GetDataFile(m_sfilename);
    while (land = m_batch.Pop()) {
      land->PrintStats(df, update);
      if (m_cfilename.GetSize()) land->PrintSiteCount(m_world->GetDataFile(m_cfilename));
      delete land;
    }
  }
};


class cActionPredictWLandscape : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  
public:
  cActionPredictWLandscape(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);

    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Predicting W Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Predicting W Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        land.PredictWProcess(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Predicting W Landscape...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      land.PredictWProcess(ctx, outfile, m_world->GetStats().GetUpdate());
    }
  }
};


class cActionPredictNuLandscape : public cAction  // @not_parallized
{
private:
  cString m_filename;
  
public:
  cActionPredictNuLandscape(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);

    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Predicting Nu Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Predicting Nu Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        land.PredictWProcess(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Predicting Nu Landscape...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      land.PredictWProcess(ctx, outfile, m_world->GetStats().GetUpdate());
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
  cActionRandomLandscape(cWorld* world, const cString& args)
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
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    while (land = m_batch.Pop()) {
      land->PrintStats(df, update);
      delete land;
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
  cActionSampleLandscape(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("land-sample.dat"), m_trials(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_trials = largs.PopWord().AsInt();
    if (m_trials == 0) m_trials = m_world->GetHardwareManager().GetInstSet().GetSize() - 1;
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
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    while (land = m_batch.Pop()) {
      land->PrintStats(df, update);
      delete land;
    }
  }
};


class cActionHillClimb : public cAction  // @not_parallized
{
private:
  cString m_filename;
  
public:
  cActionHillClimb(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Calculating Hill Climb on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        land.HillClimb(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      land.HillClimb(ctx, outfile);
    }
  }
};


class cActionHillClimbNeut : public cAction  // @not_parallized
{
private:
  cString m_filename;
  
public:
  cActionHillClimbNeut(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Calculating Hill Climb on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        land.HillClimb_Neut(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      land.HillClimb_Neut(ctx, outfile);
    }
  }
};


class cActionHillClimbRand : public cAction  // @not_parallized
{
private:
  cString m_filename;
  
public:
  cActionHillClimbRand(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(m_filename);
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Calculating Hill Climb on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        land.HillClimb_Rand(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Calculating Hill Climb...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      land.HillClimb_Rand(ctx, outfile);
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
  cActionMutationalNeighborhood(cWorld* world, const cString& args)
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
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Calculating Mutational Neighborhood for batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Calculating Mutational Neighborhood...");
      }

      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        mutn = new cMutationalNeighborhood(m_world, genotype->GetGenome(), inst_set, m_target);
        m_batch.PushRear(new sBatchEntry(mutn, genotype->GetDepth()));
        jobqueue.AddJob(new tAnalyzeJob<cMutationalNeighborhood>(mutn, &cMutationalNeighborhood::Process));
      }
      jobqueue.Execute();
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Calculating Mutational Neighborhood...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      mutn = new cMutationalNeighborhood(m_world, best_genome, inst_set, m_target);

      m_batch.PushRear(new sBatchEntry(mutn, m_world->GetStats().GetUpdate()));
      mutn->Process(ctx);
    }
    
    cMutationalNeighborhoodResults* results = NULL;
    sBatchEntry* entry = NULL;
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.WriteComment("IMPORTANT: Mutational Neighborhood is *EXPERIMENTAL*");
    df.WriteComment("Output data and format is subject to change in future releases.");
    while (entry = m_batch.Pop()) {
      results = new cMutationalNeighborhoodResults(entry->mutn);
      results->PrintStats(df, entry->depth);
      delete results;
      delete entry;
    }
  }
};


class cActionPairTestLandscape : public cAction  // @not_parallelized
{
private:
  cString m_filename;
  int m_sample_size;
  
public:
  cActionPairTestLandscape(cWorld* world, const cString& args)
  : cAction(world, args), m_filename(""), m_sample_size(0)
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
    cString filename(m_filename);
    if (filename == "") filename.Set("land-pairs-%d.dat", m_world->GetStats().GetUpdate());
    
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    std::ofstream& outfile = m_world->GetDataFileOFStream(filename);
    
    if (ctx.GetAnalyzeMode()) {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_ON) {
        cString msg("Pair Testing Landscape on batch ");
        msg += cStringUtil::Convert(m_world->GetAnalyze().GetCurrentBatchID());
        m_world->GetDriver().NotifyComment(msg);
      } else if (m_world->GetConfig().VERBOSITY.Get() > VERBOSE_SILENT) {
        m_world->GetDriver().NotifyComment("Pair Testing Landscape...");
      }
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape land(m_world, genotype->GetGenome(), inst_set);
        if (m_sample_size) land.TestPairs(ctx, m_sample_size, outfile);
        else land.TestAllPairs(ctx, outfile);
      }
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Pair Testing Landscape...");
      
      const cGenome& best_genome = m_world->GetClassificationManager().GetBestGenotype()->GetGenome();
      cLandscape land(m_world, best_genome, inst_set);
      if (m_sample_size) land.TestPairs(ctx, m_sample_size, outfile);
      else land.TestAllPairs(ctx, outfile);
    }

    m_world->GetDataFileManager().Remove(filename);
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
    inline void PrintLand(cDataFile& df, int update) { if (m_land) m_land->PrintStats(df, update); else df.Endl(); }
    
    void Process(cAvidaContext& ctx)
    {
      if (m_org->GetTestFitness(ctx) > 0.0 && m_cland) {
        m_land = new cLandscape(m_world, m_org->GetGenome(), m_world->GetHardwareManager().GetInstSet());
        m_land->SetDistance(1);
        m_land->Process(ctx);
      }
    }
  };
  
  
public:
  cActionAnalyzePopulation(cWorld* world, const cString& args)
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
      m_world->GetDriver().NotifyWarning("AnalyzePopulation not currently supported in Analyze Mode.");
    } else {
      if (m_world->GetConfig().VERBOSITY.Get() >= VERBOSE_DETAILS)
        m_world->GetDriver().NotifyComment("Analyzing Population...");

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

      cDataFile& df = m_world->GetDataFile(filename);
      while (orgdata = batch.Pop()) {
        cOrganism* organism = orgdata->GetOrganism();
        cGenotype* genotype = organism->GetGenotype();
        cPhenotype& phenotype = organism->GetPhenotype();
        
        cString name;
        if (genotype->GetThreshold()) name = genotype->GetName();
        else name.Set("%03d-no_name-u%i-c%i", genotype->GetLength(), update, orgdata->GetCellID());

        
        df.Write(orgdata->GetCellID(), "Cell ID");
        df.Write(name, "Organism Name");
        df.Write(genotype->GetLength(),"Genome Length");
        df.Write(genotype->GetTestFitness(ctx), "Fitness (test-cpu)");
        df.Write(phenotype.GetFitness(), "Fitness (actual)");
        df.Write(genotype->GetBreedTrue(), "Breed True");
        df.Write(organism->GetLineageLabel(), "Lineage Label");
        df.Write(phenotype.GetNeutralMetric(), "Neutral Metric");
        orgdata->PrintLand(df, update);
        
        // save into archive
        if (m_save_genotypes) {
          name.Set("archive/%s.org", static_cast<const char *>(name));
          cTestUtil::PrintGenome(m_world, organism->GetGenome(), name);
        }
      }
      m_world->GetDataFileManager().Remove(filename);
    }
  }
};



void RegisterLandscapeActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionAnalyzeLandscape>("AnalyzeLandscape");
  action_lib->Register<cActionPrecalcLandscape>("PrecalcLandscape");
  action_lib->Register<cActionFullLandscape>("FullLandscape");
  action_lib->Register<cActionDeletionLandscape>("DeletionLandscape");
  action_lib->Register<cActionInsertionLandscape>("InsertionLandscape");
  action_lib->Register<cActionPredictWLandscape>("PredictWLandscape");
  action_lib->Register<cActionPredictNuLandscape>("PredictNuLandscape");
  action_lib->Register<cActionRandomLandscape>("RandomLandscape");
  action_lib->Register<cActionSampleLandscape>("SampleLandscape");
  action_lib->Register<cActionHillClimb>("HillClimb");
  action_lib->Register<cActionHillClimb>("HillClimbNeut");
  action_lib->Register<cActionHillClimb>("HillClimbRand");
  action_lib->Register<cActionPairTestLandscape>("PairTestLandscape");

  action_lib->Register<cActionMutationalNeighborhood>("MutationalNeighborhood");
  
}
