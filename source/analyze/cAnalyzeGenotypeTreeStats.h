#ifndef cAnalyzeGenotypeTreeStats_h
#define cAnalyzeGenotypeTreeStats_h

#ifndef tArray_h
#include "tArray.h"
#endif
#ifndef tList_h
#include "tList.h"
#endif


class cAnalyzeGenotype;


/*
Convenience class cAGLData serves to collect various data I found I needed
while building the class cAnalyzeGenotypeTreeStats.
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

  cAGLData()
  : genotype(0)
  , id(-1)
  , pid(-1)
  , depth(-1)
  , birth(-1)
  , ppos(-1)
  , offspring_count(-1)
  , anc_branch_dist(-1)
  , anc_branch_id(-1)
  , anc_branch_pos(-1)
  , off_branch_dist_acc(-1)
  , cumulative_stemminess(-1.)
  , traversal_visited(false)
  , offspring_positions(0)
  {}
};

/*
class cAnalyzeGenotypeTreeStats

This class will eventually collect various kinds of statistics about various
kinds of trees (e.g., phylogenetic).

For now it only figures out "average cumulative stemminess".
*/
class cAnalyzeGenotypeTreeStats {
public:
  tArray<cAGLData> m_agl;
  tArray<cAGLData> m_agl2;

  double m_stemminess_sum;
  double m_average_stemminess;
  int m_inner_nodes;
  bool m_should_exclude_leaves;

public:
  cAnalyzeGenotypeTreeStats()
  : m_agl(0)
  , m_agl2(0)
  , m_stemminess_sum(0.0)
  , m_average_stemminess(0.0)
  , m_inner_nodes(0)
  , m_should_exclude_leaves(true)
  {}

  tArray<cAGLData> &AGL(){ return m_agl; }
  tArray<cAGLData> &AGL2(){ return m_agl2; }
  double StemminessSum(){ return m_stemminess_sum; }
  double AverageStemminess(){ return m_average_stemminess; }
  int InnerNodes(){ return m_inner_nodes; }
  bool ExcludesLeaves(){ return m_should_exclude_leaves; }

  void AnalyzeBatchTree(tList<cAnalyzeGenotype> &genotype_list);
};

#endif
