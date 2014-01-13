/*
 *  cAnalyzeTreeStats_CumulativeStemminess.h
 *  Avida@vallista
 *
 *  Created by Kaben Nanlohy on 2007.12.03.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeTreeStats_CumulativeStemminess_h
#define cAnalyzeTreeStats_CumulativeStemminess_h

#include "avida/core/Types.h"
#include "tList.h"

class cAnalyzeGenotype;
class cWorld;


/*
Convenience class cAGLData collects data I needed while
building the class cAnalyzeTreeStats_CumulativeStemminess.
*/
class cAGLData {
public:
  cAnalyzeGenotype *genotype;
  int id;
  int pid;
  int depth;
  int birth;
  int ppos;
  int offspring_count;
  int anc_branch_dist;
  int anc_branch_id;
  int anc_branch_pos;
  int off_branch_dist_acc;
  double cumulative_stemminess;
  bool traversal_visited;
  Apto::Array<int> offspring_positions;

  cAGLData();
};

/*
class cAnalyzeTreeStats_CumulativeStemminess

This class will eventually collect various kinds of statistics about various
kinds of trees (e.g., phylogenetic).

For now it only figures out "average cumulative stemminess".
*/
class cAnalyzeTreeStats_CumulativeStemminess {
public:
  Apto::Array<cAGLData> m_agl;
  Apto::Array<cAGLData> m_agl2;

  double m_stemminess_sum;
  double m_average_stemminess;
  int m_inner_nodes;
  bool m_should_exclude_leaves;

  cWorld* m_world;

public:
  cAnalyzeTreeStats_CumulativeStemminess(cWorld* world);

  // Accessors.
  Apto::Array<cAGLData> &AGL(){ return m_agl; }
  Apto::Array<cAGLData> &AGL2(){ return m_agl2; }
  // Getters.
  double StemminessSum(){ return m_stemminess_sum; }
  double AverageStemminess(){ return m_average_stemminess; }
  int InnerNodes(){ return m_inner_nodes; }
  bool ExcludesLeaves(){ return m_should_exclude_leaves; }

  void PrintAGLData(Apto::Array<cAGLData> &agl);

  // Commands.
  void AnalyzeBatchTree(tList<cAnalyzeGenotype> &genotype_list);
};

#endif
