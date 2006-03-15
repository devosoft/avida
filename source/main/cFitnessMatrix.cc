/*
 *  cFitnessMatrix.cc
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#include "cFitnessMatrix.h"

#include "cAvidaContext.h"
#include "cInstSet.h"
#include "cWorld.h"

using namespace std;


cFitnessMatrix::cFitnessMatrix(cWorld* world, const cGenome& code,  cInstSet* inst_set)
  :  m_world(world), m_start_genotype(m_world, code), m_inst_set( inst_set ), m_DFS_MaxDepth(0),
     m_DFS_NumRecorded(0)
{
  // @DMB - Warning: Creating context out of band.
  cAvidaContext ctx(0);
  
  m_start_genotype.SetNumInstructions( m_inst_set->GetSize());
  m_start_genotype.CalcFitness(ctx);
  m_data_set.insert(m_start_genotype);
}

cFitnessMatrix::~cFitnessMatrix()
{
}



/*
  A wrapper routine to call the depth limited search with appropriate
  exception catching.
 */

void cFitnessMatrix::CollectData(ofstream& log_file)
{
  try
    {

      log_file << endl << "DOING DEPTH LIMITED SEARCH..." << endl << endl;

      time(&m_search_start_time);
      DepthLimitedSearch(m_start_genotype, log_file);
      time(&m_search_end_time);
    }
  catch(bad_alloc& alloc_failure)
    {
      log_file << "DLS Exception: memory allocation failure" << endl;
    }
  catch(exception& std_ex)
    {
      log_file << "DLS Exception: " << std_ex.what() <<endl;
    }
  catch(...)
    {
      log_file << "DLS Exception: " << "unrecognized exception" << endl;
    }

}


/*
  Statistics collected at each node:
  ----------------------------------

  numNew = number of mutants alive and above threshold that have not
  been seen before

  numVisited = number of mutants that have been seen before (alive and
  above threshold are considered only, because these are the only ones
  stored in the data set)

  numOK = number of mutants alive and above threshold (and new)

  numBelowthresh = number of mutants alive and below threshold (these
  could have been seen before because there is no record of them)

  numDead = number of non-reproducing mutants (these could have been
  seen before because there is no record of dead mutants)

  Consequently, if we sum numOK+numBelowThresh+numDead for all nodes
  then we will get the number of _unique_ genotypes inspected

*/

void cFitnessMatrix::DepthLimitedSearch(const cMxCodeArray& startNode, ofstream& log_file, int currDepth)
{
  if (currDepth == 0)
    {
      m_DFSNumDead.clear();
      m_DFSNumBelowThresh.clear();
      m_DFSNumOK.clear();
      m_DFSNumNew.clear();
      m_DFSNumVisited.clear();
      m_DFSDepth.clear();
    }


  log_file << "cFitnessMatrix::DepthLimitedSearch, depth = " << currDepth << endl;

  currDepth++;
  if (currDepth > m_DFS_MaxDepth) m_DFS_MaxDepth = currDepth;
  m_DFSDepth.push_back(currDepth);

  list<cMxCodeArray> theMutants;

  // statistics
  short numDead = 0, numBelowThresh = 0, numOK = 0;
  short numVisited = 0, numNew = 0;

  // get the 1 mutants of the start node
  startNode.Get1Mutants(theMutants);

  list<cMxCodeArray>::iterator list_iter;
  vector<cMxCodeArray> deleteMutants;
  vector<cMxCodeArray>::iterator vect_iter;

  // MyCodeArrayLessThan myLess;

  // @DMB - Warning: Creating context out of band.
  cAvidaContext ctx(0);

  for (list_iter = theMutants.begin(); list_iter != theMutants.end(); list_iter++)
    {
      // check if its already in the data set

      set<cMxCodeArray, MyCodeArrayLessThan >::iterator data_iter;
      pair< set<cMxCodeArray, MyCodeArrayLessThan >::iterator , bool> insert_result;

      cMxCodeArray test_array = *list_iter;

      data_iter = m_data_set.find(test_array);

      if (data_iter == m_data_set.end())
	{

	  list_iter->CalcFitness(ctx);
	  double fitness = list_iter->GetFitness();
	
	  if (fitness == 0.0)
	    {
	      numDead++;
	      deleteMutants.push_back(*list_iter);
	    }
	  else if (MutantOK(fitness))
	    {
	      insert_result = m_data_set.insert(*list_iter);

	      numNew++;
	      numOK++;
	      m_DFS_NumRecorded++;
	    }
	  else
	    {
	      numBelowThresh++;
	      deleteMutants.push_back(*list_iter);
	    }
	}
      // if its already in the data set delete from the list
      else
	{
	  // log_file << "Mutant already in Set!!!!" << endl;
	  numVisited++;
	  deleteMutants.push_back(*list_iter);
	}
    }

  // delete the ones that are dead or already visited from list
  for (vect_iter = deleteMutants.begin(); vect_iter != deleteMutants.end(); vect_iter++)
    {
      theMutants.remove(*vect_iter);
    }

  // save statistics of this node
  m_DFSNumDead.push_back(numDead);
  m_DFSNumBelowThresh.push_back(numBelowThresh);
  m_DFSNumOK.push_back(numOK);
  m_DFSNumNew.push_back(numNew);
  m_DFSNumVisited.push_back(numVisited);

  // call depth first search on each mutant in the list
  if (currDepth < m_depth_limit)
    {
      for (list_iter = theMutants.begin(); list_iter != theMutants.end(); list_iter++)
	{
	  DepthLimitedSearch(*list_iter, log_file, currDepth);
	}
    }

}


bool cFitnessMatrix::MutantOK(double fitness)
{
  return (fitness > m_fitness_threshhold);
}


/*
  Diagonaliztion via repeated multiplication by a random vector.
  Based on Claus' diagonalize.pl
 */

double cFitnessMatrix::Diagonalize(vector<double>& randomVect, int hamDistThresh, double errorRate, ofstream& log_file)
{
  // first all genotypes need their transition lists
  this->MakeTransitionProbabilities(hamDistThresh, errorRate, log_file);

  log_file << endl << "DIAGONALIZING...  (error prob=" << errorRate << ")" << endl << endl;

  // initialize the random vector
  vector<double> norms;
  MakeRandomVector(randomVect, m_data_set.size());

  vector<double> result;
  // do repeated multiplication
  for (int i = 0; i < m_diag_iters; i++)
    {
      /*
      norms.push_back(VectorNorm(randomVect));
      VectorDivideBy(randomVect, norms.back());
      MatrixVectorMultiply(randomVect, result);
      randomVect = result;
      */

      MatrixVectorMultiply(randomVect, result);
      norms.push_back(VectorNorm(result));
      VectorDivideBy(result, norms.back());
      randomVect = result;

      log_file << "Vect Norm(" << i << "):" << norms.back() << endl;
    }


  return norms.back();
}


/*
   Make transition lists for all the genotypes in the data set.  The
   algorithm for building the transition lists is O(n^2) - for every
   genotype we have to look at all the other genotypes to make its
   transition list.
*/


void cFitnessMatrix::MakeTransitionProbabilities(int hamDistThresh, double errorRate, ofstream& log_file)
{

  set<cMxCodeArray, MyCodeArrayLessThan >::iterator set_iter;
  int num = 0;
  int trans = 0;

  log_file << endl << "MAKING TRANSITION PROBABILITIES... (error prob=" << errorRate << ")" << endl << endl;

  for (set_iter = m_data_set.begin(); set_iter != m_data_set.end(); set_iter++, num++)
    {
      trans = (*set_iter).MakeTransitionList(m_data_set, hamDistThresh, errorRate);
      log_file << num << ") " << trans << " transitions for " << (*set_iter).AsString() << endl;
    }

}


void cFitnessMatrix::MakeRandomVector(vector<double>& newVect, int size)
{
  cRandom randy(time(NULL));

  /* vector should be the size of the set + 1 - because the matrix
     also has the "other" column.  So thats why its '<='
   */
  for (int i = 0; i <= size; i++)
    {
      newVect.push_back(randy.GetDouble());
    }

}

void cFitnessMatrix::VectorDivideBy(vector<double>& vect, double div)
{
  vector<double>::iterator vect_iter;

  for(vect_iter = vect.begin(); vect_iter != vect.end(); vect_iter++)
    {
      *vect_iter /= div;
    }
}

double cFitnessMatrix::VectorNorm(const vector<double> &vect)
{
  double norm = 0.0;
  vector<double>::const_iterator vect_iter;

  for(vect_iter = vect.begin(); vect_iter != vect.end(); vect_iter++)
    {
      norm = norm + (*vect_iter);
    }

  return norm;
}


/*
  Perform the matrix vector multiplication using the sparse matrix
  representation in the 'transition map' of the genotypes.  The
  transition map has entries for every genotype that it has a non-zero
  probability of giving birth to.

  This algorithm is iterating through the columns of the matrix and
  adding each one's contributions to the appropriate rows of the
  resulting vector.  This is sort of the reverse of the normal row by
  row dot product way of thinking about matrix-vector multiplication.

  The true transition matrix has a last column of all zeros - the
  probabilities that a dead genotype will give birth to a live one.
  This is simply ignored because it doesn't effect the calculation
  (and because we're using the actual data set to do the calculation
  there isn't any good way to make a cMxCodeArray to represent dead
  genotypes...)

 */

void cFitnessMatrix::MatrixVectorMultiply(const vector<double> &vect, vector<double>& result)
{

  result.clear();
  result.resize(vect.size(), 0.0);

  set<cMxCodeArray, MyCodeArrayLessThan >::iterator set_iter;

  int column = 0;
  for (set_iter = m_data_set.begin(); set_iter != m_data_set.end(); set_iter++, column++)
    {
      const map<int, double, less<int> > trans_map = (*set_iter).GetTransitionList();

      map<int, double, less<int> >::const_iterator trans_iter;
      for(trans_iter = trans_map.begin(); trans_iter != trans_map.end(); trans_iter++)
	{
	  int row = (*trans_iter).first;
	  double transition_probability = (*trans_iter).second;

	  result[row] += vect[column] * transition_probability;
	}

    }

}

void cFitnessMatrix::PrintGenotypes(ostream& fp)
{
  int totNumDead=0, totNumBelowThresh=0, totNumOK=0;
  int totNumNew=0, totNumVisited=0, totDepth=0;
  int totNumDead2=0, totNumBelowThresh2=0, totNumOK2=0;
  int totNumNew2=0, totNumVisited2=0, totDepth2=0;
  int numNodes;
  float avgNumDead=0.0, avgNumBelowThresh=0.0, avgNumOK=0.0;
  float avgNumNew=0.0, avgNumVisited=0.0, avgDepth=0.0;
  float sdNumDead=0.0, sdNumBelowThresh=0.0, sdNumOK=0.0;
  float sdNumNew=0.0, sdNumVisited=0.0, sdDepth=0.0;

  totNumDead = accumulate(m_DFSNumDead.begin(), m_DFSNumDead.end(), 0);
  totNumBelowThresh = accumulate(m_DFSNumBelowThresh.begin(), m_DFSNumBelowThresh.end(), 0);
  totNumOK = accumulate(m_DFSNumOK.begin(), m_DFSNumOK.end(), 0);
  totNumNew = accumulate(m_DFSNumNew.begin(), m_DFSNumNew.end(), 0);
  totNumVisited = accumulate(m_DFSNumVisited.begin(), m_DFSNumVisited.end(), 0);
  totDepth = accumulate(m_DFSDepth.begin(), m_DFSDepth.end(), 0);

  numNodes = m_DFSNumDead.size();

  avgNumDead = ((float)totNumDead)/((float)numNodes);
  avgNumBelowThresh = ((float)totNumBelowThresh)/((float)numNodes);
  avgNumOK = ((float)totNumOK)/((float)numNodes);
  avgNumNew = ((float)totNumNew)/((float)numNodes);
  avgNumVisited = ((float)totNumVisited)/((float)numNodes);
  avgDepth = ((float)totDepth)/((float)numNodes);

  for (int i = 0; i < numNodes; i++)
    {
      totNumDead2 += m_DFSNumDead[i]*m_DFSNumDead[i];
      totNumBelowThresh2 += m_DFSNumBelowThresh[i]*m_DFSNumBelowThresh[i];
      totNumOK2 += m_DFSNumOK[i]*m_DFSNumOK[i];
      totNumNew2 += m_DFSNumNew[i]*m_DFSNumNew[i];
      totNumVisited2 += m_DFSNumVisited[i]*m_DFSNumVisited[i];
      totDepth2 += m_DFSDepth[i]*m_DFSDepth[i];
    }

  sdNumDead = sqrt( ((float)totNumDead2)/((float)numNodes)
    - (avgNumDead*avgNumDead));
  sdNumBelowThresh = sqrt( ((float) totNumBelowThresh2)/((float) numNodes)
    - (avgNumBelowThresh *avgNumBelowThresh ));
  sdNumOK =sqrt( ((float) totNumOK2)/((float) numNodes)
    - ( avgNumOK* avgNumOK)) ;
  sdNumNew = sqrt( ((float) totNumNew2)/((float) numNodes)
    - (avgNumNew * avgNumNew) );
  sdNumVisited = sqrt( ((float) totNumVisited2)/((float) numNodes)
    - (avgNumVisited * avgNumVisited) );
  sdDepth = sqrt( ((float) totDepth2)/((float) numNodes)
    - (avgDepth * avgDepth) );


  fp << "#######################################################" << endl;
  fp << "# DEPTH FIRST SEARCH RESULTS " << endl;
  fp << "# Depth Limit: " << m_depth_limit << endl;
  fp << "# Fitness Threshold Ratio: " << m_fitness_threshold_ratio  << endl;
  fp << "# Start time: " << asctime(localtime(&m_search_start_time));
  fp << "# End Time: " << asctime(localtime(&m_search_end_time));
  fp << "#######################################################" << endl;
  fp << "# Total number of unique nodes checked: ";
  fp << totNumDead + totNumOK + totNumBelowThresh << endl;
  fp << "# Total number of unique living genotypes found: ";
  fp << totNumOK + totNumBelowThresh << endl;
  fp << "# Total number of unique dead genotypes found: ";
  fp << totNumDead << endl;
  fp << "# Total number of unique genotypes alive and below threshold found: " << endl;
  fp << "# " << totNumBelowThresh << endl;
  fp << "# Total number of unique genotypes alive and above threshold found: " << endl;
  fp << "# " << totNumOK << endl;
  fp << "# Number of Nodes Fully Explored (non-leaf, alive and above threshold): " << endl;
  fp << "# " << numNodes << endl;
  fp << "# Maximum Depth Visited: ";
  fp << m_DFS_MaxDepth << endl;
  fp << "# Number of 1-Mutants Dead at each node, avg - stand dev" << endl;
  fp << "# " << avgNumDead << "\t\t" << sdNumDead << endl;
  fp << "# Number of 1-Mutants Alive but Below Threshold at each node,  avg - stand dev" << endl;
  fp << "# " << avgNumBelowThresh << "\t\t" << sdNumBelowThresh << endl;
  fp << "# Number of 1-Mutants  alive and above threshold at each node,  avg - stand dev" << endl;
  fp << "# " << avgNumOK << "\t\t" << sdNumOK << endl;
  fp << "# Number of New living, above threshold, 1-Mutants at each Node, avg - stand dev" << endl;
  fp << "# " << avgNumNew << "\t\t" << sdNumNew << endl;
  fp << "# Number of Already found living, above threshold 1-Mutants at each Node, avg - stand dev" << endl;
  fp << "# " << avgNumVisited << "\t\t" << sdNumVisited << endl;
  fp << "# Depth of Nodes in search, avg - stand dev" << endl;
  fp << "# " << avgDepth << "\t\t" << sdDepth << endl;
  fp << "#######################################################" << endl;
  fp << "# Fitness:" << endl;

  set<cMxCodeArray, MyCodeArrayLessThan >::iterator iter;
  for(iter = m_data_set.begin(); iter != m_data_set.end(); iter++)
    {
      fp << (*iter).AsString() << "\t" <<(*iter).GetFitness() << "\t" << (*iter).GetMerit() << "\t" << (*iter).GetGestationTime() << endl;
    }

  fp << "#######################################################" << endl;

}

/*
  PrintTransitionList gets m_data_set.size()+1 because there is the
  extra row at the end for the "other" category.  We also add an extra
  row of zeros for the "other" (dead) genotypes so that we're actually
  writing out a square matrix that can be processed by other programs
  (Mathematica).

  Note that this is actually printing the transpose of the transition
  matrix, assuming the matrix rows represent the offspring genotypes
  and the columns represent the parents and that the norm vector is a
  column vector.  (as discussed with Claus in March, 2001).  But since
  we're printing one parent genotype at a time there is just no
  sensible way to print a column at a time...

  Used for testing the diagonalization - this isn't really useful for
  real experiments because the matrix will be thousands by thousands
  big and printing it out is not feasible.

 */

void cFitnessMatrix::PrintTransitionMatrix(ostream& fp, int hamDistThresh, double errorRate, double avg_fitness, bool printMatrix)
{

  fp << endl << endl;
  fp << "#######################################################" << endl;
  fp << "TRANSITION PROBABILITIES" << endl;
  fp << "#Hamming Distance Threshold" << endl;
  fp << "# " << hamDistThresh << endl;
  fp << "#Error Rate" << endl;
  fp << "# " << errorRate << endl;
  fp << "#Avg Fitness" << endl;
  fp << "# " << avg_fitness << endl;
  fp << "#######################################################" << endl;


  if (printMatrix)
    {
      set<cMxCodeArray, MyCodeArrayLessThan >::iterator iter;
      fp << "{" << endl;
      int index = 0;
      for(iter = m_data_set.begin(); iter != m_data_set.end(); iter++, index++)
	{
	  (*iter).PrintTransitionList(fp, m_data_set.size()+1);
	  fp << "," << endl;
	}


      /*
	Add the transition probabilities for "other" (dead) genotypes
	so that we're actually writing out a square matrix that can be
	processed by other programs (Mathematica)
      */

      fp << "{" ;
      for (index = 0; index <= (int) m_data_set.size(); index++)
	{
	  fp << 0.0;
	  if (index < (int) m_data_set.size())
	    fp << " , ";
	}
      fp << "}" << endl;

      fp << "}";
    }

}



void cFitnessMatrix::PrintHammingVector(ostream& fp,const vector<double>& dataVect, double errProb, double avgFit)
{
  vector<double> hamVect;

  for (int i = 0; i < m_start_genotype.GetSize(); i++)
    hamVect.push_back(0.0);

  set<cMxCodeArray, MyCodeArrayLessThan >::iterator data_iter;

  int index = 0;

  for (data_iter = m_data_set.begin(); data_iter != m_data_set.end(); data_iter++, index++)
    {
      int dist = m_start_genotype.HammingDistance(*data_iter);
      hamVect[dist] += dataVect[index];
    }



  fp << errProb << "\t";
  fp << avgFit << "\t";

  vector<double>::const_iterator vect_iter;

  for(vect_iter = hamVect.begin(); vect_iter != hamVect.end(); vect_iter++)
    {
      fp << (*vect_iter) << "\t";
    }

  fp << endl;

}


void cFitnessMatrix::PrintFitnessVector(ostream& fp,const vector<double>& dataVect, double errProb, double avgFit, double maxFit, double step)
{
  vector<double> fitVect;

  int maxi = (int) (maxFit /step) +1;

  for (int i = 0; i < maxi; i++)
    fitVect.push_back(0.0);

  set<cMxCodeArray, MyCodeArrayLessThan >::iterator data_iter;

  int index = 0;

  for (data_iter = m_data_set.begin(); data_iter != m_data_set.end(); data_iter++, index++)
    {
      double f = (*data_iter).GetFitness();
      fitVect[(int) (f/step)] += dataVect[index];
    }
  // the last contribution is of fitness zero:
  fitVect[0] += dataVect[index];



  fp << errProb << "\t";
  fp << avgFit << "\t";

  vector<double>::const_iterator vect_iter;

  for(vect_iter = fitVect.begin(); vect_iter != fitVect.end(); vect_iter++)
    {
      fp << (*vect_iter) << "\t";
    }

  fp << endl;

}

void cFitnessMatrix::PrintFullVector(ostream& fp,const vector<double>& dataVect, double errProb, double avgFit)
{
  fp << errProb << "\t";
  fp << avgFit << "\t";

  vector<double>::const_iterator vect_iter;

  for(vect_iter = dataVect.begin(); vect_iter != dataVect.end(); vect_iter++)
    {
      fp << (*vect_iter) << "\t";
    }

  fp << endl;
}



void cFitnessMatrix::CalcFitnessMatrix( int depth_limit, double fitness_threshold_ratio, int ham_thresh, double error_rate_min, double error_rate_max, double error_rate_step, double output_start, double output_step, int diag_iters, bool write_ham_vector, bool write_full_vector )
{

  /* set parameters (TODO: read these from event list) */

  m_depth_limit = depth_limit;
  m_fitness_threshold_ratio = fitness_threshold_ratio;
  m_ham_thresh = ham_thresh;
  m_error_rate_min = error_rate_min;
  m_error_rate_max = error_rate_max;
  m_error_rate_step = error_rate_step;
  m_diag_iters = diag_iters;

  m_fitness_threshhold = m_start_genotype.GetFitness() * m_fitness_threshold_ratio;

  /* do the depth first search */
  CollectData(m_world->GetDataFileOFStream("fitness_matrix.log"));
  PrintGenotypes(m_world->GetDataFileOFStream("found_genotypes.dat"));

  /* diagonalize transition matrices at different copy error rates */
  for (double error = m_error_rate_min; error <= m_error_rate_max; error += m_error_rate_step)
  {
    vector<double> dataVect;
    double avg_fitness = Diagonalize(dataVect, m_ham_thresh, error, m_world->GetDataFileOFStream("fitness_matrix.log"));

    PrintFitnessVector(m_world->GetDataFileOFStream("fitness_vect.dat"), dataVect, error, avg_fitness, output_start, output_step);

    if (write_ham_vector)
      PrintHammingVector(m_world->GetDataFileOFStream("hamming_vect.dat"), dataVect, error, avg_fitness);
    if (write_full_vector)
      PrintFullVector(m_world->GetDataFileOFStream("full_vect.dat"), dataVect, error, avg_fitness);
  }
}

