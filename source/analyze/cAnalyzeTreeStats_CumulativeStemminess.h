/*
 *  cAnalyzeTreeStats_CumulativeStemminess.h
 *  Avida@vallista
 *
 *  Created by Kaben Nanlohy on 2007.12.03.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cAnalyzeTreeStats_CumulativeStemminess_h
#define cAnalyzeTreeStats_CumulativeStemminess_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


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
  tArray<int> offspring_positions;

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
  tArray<cAGLData> m_agl;
  tArray<cAGLData> m_agl2;

  double m_stemminess_sum;
  double m_average_stemminess;
  int m_inner_nodes;
  bool m_should_exclude_leaves;

  cWorld* m_world;

public:
  cAnalyzeTreeStats_CumulativeStemminess(cWorld* world);

  // Accessors.
  tArray<cAGLData> &AGL(){ return m_agl; }
  tArray<cAGLData> &AGL2(){ return m_agl2; }
  // Getters.
  double StemminessSum(){ return m_stemminess_sum; }
  double AverageStemminess(){ return m_average_stemminess; }
  int InnerNodes(){ return m_inner_nodes; }
  bool ExcludesLeaves(){ return m_should_exclude_leaves; }

  void PrintAGLData(tArray<cAGLData> &agl);

  // Commands.
  void AnalyzeBatchTree(tList<cAnalyzeGenotype> &genotype_list);
};

#endif
