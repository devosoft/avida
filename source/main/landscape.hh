//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef LANDSCAPE_HH
#define LANDSCAPE_HH

#ifndef CPU_TEST_INFO_HH
#include "cpu_test_info.hh"
#endif
#ifndef GENOME_HH
#include "genome.hh"
#endif
#ifndef STRING_HH
#include "string.hh"
#endif
#ifndef TMATRIX_HH
#include "tMatrix.hh"
#endif

#include <fstream>

class cInstSet;
class cGenome; // aggregate
class cCPUTestInfo; // aggregate
template <class T> class tMatrix; // aggregate
class cInstruction;
class cString; // aggregate

struct cLandscape {
private:
  const cInstSet & inst_set;
  cGenome base_genome;
  cGenome peak_genome;
  double base_fitness;
  double base_merit;
  double base_gestation;
  double peak_fitness;
  double total_fitness;
  double total_sqr_fitness;

  int distance;
  int del_distance;
  int ins_distance;

  int trials;

  int total_count;

  int dead_count;
  int neg_count;
  int neut_count;
  int pos_count;

  double pos_size; 
  double neg_size; 

  int total_epi_count;
  int pos_epi_count;
  int neg_epi_count;
  int no_epi_count;
  int dead_epi_count;

  double pos_epi_size; 
  double neg_epi_size; 
  double no_epi_size; 


  double test_fitness;

  int * site_count;

  double total_entropy;
  double complexity;

  cCPUTestInfo test_info;  // Info used for all cpu calculations.
  double neut_min;         // These two variables are a range around the base
  double neut_max;         //   fitness to be counted as neutral mutations.
  tMatrix<double> fitness_chart; // Chart of all one-step mutations.

private:
  void BuildFitnessChart();
  void ProcessGenome(cGenome & in_genome);
  void ProcessBase();
  void Process_Body(cGenome & cur_genome, int cur_distance, int start_line);

  void HillClimb_Body(std::ofstream & fp, cGenome & cur_genome, int & gen);
  void HillClimb_Print(std::ofstream & fp, const cGenome & _genome,
		       const int gen) const;

  double TestMutPair(cGenome & mod_genome, int line1, int line2,
    const cInstruction & mut1, const cInstruction & mut2, std::ostream & fp);
private:
  // disabled copy constructor.
  cLandscape(const cLandscape &);
public:
  cLandscape(const cGenome & in_genome, const cInstSet & in_inst_set);
  ~cLandscape();

  void Reset(const cGenome & in_genome);

  void Process(int in_distance=1);
  void ProcessDelete();
  void ProcessInsert();
  void PredictWProcess(std::ostream & fp, int update=-1);
  void PredictNuProcess(std::ostream & fp, int update=-1);

  void SampleProcess(int in_trials);
  int RandomProcess(int in_trials, int in_distance=1, int min_found=0,
		     int max_trials=0, bool print_if_found=false);

  void TestPairs(int in_trials, std::ostream & fp);
  void TestAllPairs(std::ostream & fp);

  void HillClimb(std::ofstream & fp);
  void HillClimb_Neut(std::ofstream & fp);
  void HillClimb_Rand(std::ofstream & fp);

  void PrintStats(std::ofstream & fp, int update=-1);
  void PrintEntropy(std::ofstream & fp);
  void PrintSiteCount(std::ofstream & fp);
  void PrintBase(cString filename);
  void PrintPeak(cString filename);

  inline const cGenome & GetPeakGenome() { return peak_genome; }
  inline double GetAveFitness() { return total_fitness / total_count; }
  inline double GetAveSqrFitness() { return total_sqr_fitness / total_count; }
  inline double GetPeakFitness() { return peak_fitness; }

  inline double GetProbDead() const { return ((double)dead_count)/total_count;}
  inline double GetProbNeg()  const { return ((double)neg_count) /total_count;}
  inline double GetProbNeut() const { return ((double)neut_count)/total_count;}
  inline double GetProbPos()  const { return ((double)pos_count) /total_count;}
  inline double GetAvPosSize() const { 
	if (pos_count == 0) return 0;
	else return pos_size/pos_count;}
  inline double GetAvNegSize() const { 
        if (neg_count == 0) return 0;
	else return neg_size/neg_count;}

  inline double GetProbEpiDead() const { 
	if (total_epi_count == 0) return 0;
	else	return ((double)dead_epi_count)/total_epi_count;}
  inline double GetProbEpiPos()  const { 
        if (total_epi_count == 0) return 0;
	else return ((double)pos_epi_count) /total_epi_count;}
  inline double GetProbEpiNeg()  const { 
        if (total_epi_count == 0) return 0;
        else return ((double)neg_epi_count) /total_epi_count;}
  inline double GetProbNoEpi() const {         
	if (total_epi_count == 0) return 0;
        else return ((double)no_epi_count)/total_epi_count;}
  inline double GetAvPosEpiSize() const { 
	if (pos_epi_count == 0) return 0;
	else return pos_epi_size/pos_epi_count;}
  inline double GetAvNegEpiSize() const { 
        if (neg_epi_count == 0) return 0;
	else return neg_epi_size/neg_epi_count;}
  inline double GetAvNoEpiSize() const { 
        if (no_epi_count == 0) return 0;
	else return no_epi_size/no_epi_count;}

  inline int GetNumTrials() const { return trials; }
};

#endif
