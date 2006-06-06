/*
 *  PrintActions.cc
 *  Avida
 *
 *  Created by David on 5/11/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "PrintActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cAnalyzeUtil.h"
#include "cClassificationManager.h"
#include "cCPUTestInfo.h"
#include "cGenome.h"
#include "cGenomeUtil.h"
#include "cGenotype.h"
#include "cHardwareBase.h"
#include "cHardwareManager.h"
#include "cInjectGenotype.h"
#include "cInstSet.h"
#include "cInstUtil.h"
#include "cOrganism.h"
#include "cPopulation.h"
#include "cPopulationCell.h"
#include "cSpecies.h"
#include "cStats.h"
#include "cTestUtil.h"
#include "cWorld.h"
#include "cWorldDriver.h"


#define STATS_OUT_FILE(METHOD, DEFAULT)                                                   /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
  cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
  cAction ## METHOD(cWorld* world, const cString& args) : cAction(world, args)            /*  6 */ \
  {                                                                                       /*  7 */ \
    cString largs(args);                                                                  /*  8 */ \
    if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
  }                                                                                       /* 10 */ \
  const cString GetDescription() { return #METHOD " [cString fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
  void Process(cAvidaContext& ctx) { m_world->GetStats().METHOD(m_filename); }            /* 12 */ \
}                                                                                         /* 13 */ \

STATS_OUT_FILE(PrintAverageData,       average.dat         );
STATS_OUT_FILE(PrintErrorData,         error.dat           );
STATS_OUT_FILE(PrintVarianceData,      variance.dat        );
STATS_OUT_FILE(PrintDominantData,      dominant.dat        );
STATS_OUT_FILE(PrintStatsData,         stats.dat           );
STATS_OUT_FILE(PrintCountData,         count.dat           );
STATS_OUT_FILE(PrintTotalsData,        totals.dat          );
STATS_OUT_FILE(PrintTasksData,         tasks.dat           );
STATS_OUT_FILE(PrintTasksExeData,      tasks_exe.dat       );
STATS_OUT_FILE(PrintTasksQualData,     tasks_quality.dat   );
STATS_OUT_FILE(PrintResourceData,      resource.dat        );
STATS_OUT_FILE(PrintTimeData,          time.dat            );
STATS_OUT_FILE(PrintMutationData,      mutation.dat        );
STATS_OUT_FILE(PrintMutationRateData,  mutation_rates.dat  );
STATS_OUT_FILE(PrintDivideMutData,     divide_mut.dat      );
STATS_OUT_FILE(PrintDominantParaData,  parasite.dat        );
STATS_OUT_FILE(PrintInstructionData,   instruction.dat     );
STATS_OUT_FILE(PrintGenotypeMap,       genotype_map.m      );

#define POP_OUT_FILE(METHOD, DEFAULT)                                                     /*  1 */ \
class cAction ## METHOD : public cAction {                                                /*  2 */ \
private:                                                                                  /*  3 */ \
  cString m_filename;                                                                     /*  4 */ \
public:                                                                                   /*  5 */ \
  cAction ## METHOD(cWorld* world, const cString& args) : cAction(world, args)            /*  6 */ \
  {                                                                                       /*  7 */ \
    cString largs(args);                                                                  /*  8 */ \
    if (largs == "") m_filename = #DEFAULT; else m_filename = largs.PopWord();            /*  9 */ \
  }                                                                                       /* 10 */ \
  const cString GetDescription() { return #METHOD " [cString fname=\"" #DEFAULT "\"]"; }  /* 11 */ \
  void Process(cAvidaContext& ctx) { m_world->GetPopulation().METHOD(m_filename); }       /* 12 */ \
}                                                                                         /* 13 */ \

POP_OUT_FILE(PrintPhenotypeData,       phenotype_count.dat );
POP_OUT_FILE(PrintPhenotypeStatus,     phenotype_status.dat);


class cActionPrintData : public cAction
{
private:
  cString m_filename;
  cString m_format;
public:
  cActionPrintData(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    m_filename = largs.PopWord();
    m_format = largs.PopWord();
  }
  
  const cString GetDescription() { return "PringData <cString fname> <cString format>"; }

  void Process(cAvidaContext& ctx)
  {
    m_world->GetStats().PrintDataFile(m_filename, m_format, ',');
  }
};


class cActionPrintInstructionAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintInstructionAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "instruction_histogram.dat"; else m_filename = largs.PopWord();
  }

  const cString GetDescription() { return "PrintInstructionAbundanceHistogram [cString fname=\"instruction_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& population = m_world->GetPopulation();
    
    // ----- number of instructions available?
    const int num_inst = m_world->GetNumInstructions();
    tArray<int> inst_counts(num_inst);
    inst_counts.SetAll(0);

    //looping through all CPUs counting up instructions
    const int num_cells = population.GetSize();
    for (int x = 0; x < num_cells; x++) {
      cPopulationCell& cell = population.GetCell(x);
      if (cell.IsOccupied()) {
        // access this CPU's code block
        cCPUMemory& cpu_mem = cell.GetOrganism()->GetHardware().GetMemory();
        const int mem_size = cpu_mem.GetSize();
        for (int y = 0; y < mem_size; y++) inst_counts[cpu_mem[y].GetOp()]++;     
      }
    }
    
    // ----- output instruction counts
    cInstSet& inst_set = m_world->GetHardwareManager().GetInstSet();
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < num_inst; i++) df.Write(inst_counts[i], inst_set.GetName(i));
    df.Endl();
  }
};


class cActionPrintDepthHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintDepthHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "depth_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintDepthHistogram [cString fname=\"depth_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // Output format:    update  min  max  histogram_values...
    int min = INT_MAX;
    int max = 0;
    
    // Two pass method
    
    // Loop through all genotypes getting min and max values
    cClassificationManager& classmgr = m_world->GetClassificationManager();
    cGenotype* cur_genotype = classmgr.GetBestGenotype();
    for (int i = 0; i < classmgr.GetGenotypeCount(); i++) {
      if (cur_genotype->GetDepth() < min) min = cur_genotype->GetDepth();
      if (cur_genotype->GetDepth() > max) max = cur_genotype->GetDepth();
      cur_genotype = cur_genotype->GetNext();
    }
    assert(max >= min);
    
    // Allocate the array for the bins (& zero)
    tArray<int> n(max - min + 1);
    n.SetAll(0);
    
    // Loop through all genotypes binning the values
    cur_genotype = classmgr.GetBestGenotype();
    for (int i = 0; i < classmgr.GetGenotypeCount(); i++) {
      n[cur_genotype->GetDepth() - min] += cur_genotype->GetNumOrganisms();
      cur_genotype = cur_genotype->GetNext();
    }
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(min, "Minimum");
    df.Write(max, "Maximum");
    for (int i = 0; i < n.GetSize(); i++)  df.Write(n[i], "");
    df.Endl();
  }
};


class cActionEcho : public cAction
{
private:
  cString m_filename;
public:
  cActionEcho(cWorld* world, const cString& args) : cAction(world, args) { ; }
  
  const cString GetDescription() { return "Echo <cString message>"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_args == "") {
      m_args.Set("Echo : Update = %f\t AveGeneration = %f", m_world->GetStats().GetUpdate(),
                 m_world->GetStats().SumGeneration().Average());
    }
    m_world->GetDriver().NotifyComment(m_args);
  }
};


class cActionPrintGenotypeAbundanceHistogram : public cAction
{
private:
  cString m_filename;
  
public:
  cActionPrintGenotypeAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "genotype_abundance_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintGenotypeAbundanceHistogram [cString fname=\"genotype_abundance_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // Allocate array for the histogram & zero it
    tArray<int> hist(m_world->GetClassificationManager().GetBestGenotype()->GetNumOrganisms());
    hist.SetAll(0);
    
    // Loop through all genotypes binning the values
    cGenotype* cur_genotype = m_world->GetClassificationManager().GetBestGenotype();
    for (int i = 0; i < m_world->GetClassificationManager().GetGenotypeCount(); i++) {
      assert( cur_genotype->GetNumOrganisms() - 1 >= 0 );
      assert( cur_genotype->GetNumOrganisms() - 1 < hist.GetSize() );
      hist[cur_genotype->GetNumOrganisms() - 1]++;
      cur_genotype = cur_genotype->GetNext();
    }
    
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < hist.GetSize(); i++) df.Write(hist[i],"");
    df.Endl();
  }
};


class cActionPrintSpeciesAbundanceHistogram : public cAction
{
private:
  cString m_filename;
public:
  cActionPrintSpeciesAbundanceHistogram(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
    if (largs == "") m_filename = "species_abundance_histogram.dat"; else m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintSpeciesAbundanceHistogram [cString fname=\"species_abundance_histogram.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    int max = 0;
    
    // Find max species abundance...
    cClassificationManager& classmgr = m_world->GetClassificationManager();
    cSpecies* cur_species = classmgr.GetFirstSpecies();
    for (int i = 0; i < classmgr.GetNumSpecies(); i++) {
      if (max < cur_species->GetNumOrganisms()) {
        max = cur_species->GetNumOrganisms();
      }
      cur_species = cur_species->GetNext();
    }
    
    // Allocate array for the histogram & zero it
    tArray<int> hist(max);
    hist.SetAll(0);
    
    // Loop through all species binning the values
    cur_species = classmgr.GetFirstSpecies();
    for (int i = 0; i < classmgr.GetNumSpecies(); i++) {
      assert( cur_species->GetNumOrganisms() - 1 >= 0 );
      assert( cur_species->GetNumOrganisms() - 1 < hist.GetSize() );
      hist[cur_species->GetNumOrganisms() - 1]++;
      cur_species = cur_species->GetNext();
    }
    
    // Actual output
    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    for (int i = 0; i < hist.GetSize(); i++) df.Write(hist[i], "");
    df.Endl();
  }
};

class cActionPrintLineageTotals : public cAction
{
private:
  cString m_filename;
  int m_verbose;
public:
  cActionPrintLineageTotals(cWorld* world, const cString& args) : cAction(world, args), m_verbose(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord(); else m_filename = "lineage_totals.dat";
    if (largs.GetSize()) m_verbose = largs.PopWord().AsInt();
  }
  
  const cString GetDescription() { return "PrintLineageTotals [cString fname='lineage_totals.dat'] [int verbose=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
      m_world->GetDataFileOFStream(m_filename) << "No lineage data available!" << endl;
      return;
    }
    m_world->GetClassificationManager().PrintLineageTotals(m_filename, m_verbose);
  }
};

class cActionPrintLineageCounts : public cAction
{
private:
  cString m_filename;
  int m_verbose;
public:
  cActionPrintLineageCounts(cWorld* world, const cString& args) : cAction(world, args), m_verbose(1)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord(); else m_filename = "lineage_counts.dat";
    if (largs.GetSize()) m_verbose = largs.PopWord().AsInt();
  }
  
  const cString GetDescription() { return "PrintLineageCounts [cString fname='lineage_counts.dat'] [int verbose=1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (!m_world->GetConfig().LOG_LINEAGES.Get()) {
      m_world->GetDataFileOFStream(m_filename) << "No lineage data available!" << endl;
      return;
    }
    if (m_verbose) {    // verbose mode is the same in both methods
      m_world->GetClassificationManager().PrintLineageTotals(m_filename, m_verbose);
      return;
    }
    m_world->GetClassificationManager().PrintLineageCurCounts(m_filename);
  }
};


/*
 Write the currently dominant genotype to disk.

 Parameters:
   filename (string)
     The name under which the genotype should be saved. If no
     filename is given, the genotype is saved into the directory
     archive, under the name that the archive has associated with
     this genotype.
*/
class cActionPrintDominantGenotype : public cAction
{
private:
  cString m_filename;

public:
  cActionPrintDominantGenotype(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintDominantGenotype [cString fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cGenotype* dom = m_world->GetClassificationManager().GetBestGenotype();
    cString filename(m_filename);
    if (filename == "") filename.Set("archive/%s.org", static_cast<const char*>(dom->GetName()));
    cTestUtil::PrintGenome(m_world, dom->GetGenome(), filename, dom, m_world->GetStats().GetUpdate());
  }
};

/*
 Write the currently dominant injected genotype to disk.
 
 Parameters:
   filename (string)
     The name under which the genotype should be saved. If no
     filename is given, the genotype is saved into the directory
     archive, under the name that the archive has associated with
     this genotype.
*/
class cActionPrintDominantParasiteGenotype : public cAction
{
private:
  cString m_filename;

public:
  cActionPrintDominantParasiteGenotype(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintDominantParasiteGenotype [cString fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cInjectGenotype* dom = m_world->GetClassificationManager().GetBestInjectGenotype();
    if (dom != NULL) {
      cString filename(m_filename);
      if (filename == "") filename.Set("archive/%s.para", static_cast<const char*>(dom->GetName()));
      cTestUtil::PrintGenome(m_world, dom, dom->GetGenome(), filename, m_world->GetStats().GetUpdate());
    }
  }
};

// This is a generic place for Developers to hook into an action for printing out debug information
class cActionPrintDebug : public cAction
{
public:
  cActionPrintDebug(cWorld* world, const cString& args) : cAction(world, args) { ; }
  const cString GetDescription() { return "PrintDebug"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().ParasiteDebug();
  }
};


/*
 This is a new version of "detail_pop" or "historic_dump".  It allows you to
 output one line per genotype in memory where you get to choose what data
 should be included.
 
 Parameters
   data_fields (string)
     This must be a comma separated string of all data you wish to output.
     Options include: id, parent_id, parent2_id (for sex), parent_dist,
       num_cpus, total_cpus, length, merit, gest_time, fitness, update_born,
       update_dead, depth, lineage, sequence
   historic (int) default: 0
     How many updates back of history should we include (-1 = all)
   filename (string) default: "genotypes-<update>.dat"
     The name of the file into which the population dump should be written.
*/
class cActionPrintGenotypes : public cAction
{
private:
  cString m_datafields;
  cString m_filename;
  int m_historic;
  
public:
  cActionPrintGenotypes(cWorld* world, const cString& args)
    : cAction(world, args), m_datafields("all"), m_filename(""), m_historic(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_datafields = largs.PopWord();
    if (largs.GetSize()) m_historic = largs.PopWord().AsInt();
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintGenotypes [cString data_fields=\"all\"] [int historic=0] [cString fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("genotypes-%d.dat", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().PrintGenotypes(m_world->GetDataFileOFStream(filename),
                                                       m_datafields, m_historic);
    m_world->GetDataFileManager().Remove(filename);
  }
};


/*
 This function prints out fitness data. The main point is that it
 calculates the average fitness from info from the testCPU + the actual
 merit of the organisms, and assigns zero fitness to those organisms
 that will never reproduce.
 
 The function also determines the maximum fitness genotype, and can
 produce fitness histograms.
 
 Parameters
   datafn (cString)
     Where the fitness data should be written.
   histofn (cString)
     Where the fitness histogram should be written.
   histotestfn (cString)
     Where the fitness histogram as determined exclusively from the test-CPU should be written.
   save_max_f_genotype (bool)
     Whether the genotype with the maximum fitness should be saved into the classmgr.
   print_fitness_histo (bool)
     Determines whether fitness histograms should be written.
   hist_fmax (double)
     The maximum fitness value to be taken into account for the fitness histograms.
   hist_fstep (double)
     The width of the individual bins in the fitness histograms.
*/
class cActionPrintDetailedFitnessData : public cAction
{
private:
  int m_save_max;
  int m_print_fitness_histo;
  double m_hist_fmax;
  double m_hist_fstep;
  cString m_filenames[3];

public:
  cActionPrintDetailedFitnessData(cWorld* world, const cString& args)
    : cAction(world, args), m_save_max(0), m_print_fitness_histo(0), m_hist_fmax(1.0), m_hist_fstep(0.1)
  {
    cString largs(args);
    if (largs.GetSize()) m_save_max = largs.PopWord().AsInt();
    if (largs.GetSize()) m_print_fitness_histo = largs.PopWord().AsInt();
    if (largs.GetSize()) m_hist_fmax = largs.PopWord().AsDouble();
    if (largs.GetSize()) m_hist_fstep = largs.PopWord().AsDouble();
    if (!largs.GetSize()) m_filenames[0] = "fitness.dat"; else m_filenames[0] = largs.PopWord();
    if (!largs.GetSize()) m_filenames[1] = "fitness_histos.dat"; else m_filenames[1] = largs.PopWord();
    if (!largs.GetSize()) m_filenames[2] = "fitness_histos_testCPU.dat"; else m_filenames[2] = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintDetailedFitnessData [int save_max_f_genotype=0] [int print_fitness_histo=0] [double hist_fmax=1] [double hist_fstep=0.1] [cString datafn=\"fitness.dat\"] [cString histofn=\"fitness_histos.dat\"] [cString histotestfn=\"fitness_histos_testCPU.dat\"]"; }
  
  void Process(cAvidaContext& ctx)
  {
    cPopulation& pop = m_world->GetPopulation();
    const int update = m_world->GetStats().GetUpdate();
    const double generation = m_world->GetStats().SumGeneration().Average();
    
    // the histogram variables
    tArray<int> histo;
    tArray<int> histo_testCPU;
    int bins = 0;
    
    if (m_print_fitness_histo) {
      bins = static_cast<int>(m_hist_fmax / m_hist_fstep) + 1;
      histo.Resize(bins, 0);
      histo_testCPU.Resize(bins, 0 );
    }
    
    int n = 0;
    int nhist_tot = 0;
    int nhist_tot_testCPU = 0;
    double fave = 0;
    double fave_testCPU = 0;
    double max_fitness = -1; // we set this to -1, so that even 0 is larger...
    cGenotype* max_f_genotype = NULL;
    
    cTestCPU* testcpu = m_world->GetHardwareManager().CreateTestCPU();

    for (int i = 0; i < pop.GetSize(); i++) {
      if (pop.GetCell(i).IsOccupied() == false) continue;  // One use organisms.
      
      cOrganism* organism = pop.GetCell(i).GetOrganism();
      cGenotype* genotype = organism->GetGenotype();
      
      cCPUTestInfo test_info;
      testcpu->TestGenome(ctx, test_info, genotype->GetGenome());
      // We calculate the fitness based on the current merit,
      // but with the true gestation time. Also, we set the fitness
      // to zero if the creature is not viable.
      const double f = (test_info.IsViable()) ? organism->GetPhenotype().GetMerit().CalcFitness(test_info.GetTestPhenotype().GetGestationTime()) : 0;
      const double f_testCPU = test_info.GetColonyFitness();
      
      // Get the maximum fitness in the population
      // Here, we want to count only organisms that can truly replicate,
      // to avoid complications
      if (f_testCPU > max_fitness && test_info.GetTestPhenotype().CopyTrue()) {
        max_fitness = f_testCPU;
        max_f_genotype = genotype;
      }
      
      fave += f;
      fave_testCPU += f_testCPU;
      n += 1;
      
      
      // histogram
      if (m_print_fitness_histo && f < m_hist_fmax) {
        histo[static_cast<int>(f / m_hist_fstep)] += 1;
        nhist_tot += 1;
      }
      
      if (m_print_fitness_histo && f_testCPU < m_hist_fmax) {
        histo_testCPU[static_cast<int>(f_testCPU / m_hist_fstep)] += 1;
        nhist_tot_testCPU += 1;
      }
    }
    
    delete testcpu;
    
    // determine the name of the maximum fitness genotype
    cString max_f_name;
    if (max_f_genotype->GetThreshold())
      max_f_name = max_f_genotype->GetName();
    else // we put the current update into the name, so that it becomes unique.
      max_f_name.Set("%03d-no_name-u%i", max_f_genotype->GetLength(), update);
    
    cDataFile& df = m_world->GetDataFile(m_filenames[0]);
    df.Write(update, "Update");
    df.Write(generation, "Generation");
    df.Write(fave / static_cast<double>(n), "Average Fitness");
    df.Write(fave_testCPU / static_cast<double>(n), "Average Test Fitness");
    df.Write(n, "Organism Total");
    df.Write(max_fitness, "Maximum Fitness");
    df.Write(max_f_name, "Maxfit genotype name");
    df.Endl();
    
    if (m_save_max) {
      cString filename;
      filename.Set("classmgr/%s", static_cast<const char*>(max_f_name));
      cTestUtil::PrintGenome(m_world, max_f_genotype->GetGenome(), filename);
    }
    
    if (m_print_fitness_histo) {
      cDataFile& hdf = m_world->GetDataFile(m_filenames[1]);
      hdf.Write(update, "Update");
      hdf.Write(generation, "Generation");
      hdf.Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo.GetSize(); i++)
        hdf.Write(static_cast<double>(histo[i]) / static_cast<double>(nhist_tot), "");
      hdf.Endl();
      
      
      cDataFile& tdf = m_world->GetDataFile(m_filenames[2]);
      tdf.Write(update, "Update");
      tdf.Write(generation, "Generation");
      tdf.Write(fave / static_cast<double>(n), "Average Fitness");
      
      // now output the fitness histo
      for (int i = 0; i < histo_testCPU.GetSize(); i++)
        tdf.Write(static_cast<double>(histo_testCPU[i]) / static_cast<double>(nhist_tot_testCPU), "");
      tdf.Endl();
    }
  }
};


/*
 This function goes through all genotypes currently present in the soup,
 and writes into an output file the average Hamming distance between the
 creatures in the population and a given reference genome.
 
 Parameters
   ref_creature_file (cString)
     Filename for the reference genome, defaults to START_CREATURE
   fname (cString)
     Name of file to create, defaults to 'genetic_distance.dat'
*/
class cActionPrintGeneticDistanceData : public cAction
{
private:
  cGenome m_reference;
  cString m_filename;
  
public:
  cActionPrintGeneticDistanceData(cWorld* world, const cString& args)
    : cAction(world, args), m_filename("genetic_distance.dat")
  {
    cString creature_file;
    cString largs(args);
    
    // Load the genome of the reference creature
    if (largs.GetSize())
      creature_file = largs.PopWord();
    else
      creature_file = m_world->GetConfig().START_CREATURE.Get();
    m_reference = cInstUtil::LoadGenome(creature_file, world->GetHardwareManager().GetInstSet());
    
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintGeneticDistanceData [cString ref_creature_file='START_CREATURE'] [cString fname='genetic_distance.dat']"; }
  
  void Process(cAvidaContext& ctx)
  {
    double hamming_m1 = 0;
    double hamming_m2 = 0;
    int count = 0;
    int dom_dist = 0;
    
    // get the info for the dominant genotype
    cGenotype* cur_genotype = m_world->GetClassificationManager().GetBestGenotype();
    cGenome genome = cur_genotype->GetGenome();
    dom_dist = cGenomeUtil::FindHammingDistance(m_reference, genome);
    hamming_m1 += dom_dist;
    hamming_m2 += dom_dist*dom_dist;
    count += cur_genotype->GetNumOrganisms();
    // now cycle over the remaining genotypes
    for (int i = 1; i < m_world->GetClassificationManager().GetGenotypeCount(); i++) {
      cur_genotype = cur_genotype->GetNext();
      cGenome genome = cur_genotype->GetGenome();
      
      int dist = cGenomeUtil::FindHammingDistance(m_reference, genome);
      hamming_m1 += dist;
      hamming_m2 += dist*dist;
      count += cur_genotype->GetNumOrganisms();
    }
    
    hamming_m1 /= static_cast<double>(count);
    hamming_m2 /= static_cast<double>(count);

    double hamming_best = cGenomeUtil::FindHammingDistance(m_reference, m_world->GetClassificationManager().GetBestGenotype()->GetGenome());

    cDataFile& df = m_world->GetDataFile(m_filename);
    df.Write(m_world->GetStats().GetUpdate(), "Update");
    df.Write(hamming_m1, "Average Hamming Distance");
    df.Write(sqrt((hamming_m2 - hamming_m1*hamming_m1) / static_cast<double>(count)), "Standard Error");
    df.Write(hamming_best, "Best Genotype Hamming Distance");
    df.Endl();
  }
};


class cActionDumpMemory : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpMemory(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  const cString GetDescription() { return "PrintDumpMemory [cString fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("memory_dump-%d.dat", m_world->GetStats().GetUpdate());
    m_world->GetPopulation().DumpMemorySummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


void RegisterPrintActions(cActionLibrary* action_lib)
{
  // Stats Out Files
  action_lib->Register<cActionPrintAverageData>("PrintAverageData");
  action_lib->Register<cActionPrintErrorData>("PrintErrorData");
  action_lib->Register<cActionPrintVarianceData>("PrintVarianceData");
  action_lib->Register<cActionPrintDominantData>("PrintDominantData");
  action_lib->Register<cActionPrintStatsData>("PrintStatsData");
  action_lib->Register<cActionPrintCountData>("PrintCountData");
  action_lib->Register<cActionPrintTotalsData>("PrintTotalsData");
  action_lib->Register<cActionPrintTasksData>("PrintTasksData");
  action_lib->Register<cActionPrintTasksExeData>("PrintTasksExeData");
  action_lib->Register<cActionPrintTasksQualData>("PrintTasksQualData");
  action_lib->Register<cActionPrintResourceData>("PrintResourceData");
  action_lib->Register<cActionPrintTimeData>("PrintTimeData");
  action_lib->Register<cActionPrintMutationData>("PrintMutationData");
  action_lib->Register<cActionPrintMutationRateData>("PrintMutationRateData");
  action_lib->Register<cActionPrintDivideMutData>("PrintDivideMutData");
  action_lib->Register<cActionPrintDominantParaData>("PrintDominantParaData");
  action_lib->Register<cActionPrintInstructionData>("PrintInstructionData");
  action_lib->Register<cActionPrintGenotypeMap>("PrintGenotypeMap");
  
  // Population Out Files
  action_lib->Register<cActionPrintPhenotypeData>("PrintPhenotypeData");
  action_lib->Register<cActionPrintPhenotypeStatus>("PrintPhenotypeStatus");
  
  // Processed Data
  action_lib->Register<cActionPrintData>("PrintData");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("PrintInstructionAbundanceHistogram");
  action_lib->Register<cActionPrintDepthHistogram>("PrintDepthHistogram");
  action_lib->Register<cActionEcho>("Echo");
  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("PrintGenotypeAbundanceHistogram");
  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("PrintSpeciesAbundanceHistogram");
  action_lib->Register<cActionPrintLineageTotals>("PrintLineageTotals");
  action_lib->Register<cActionPrintLineageCounts>("PrintLineageCounts");
  action_lib->Register<cActionPrintDominantGenotype>("PrintDominantGenotype");
  action_lib->Register<cActionPrintDominantParasiteGenotype>("PrintDominantParasiteGenotype");
  action_lib->Register<cActionPrintDetailedFitnessData>("PrintDetailedFitnessData");
  action_lib->Register<cActionPrintGeneticDistanceData>("PrintGenericDistanceData");
  action_lib->Register<cActionPrintDebug>("PrintDebug");

  action_lib->Register<cActionPrintGenotypes>("PrintGenotypes");
  action_lib->Register<cActionDumpMemory>("DumpMemory");


  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionPrintAverageData>("print_average_data");
  action_lib->Register<cActionPrintErrorData>("print_error_data");
  action_lib->Register<cActionPrintVarianceData>("print_variance_data");
  action_lib->Register<cActionPrintDominantData>("print_dominant_data");
  action_lib->Register<cActionPrintStatsData>("print_stats_data");
  action_lib->Register<cActionPrintCountData>("print_count_data");
  action_lib->Register<cActionPrintTotalsData>("print_totals_data");
  action_lib->Register<cActionPrintTasksData>("print_tasks_data");
  action_lib->Register<cActionPrintTasksExeData>("print_tasks_exe_data");
  action_lib->Register<cActionPrintTasksQualData>("print_tasks_qual_data");
  action_lib->Register<cActionPrintResourceData>("print_resource_data");
  action_lib->Register<cActionPrintTimeData>("print_time_data");
  action_lib->Register<cActionPrintMutationData>("print_mutation_dat");
  action_lib->Register<cActionPrintMutationRateData>("print_mutation_rate_data");
  action_lib->Register<cActionPrintDivideMutData>("print_divide_mut_data");
  action_lib->Register<cActionPrintDominantParaData>("print_dom_parasite_data");
  action_lib->Register<cActionPrintInstructionData>("print_instruction_data");
  action_lib->Register<cActionPrintGenotypeMap>("print_genotype_map");
  
  action_lib->Register<cActionPrintPhenotypeData>("print_number_phenotypes");
  action_lib->Register<cActionPrintPhenotypeStatus>("print_phenotype_status");
  
  action_lib->Register<cActionPrintData>("print_data");
  action_lib->Register<cActionPrintInstructionAbundanceHistogram>("print_instruction_abundance_histogram");
  action_lib->Register<cActionPrintDepthHistogram>("print_depth_histogram");
  action_lib->Register<cActionEcho>("echo");
  action_lib->Register<cActionPrintGenotypeAbundanceHistogram>("print_genotype_abundance_histogram");
  action_lib->Register<cActionPrintSpeciesAbundanceHistogram>("print_species_abundance_histogram");
  action_lib->Register<cActionPrintLineageTotals>("print_lineage_totals");
  action_lib->Register<cActionPrintLineageCounts>("print_lineage_counts");
  action_lib->Register<cActionPrintDominantGenotype>("print_dom");
  action_lib->Register<cActionPrintDominantParasiteGenotype>("print_dom_parasite");
  action_lib->Register<cActionPrintDetailedFitnessData>("print_detailed_fitness_data");
  action_lib->Register<cActionPrintGeneticDistanceData>("print_genetic_distance_data");
  
  action_lib->Register<cActionPrintGenotypes>("print_genotypes");
  action_lib->Register<cActionDumpMemory>("dump_memory");
}
