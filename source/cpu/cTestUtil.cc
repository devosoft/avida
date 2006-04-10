/*
 *  cTestUtil.cc
 *  Avida
 *
 *  Created by David on 11/30/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1999-2003 California Institute of Technology.
 *
 */

#include "cTestUtil.h"

#include "cAvidaContext.h"
#include "cCPUTestInfo.h"
#include "cDataFileManager.h"
#include "cEnvironment.h"
#include "cGenome.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstUtil.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "cStats.h"
#include "cTaskEntry.h"
#include "cTestCPU.h"
#include "cWorld.h"

#include <fstream>
#include <iomanip>

using namespace std;


void cTestUtil::PrintGenome(cWorld* world, const cGenome & genome, cString filename,
			    cGenotype * genotype, int update_out)
{
  if (filename == "") filename.Set("%03d-unnamed", genome.GetSize());

  // Build the test info for printing.
  cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU();
  cAvidaContext& ctx = world->GetDefaultContext();
  
  cCPUTestInfo test_info;
  test_info.TestThreads();
  testcpu->TestGenome(ctx, test_info, genome);
  delete testcpu;

  // Open the file...
  ofstream& fp = world->GetDataFileOFStream(filename);

  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename << "'" << endl;
    return;
  }

  // Print the useful info at the top...

  fp << "# Filename........: " << filename << endl;

  if (update_out >= 0) fp << "# Update Output...: " << update_out << endl;
  else fp << "# Update Output...: N/A" << endl;

  fp << "# Is Viable.......: " << test_info.IsViable() << endl
     << "# Repro Cycle Size: " << test_info.GetMaxCycle()
     << endl
     << "# Depth to Viable.: " << test_info.GetDepthFound()
     << endl;

  if (genotype != NULL) {
    fp << "# Update Created..: " << genotype->GetUpdateBorn()     <<
       endl
       << "# Genotype ID.....: " << genotype->GetID()             <<
       endl
       << "# Parent Gen ID...: " << genotype->GetParentID()       <<
       endl
       << "# Tree Depth......: " << genotype->GetDepth()          <<
       endl
       << "# Parent Distance.: " << genotype->GetParentDistance() <<
       endl
      ;
  }
  fp << endl;

  const int num_levels = test_info.GetMaxDepth() + 1;
  for (int j = 0; j < num_levels; j++) {
    fp << "# Generation: " << j << endl;
    cOrganism * organism = test_info.GetTestOrganism(j);
    assert(organism != NULL);
    cPhenotype & phenotype = organism->GetPhenotype();

    fp << "# Merit...........: "
       << setw(12) << setfill(' ') << phenotype.GetMerit() << endl;
    fp << "# Gestation Time..: "
       << setw(12) << setfill(' ') << phenotype.GetGestationTime() << endl;
    fp << "# Fitness.........: "
       << setw(12) << setfill(' ') << phenotype.GetFitness() << endl;
    fp << "# Errors..........: "
       << setw(12) << setfill(' ') << phenotype.GetLastNumErrors() << endl;
    fp << "# Genome Size.....: "
       << setw(12) << setfill(' ') << organism->GetGenome().GetSize() << endl;
    fp << "# Copied Size.....: "
       << setw(12) << setfill(' ') << phenotype.GetCopiedSize() << endl;
    fp << "# Executed Size...: "
       << setw(12) << setfill(' ') << phenotype.GetExecutedSize() << endl;

    fp << "# Offspring.......: ";
    if (phenotype.GetNumDivides() == 0)
      fp << setw(12) << setfill(' ') << "NONE";
    else if (phenotype.CopyTrue() == true)
      fp << setw(12) << setfill(' ') << "SELF";
    else if (test_info.GetCycleTo() != -1)
      fp << setw(12) << setfill(' ') << test_info.GetCycleTo();
    else
      fp << setw(12) << setfill(' ') << (j+1);
    fp << endl;

    fp << endl;     // Skip line
  }
  
  // Display the tasks performed...
  cPhenotype & phenotype = test_info.GetTestOrganism()->GetPhenotype();
  for (int i = 0; i < world->GetEnvironment().GetTaskLib().GetSize(); i++) {
    fp << "# "<< world->GetEnvironment().GetTaskLib().GetTask(i).GetName()
       << "\t" << phenotype.GetLastTaskCount()[i]
       << endl;
  }
  fp << endl; // Skip line

  // Display the genome
  const cInstSet & inst_set =
    test_info.GetTestOrganism()->GetHardware().GetInstSet();
  cInstUtil::SaveGenome(fp, inst_set, genome);
  
  world->GetDataFileManager().Remove(filename);
}

void cTestUtil::PrintGenome(cWorld* world, cInjectGenotype * inject_genotype, 
			    const cGenome & genome, cString filename, int update_out)
{
  if (filename == "") filename.Set("p%03d-unnamed", genome.GetSize());

  // Build the test info for printing.
  cTestCPU* testcpu = world->GetHardwareManager().CreateTestCPU();
  cAvidaContext& ctx = world->GetDefaultContext();

  cCPUTestInfo test_info;
  test_info.TestThreads();
  testcpu->TestGenome(ctx, test_info, genome);
  delete testcpu;

  // Open the file...
  ofstream& fp = world->GetDataFileOFStream(filename);

  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename << "'" <<
    endl;
    return;
  }

  // Print the useful info at the top...

  fp << "# Filename........: " << filename << endl;

  if (update_out >= 0) fp << "# Update Output...: " << update_out <<
  endl;
  else fp << "# Update Output...: N/A" << endl;

  //fp << "# Is Viable.......: " << test_info.IsViable()
  //<< endl
  //   << "# Repro Cycle Size: " << test_info.GetMaxCycle()
  //   << endl
  //   << "# Depth to Viable.: " << test_info.GetDepthFound()
  //   << endl;

  if (inject_genotype != NULL) {
    fp << "# Update Created..: " << inject_genotype->GetUpdateBorn()     <<
       endl
       << "# Genotype ID.....: " << inject_genotype->GetID()             <<
       endl
       << "# Parent Gen ID...: " << inject_genotype->GetParentID()       <<
       endl
       << "# Tree Depth......: " << inject_genotype->GetDepth()          <<
       endl
      //<< "# Parent Distance.: " << inject_genotype->GetParentDistance() <<
      // endl
      ;
  }
  fp << endl;

  //const int num_levels = test_info.GetMaxDepth() + 1;
  /*for (int j = 0; j < num_levels; j++) {
    fp << "# Generation: " << j << endl;
    cOrganism * organism = test_info.GetTestOrganism(j);
    assert(organism != NULL);
    cPhenotype & phenotype = organism->GetPhenotype();

    fp << "# Merit...........: "
       << setw(12) << setfill(' ') << phenotype.GetMerit() << endl;
    fp << "# Gestation Time..: "
       << setw(12) << setfill(' ') << phenotype.GetGestationTime() << endl;
    fp << "# Fitness.........: "
       << setw(12) << setfill(' ') << phenotype.GetFitness() << endl;
    fp << "# Errors..........: "
       << setw(12) << setfill(' ') << phenotype.GetLastNumErrors() << endl;
    fp << "# Genome Size.....: "
       << setw(12) << setfill(' ') << organism->GetGenome().GetSize() << endl;
    fp << "# Copied Size.....: "
       << setw(12) << setfill(' ') << phenotype.GetCopiedSize() << endl;
    fp << "# Executed Size...: "
       << setw(12) << setfill(' ') << phenotype.GetExecutedSize() << endl;

    fp << "# Offspring.......: ";
    if (phenotype.GetNumDivides() == 0)
      fp << setw(12) << setfill(' ') << "NONE";
    else if (phenotype.CopyTrue() == true)
      fp << setw(12) << setfill(' ') << "SELF";
    else if (test_info.GetCycleTo() != -1)
      fp << setw(12) << setfill(' ') << test_info.GetCycleTo();
    else
      fp << setw(12) << setfill(' ') << (j+1);
    fp << endl;

    fp << endl;     // Skip line
    }
  
  // Display the tasks performed...
  cPhenotype & phenotype = test_info.GetTestOrganism()->GetPhenotype();
  for (int i = 0; i < phenotype.GetEnvironment().GetTaskLib().GetSize(); i++) {
    fp << "# "<< phenotype.GetEnvironment().GetTaskLib().GetTask(i).GetName()
       << "\t" << phenotype.GetLastTaskCount()[i]
       << endl;
  }
  fp << endl; // Skip line
  */
  // Display the genome
  const cInstSet & inst_set =
    test_info.GetTestOrganism()->GetHardware().GetInstSet();
  cInstUtil::SaveGenome(fp, inst_set, genome);
  
  world->GetDataFileManager().Remove(filename);
}
