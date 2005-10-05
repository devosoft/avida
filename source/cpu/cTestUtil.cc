//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "cTestUtil.h"

#include "cCPUTestInfo.h"
#include "cEnvironment.h"
#include "cGenome.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cInjectGenotype.h"
#include "cInstUtil.h"
#include "cOrganism.h"
#include "cPhenotype.h"
#include "stats.hh"
#include "task_entry.hh"
#include "cTestCPU.h"

#include <fstream>
#include <iomanip>


using namespace std;


void cTestUtil::PrintGenome(const cGenome & genome, cString filename,
			    cGenotype * genotype, int update_out)
{
  if (filename == "") filename.Set("%03d-unnamed", genome.GetSize());

  // Build the test info for printing.
  cCPUTestInfo test_info;
  test_info.TestThreads();
  cTestCPU::TestGenome(test_info, genome);

  // Open the file...

  ofstream fp(filename());

  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename() << "'" <<
    endl;
    return;
  }

  // Print the useful info at the top...

  fp << "# Filename........: " << filename << endl;

  if (update_out >= 0) fp << "# Update Output...: " << update_out <<
  endl;
  else fp << "# Update Output...: N/A" << endl;

  fp << "# Is Viable.......: " << test_info.IsViable()
  << endl
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
  for (int i = 0; i < phenotype.GetEnvironment().GetTaskLib().GetSize(); i++) {
    fp << "# "<< phenotype.GetEnvironment().GetTaskLib().GetTask(i).GetName()
       << "\t" << phenotype.GetLastTaskCount()[i]
       << endl;
  }
  fp << endl; // Skip line

  // Display the genome
  const cInstSet & inst_set =
    test_info.GetTestOrganism()->GetHardware().GetInstSet();
  cInstUtil::SaveGenome(fp, inst_set, genome);
}

void cTestUtil::PrintGenome(cInjectGenotype * inject_genotype, 
			    const cGenome & genome, cString filename, int update_out)
{
  if (filename == "") filename.Set("p%03d-unnamed", genome.GetSize());

  // Build the test info for printing.
  cCPUTestInfo test_info;
  test_info.TestThreads();
  cTestCPU::TestGenome(test_info, genome);

  // Open the file...

  ofstream fp(filename());

  // @CAO Fix!!!!!!
  if( fp.good() == false ) {
    cerr << "Unable to open output file '" <<  filename() << "'" <<
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
}
