/*
 *  cFitnessMatrix.h
 *  Avida
 *
 *  Called "fitness_matrix.hh" prior to 12/2/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cFitnessMatrix_h
#define cFitnessMatrix_h

#include <numeric>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <list>
#include <sys/timeb.h>

#ifndef defs_h
#include "defs.h"
#endif
#ifndef cMxCodeArray_h
#include "cMxCodeArray.h"
#endif
#ifndef MyCodeArrayLessThan_h
#include "MyCodeArrayLessThan.h"
#endif
#ifndef cOrganism_h
#include "cOrganism.h"
#endif
#ifndef cStats_h
#include "cStats.h"
#endif
#ifndef cStringUtil_h
#include "cStringUtil.h"
#endif

class cGenome;
class cInstSet;
class MyCodeArrayLessThan;
class cWorld;

class cFitnessMatrix
{
private:
  cWorld* m_world;

  /* genome data */
  cMxCodeArray m_start_genotype;
  cInstSet *m_inst_set;
  std::set<cMxCodeArray, MyCodeArrayLessThan > m_data_set;
  double m_fitness_threshhold;

  /* parameters for search */
  int m_depth_limit;
  double m_fitness_threshold_ratio;

  /* parameters for diagonalization */
  int m_ham_thresh;
  double m_error_rate_min;
  double m_error_rate_max;
  double m_error_rate_step;
  int m_diag_iters;


  /* statistics of search */
  std::vector<int> m_DFSNumDead;
  std::vector<int> m_DFSNumBelowThresh;
  std::vector<int> m_DFSNumOK;
  std::vector<int> m_DFSNumNew;
  std::vector<int> m_DFSNumVisited;
  std::vector<int> m_DFSDepth;
  int m_DFS_MaxDepth;
  int m_DFS_NumRecorded;
  time_t m_search_start_time;
  time_t m_search_end_time;


  /* Methods for Depth-Limited Search of Genotype Space */

  void DepthLimitedSearch(const cMxCodeArray& startNode, std::ofstream& log_file, int currDepth=0);
  bool MutantOK(double fitness);
  void CollectData(std::ofstream& log_file);


  /* Methods for Diagonalization of Transition Matrix */
  double Diagonalize(std::vector<double>& randomVect, int hamDistThresh, double errorRate, std::ofstream& logfile);
  void MakeRandomVector(std::vector<double>& newVect, int size);
  void VectorDivideBy(std::vector<double>& vect, double div);
  double VectorNorm(const std::vector<double> &vect);
  void MatrixVectorMultiply(const std::vector<double>& vect, std::vector<double>& result);
  void MakeTransitionProbabilities(int hamDistThresh, double errorRate, std::ofstream& logfile);

  /* Data Output */
  void PrintGenotypes(std::ostream& fp);
  void PrintTransitionMatrix(std::ostream& fp, int hamDistThresh, double errorRate, double avg_fitness, bool printMatrix=false);
  void PrintHammingVector(std::ostream& fp,const std::vector<double>& dataVect, double errProb, double avgFit);
  void PrintFitnessVector(std::ostream& fp,const std::vector<double>& dataVect, double errProb, double avgFit, double maxFit, double step);
  void PrintFullVector(std::ostream& fp, const std::vector<double>& dataVect, double errProb, double avgFit);

  cFitnessMatrix(); // @not_implemented
  cFitnessMatrix(const cFitnessMatrix&); // @not_implemented
  cFitnessMatrix& operator=(const cFitnessMatrix&); // @not_implemented

public:
  cFitnessMatrix(cWorld* world, const cGenome&, cInstSet* inst_set);
  ~cFitnessMatrix();

  /**
   * The main entry function.
   *
   * @param depth_limit Limits the depth of the search
   * (how far should we go out in Hamming distance).
   *
   * @param fitness_threshold_ratio Creatures with fitnesses below the
   * starting fitness times this value are rejected
   *
   * @param ham_thresh The threshold for the construction of the matrix
   * (what transitions are included).
   *
   * @param error_rate_min The minimum error rate for which the matrix
   * should be diagonalized.
   *
   * @param error_rate_max The maximum error rate for which the matrix
   * should be diagonalized.
   *
   * @param error_rate_step The interval between two error rates at which
   * the matrix gets diagonalized.
   *
   * @param vect_fmax The maximum fitness to be considered in the output
   * vector. (We output concentrations of genotypes in bins of given fitness
   * width. This is the maximum fitness we consider).
   *
   * @param vect_fstep The width of the fitness bins for output.
   *
   * @param diag_iters The number of iterations for the diagonalization
   * of the matrix.
   *
   * @param write_ham_vector Should we also write a concentration vector
   * grouped according to Hamming distances?
   *
   * @param write_full_vector Should we also write the full concentration
   * vector?
   **/
  void CalcFitnessMatrix( int depth_limit, double fitness_threshold_ratio, int ham_thresh, double error_rate_min, double error_rate_max, double error_rate_step, double vect_fmax, double vect_fstep, int diag_iters, bool write_ham_vector, bool write_full_vector );
};

#endif
