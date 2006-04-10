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
#include "cWorld.h"


class cActionSampleLandscape : public cAction
{
private:
  cString m_filename;
  int m_sample_size;
  tList<cLandscape> m_batch;

public:
  cActionSampleLandscape(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("land-sample.dat"), m_sample_size(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_sample_size = largs.PopWord().AsInt();
    if (m_sample_size == 0) m_sample_size = m_world->GetHardwareManager().GetInstSet().GetSize() - 1;
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  void Process(cAvidaContext& ctx)
  {
    int update = -1;
    cLandscape* land = NULL;
    
    if (ctx.GetAnalyzeMode()) {
      cAnalyzeJobQueue& jobqueue = m_world->GetAnalyze().GetJobQueue();
      cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
      
      tListIterator<cAnalyzeGenotype> batch_it(m_world->GetAnalyze().GetCurrentBatch().List());
      cAnalyzeGenotype* genotype = NULL;
      while (genotype = batch_it.Next()) {
        cLandscape* land = new cLandscape(m_world, genotype->GetGenome(), inst_set);
        m_batch.PushRear(land);
        jobqueue.AddJob(new tAnalyzeJob<cLandscape>(land, &cLandscape::SampleProcess));
      }
      jobqueue.Execute();
    } else {
      land = new cLandscape(m_world, m_world->GetClassificationManager().GetBestGenotype()->GetGenome(),
                            m_world->GetHardwareManager().GetInstSet());
      m_batch.PushRear(land);
      land->SetTrials(m_sample_size);
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
  action_lib->Register<cActionSampleLandscape>("SAMPLE_LANDSCAPE");
}
