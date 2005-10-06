//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef INJECT_GENEBANK_HH
#include "cInjectGenebank.h"
#endif

#ifndef CONFIG_HH
#include "cConfig.h"
#endif
#ifndef DATA_FILE_HH
#include "data_file.hh"
#endif
#ifndef GENOME_HH
#include "cGenome.h"
#endif
#ifndef INJECT_GENOTYPE_HH
#include "cInjectGenotype.h"
#endif
#ifndef STATS_HH
#include "cStats.h"
#endif
#ifndef TEST_UTIL_HH
#include "cTestUtil.h"
#endif

using namespace std;

////////////////////
//  cInjectGenebank
////////////////////

cInjectGenebank::cInjectGenebank(cStats & in_stats)
  : stats(in_stats)
{
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    inject_genotype_count[i] = 0;
  }

  inject_genotype_control = new cInjectGenotypeControl(*this);

}

cInjectGenebank::~cInjectGenebank()
{
  delete inject_genotype_control;
}

void cInjectGenebank::UpdateReset()
{
  static int genotype_dom_time = 0;
  static int prev_dom = -1;

  cInjectGenotype * best_inject_genotype = GetBestInjectGenotype();

  if (best_inject_genotype && best_inject_genotype->GetID() != prev_dom) {
    genotype_dom_time = 0;
    prev_dom = best_inject_genotype->GetID();
  }
  else {
    genotype_dom_time++;
    if (genotype_dom_time == cConfig::GetGenotypePrintDom()) {
      cString filename;
      filename.Set("genebank/%s", best_inject_genotype->GetName()());
      cTestUtil::PrintGenome(best_inject_genotype, best_inject_genotype->GetGenome(), 
			     filename, stats.GetUpdate());
    }
  }
}

cString cInjectGenebank::GetLabel(int in_size, int in_num)
{
  char alpha[6];
  char full_name[12];
  int i;

  for (i = 4; i >= 0; i--) {
    alpha[i] = (in_num % 26) + 'a';
    in_num /= 26;
  }
  alpha[5] = '\0';

  sprintf(full_name, "p%03d-%s", in_size, alpha);

  return full_name;
}

void cInjectGenebank::AddInjectGenotype(cInjectGenotype * in_inject_genotype, int in_list_num)
{
  assert( in_inject_genotype != 0 );
  
  if ( in_list_num < 0 )
    in_list_num = FindCRC(in_inject_genotype->GetGenome()) % nInjectGenotype::HASH_SIZE;
  
  active_inject_genotypes[in_list_num].Insert(*in_inject_genotype);
  inject_genotype_control->Insert(*in_inject_genotype);
  //stats.AddGenotype(in_inject_genotype->GetID());
}


cInjectGenotype * cInjectGenebank::AddInjectGenotype(const cGenome & in_genome,
				   cInjectGenotype * parent_genotype)
{
  int list_num = FindCRC(in_genome) % nInjectGenotype::HASH_SIZE;
  cInjectGenotype * found_genotype;

  found_genotype = active_inject_genotypes[list_num].Find(in_genome);

  if (!found_genotype) {
    found_genotype = new cInjectGenotype(stats.GetUpdate());
    found_genotype->SetGenome(in_genome);
    found_genotype->SetParent(parent_genotype);
    if(parent_genotype!=NULL)
      {
	parent_genotype->SetCanReproduce();
      }
    AddInjectGenotype( found_genotype, list_num );
  }
  return found_genotype;
}

cInjectGenotype * cInjectGenebank::FindInjectGenotype(const cGenome & in_genome) const
{
  int list_num = FindCRC(in_genome) % nInjectGenotype::HASH_SIZE;
  return active_inject_genotypes[list_num].Find(in_genome);
}

void cInjectGenebank::RemoveInjectGenotype(cInjectGenotype & in_inject_genotype)
{
  // If this genotype is still active, mark it no longer active and
  // take it out of the hash table so it doesn't have any new organisms
  // assigned to it.

  if (in_inject_genotype.GetActive() == true) {
    int list_num = FindCRC(in_inject_genotype.GetGenome()) % nInjectGenotype::HASH_SIZE;
    active_inject_genotypes[list_num].Remove(in_inject_genotype);
    inject_genotype_control->Remove(in_inject_genotype);
    //in_inject_genotype.Deactivate(stats.GetUpdate());
    if (cConfig::GetTrackMainLineage()) {
      inject_genotype_control->InsertHistoric(in_inject_genotype);
    }
  }

  // If we are tracking the main lineage, we only want to delete a
  // genotype when all of its decendents have also died out.

  /*if (cConfig::GetTrackMainLineage()) {
    // If  there are more offspring genotypes, hold off on deletion...
    if (in_inject_genotype.GetNumOffspringGenotypes() != 0) return;

    // If this is a dead end, delete it and recurse up...
    cInjectGenotype * parent = in_inject_genotype.GetParentGenotype();
    if (parent != NULL) {
      parent->RemoveOffspringGenotype();

      // Test to see if we need to update the coalescent genotype.
      const int new_coal = inject_genotype_control->UpdateCoalescent();
      stats.SetCoalescentGenotypeDepth(new_coal);
      // cout << "Set coalescent to " << found_gen->GetDepth() << endl;

      if (parent->GetNumInjected() == 0) {
	// Regardless, run RemoveGenotype on the parent.
	RemoveGenotype(*parent);
      }
    }

    inject_genotype_control->RemoveHistoric(in_inject_genotype);
  }

  // Handle the relevent statistics...
  stats.RemoveGenotype(in_inject_genotype.GetID(),
	      in_inject_genotype.GetParentID(), in_inject_genotype.GetParentDistance(),
	      in_inject_genotype.GetDepth(), in_inject_genotype.GetTotalOrganisms(),
              in_inject_genotype.GetTotalParasites(),
	      stats.GetUpdate() - in_inject_genotype.GetUpdateBorn(),
              in_inject_genotype.GetLength());
  if (in_inject_genotype.GetThreshold()) {
  stats.RemoveThreshold(in_inject_genotype.GetID());
  }*/

  delete &in_inject_genotype;
}

void cInjectGenebank::ThresholdInjectGenotype(cInjectGenotype & in_inject_genotype)
{
  in_inject_genotype.SetName( GetLabel(in_inject_genotype.GetLength(),
				inject_genotype_count[in_inject_genotype.GetLength()]++) );
  in_inject_genotype.SetThreshold();

  //stats.AddThreshold(in_inject_genotype.GetID(), in_inject_genotype.GetName()());
  
  // Print the genotype?

  if (cConfig::GetGenotypePrint()) {
    cString filename;
    filename.Set("genebank/%s", in_inject_genotype.GetName()());
    //cTestUtil::PrintGenome(in_inject_genotype.GetGenome(), filename,
    //			   &in_inject_genotype, stats.GetUpdate());
  }
}

bool cInjectGenebank::AdjustInjectGenotype(cInjectGenotype & in_inject_genotype)
{
  if (!inject_genotype_control->Adjust(in_inject_genotype)) return false;

  if ((in_inject_genotype.GetNumInjected() >= cConfig::GetThreshold() ||
       &in_inject_genotype == inject_genotype_control->GetBest()) &&
      !(in_inject_genotype.GetThreshold())) {
    ThresholdInjectGenotype(in_inject_genotype);
  }

  return true;
}

bool cInjectGenebank::SaveClone(ofstream & fp)
{
  // This method just save the counts at each size-class of genotypes.
  // The rest is reconstructable.

  // Save the numbers of organisms we're up to at each size.
  fp << MAX_CREATURE_SIZE << " ";
  for (int i = 0; i < MAX_CREATURE_SIZE; i++) {
    fp << inject_genotype_count[i] << " ";
  }

  return true;
}

bool cInjectGenebank::LoadClone(ifstream & fp)
{
  // This method just restores the counts at each size-class of genotypes.
  // The rest of the loading process should be handled elsewhere.

  // Load the numbers of organisms we're up to at each size.
  int max_size;
  fp >> max_size;
  assert (max_size <= MAX_CREATURE_SIZE); // MAX_CREATURE_SIZE too small
  for (int i = 0; i < max_size && i < MAX_CREATURE_SIZE; i++) {
    fp >> inject_genotype_count[i];
  }

  return true;
}

bool cInjectGenebank::DumpTextSummary(ofstream & fp)
{
  inject_genotype_control->Reset(0);
  for (int i = 0; i < inject_genotype_control->GetSize(); i++) {
    cInjectGenotype * genotype = inject_genotype_control->Get(0);
    fp << genotype->GetGenome().AsString() << " "
       << genotype->GetNumInjected() << " "
       << genotype->GetID() << endl;
    inject_genotype_control->Next(0);
  }

  return true;
}

bool cInjectGenebank::DumpDetailedSummary(const cString & file, int update)
{
  inject_genotype_control->Reset(0);
  for (int i = 0; i < inject_genotype_control->GetSize(); i++) {
    DumpDetailedEntry(inject_genotype_control->Get(0), file, update);
    inject_genotype_control->Next(0);
  }
  return true;
}

/*bool cInjectGenebank::DumpHistoricSummary(ofstream & fp)
{
  inject_genotype_control->ResetHistoric(0);
  for (int i = 0; i < inject_genotype_control->GetHistoricCount(); i++) {
    DumpDetailedEntry(inject_genotype_control->Get(0), fp);
    inject_genotype_control->Next(0);
  }

  return true;
}*/

void cInjectGenebank::DumpDetailedEntry(cInjectGenotype * genotype, const cString & filename, int update)
{
  //if(genotype->CanReproduce())
  //  {
      cDataFile & df = stats.GetDataFile(filename);
      
      df.WriteComment( "Avida parasite dump data" );
      df.WriteTimeStamp();
      
      df.Write( genotype->GetID(),                 "parasite genotype ID");
      df.Write( genotype->GetName(),              "parasite genotype name");
      df.Write( genotype->GetParentID(),           "parasite parent ID");
      df.Write( genotype->GetNumInjected(),        "current number of injected creatures with this genotype");
      df.Write( genotype->GetTotalInjected(),      "total number of injected creatures with this genotype");
      df.Write( genotype->GetLength(),             "genotype length");
      df.Write( genotype->GetUpdateBorn(),         "update this genotype was born");
      df.Write( genotype->CanReproduce(),          "has this genotype reproduced?");
      df.Write( genotype->GetGenome().AsString(),  "genome of this genotype");
      df.Endl();
      //}
}

bool cInjectGenebank::OK()
{
  bool ret_value = true;
  int i;

  // Check components...

  if (!inject_genotype_control->OK()) {
    ret_value = false;
  }

  // Loop through all of the reference lists for matching genotypes...

  for (i = 0; i < nInjectGenotype::HASH_SIZE; i++) {
    assert (active_inject_genotypes[i].OK());
  }

  assert (ret_value == true);

  return ret_value;
}

int cInjectGenebank::CountNumCreatures()
{
  int i;
  int total = 0;

  inject_genotype_control->Reset(0);
  for (i = 0; i < inject_genotype_control->GetSize(); i++) {
    total += inject_genotype_control->Get(0)->GetNumInjected();
    inject_genotype_control->Next(0);
  }

  return total;
}


unsigned int cInjectGenebank::FindCRC(const cGenome & in_genome) const
{
  unsigned int total = 13;
  int i;

  for (i = 0; i < in_genome.GetSize(); i++) {
    total *= in_genome[i].GetOp() + 10 + i << 6;
    total += 3;
  }

  return total;
}

