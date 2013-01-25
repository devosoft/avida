/*
 *  cLandscape.h
 *  Avida
 *
 *  Called "landscape.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cLandscape_h
#define cLandscape_h

#include "avida/core/Genome.h"
#include "avida/core/InstructionSequence.h"
#include "avida/output/Types.h"

#include "cCPUTestInfo.h"
#include "tMatrix.h"

class cAvidaContext;
class cInstSet;
class cTestCPU;
class cWorld;

using namespace Avida;


class cLandscape
{
private:
  cWorld* m_world;
  cCPUTestInfo m_cpu_test_info;
  Genome base_genome;
  Genome peak_genome;
  double base_fitness;
  double base_merit;
  double base_gestation;
  double peak_fitness;
  double total_fitness;
  double total_sqr_fitness;

  int distance;

  int trials;
  int m_min_found;
  int m_max_trials;

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

  int* site_count;

  double total_entropy;
  double complexity;

  double neut_min;         // These two variables are a range around the base
  double neut_max;         //   fitness to be counted as neutral mutations.
  tMatrix<double> fitness_chart; // Chart of all one-step mutations.
  
  int m_num_found;


  cLandscape(); // @not_implemented
  cLandscape(const cLandscape&); // @not_implemented
  cLandscape& operator=(const cLandscape&); // @not_implemented

public:
  cLandscape(cWorld* world, const Genome& in_genome);
  ~cLandscape();

  void Reset(const Genome& in_genome);

  void Process(cAvidaContext& ctx);
  void ProcessDelete(cAvidaContext& ctx);
  void ProcessInsert(cAvidaContext& ctx);
  void PredictWProcess(cAvidaContext& ctx, Avida::Output::File& df, int update = -1);
  void PredictNuProcess(cAvidaContext& ctx, Avida::Output::File& df, int update = -1);
  void ProcessDump(cAvidaContext& ctx, Avida::Output::File& df);
  
  inline void SetDistance(int in_distance) { distance = in_distance; }
  inline void SetTrials(int in_trials) { trials = in_trials; }
  inline void SetMinFound(int min_found) { m_min_found = min_found; }
  inline void SetMaxTrials(int max_trials) { m_max_trials = max_trials; }
  inline void SetCPUTestInfo(const cCPUTestInfo& in_cpu_test_info) 
  { 
      m_cpu_test_info = in_cpu_test_info; 
  }

  void SampleProcess(cAvidaContext& ctx);
  void RandomProcess(cAvidaContext& ctx);
  
  inline int GetNumFound() { return m_num_found; }

  void TestPairs(cAvidaContext& ctx);
  void TestAllPairs(cAvidaContext& ctx);

  void HillClimb(cAvidaContext& ctx, Avida::Output::File& df);

  void PrintStats(Avida::Output::File& df, int update = -1);
  void PrintEntropy(Avida::Output::File& fp);
  void PrintSiteCount(Avida::Output::File& fp);

  inline const Genome& GetPeakGenome() { return peak_genome; }
  inline double GetAveFitness() { return total_fitness / total_count; }
  inline double GetAveSqrFitness() { return total_sqr_fitness / total_count; }
  inline double GetPeakFitness() { return peak_fitness; }

  inline double GetProbDead() const { return static_cast<double>(dead_count) / total_count; }
  inline double GetProbNeg()  const { return static_cast<double>(neg_count) / total_count; }
  inline double GetProbNeut() const { return static_cast<double>(neut_count) / total_count; }
  inline double GetProbPos()  const { return static_cast<double>(pos_count) / total_count; }
  inline double GetAvPosSize() const { if (pos_count == 0) return 0; else return pos_size / pos_count; }
  inline double GetAvNegSize() const { if (neg_count == 0) return 0; else return neg_size / neg_count; }
  inline double GetProbEpiDead() const
  {
    if (total_epi_count == 0) return 0; else return static_cast<double>(dead_epi_count) / total_epi_count;
  }
  inline double GetProbEpiPos()  const
  { 
    if (total_epi_count == 0) return 0; else return static_cast<double>(pos_epi_count) / total_epi_count;
  }
  inline double GetProbEpiNeg()  const
  { 
    if (total_epi_count == 0) return 0; else return static_cast<double>(neg_epi_count) / total_epi_count;
  }
  inline double GetProbNoEpi() const
  {
    if (total_epi_count == 0) return 0; else return static_cast<double>(no_epi_count) / total_epi_count;
  }
  inline double GetAvPosEpiSize() const { if (pos_epi_count == 0) return 0; else return pos_epi_size / pos_epi_count; }
  inline double GetAvNegEpiSize() const { if (neg_epi_count == 0) return 0; else return neg_epi_size / neg_epi_count; }
  inline double GetAvNoEpiSize() const { if (no_epi_count == 0) return 0; else return no_epi_size / no_epi_count; }

  inline int GetNumTrials() const { return trials; }
  inline double GetTotalEntropy() const { return total_entropy; }
  inline double GetComplexity() const { return complexity; }
  
  
private:
  void BuildFitnessChart(cAvidaContext& ctx, cTestCPU* testcpu);
  double ProcessGenome(cAvidaContext& ctx, cTestCPU* testcpu, Genome& in_genome);
  void ProcessBase(cAvidaContext& ctx, cTestCPU* testcpu);
  void Process_Body(cAvidaContext& ctx, cTestCPU* testcpu, Genome& cur_genome, int cur_distance, int start_line);
  
  double TestMutPair(cAvidaContext& ctx, cTestCPU* testcpu, Genome& mod_genome, int line1, int line2,
                     const Instruction& mut1, const Instruction& mut2);  
};

#endif
