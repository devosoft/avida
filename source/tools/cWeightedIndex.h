/*
 *  cWeightedIndex.h
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#ifndef cWeightedIndex_h
#define cWeightedIndex_h

#ifndef tArray_h
#include "tArray.h"
#endif

#ifndef NULL
#define NULL 0
#endif

/**
 * This class allows indecies to be assigned a "weight" and then indexed by
 * that weight.
 **/

class cWeightedIndex {
protected:
  int size;
  tArray<double> item_weight;
  tArray<double> subtree_weight;

  void AdjustSubtree(int id, double weight_change);
public:
  cWeightedIndex(int in_size);
  ~cWeightedIndex();

  void SetWeight(int id, double weight);
  double GetWeight(int id) { return item_weight[id]; }

  double GetTotalWeight() { return subtree_weight[0]; }
  int FindPosition(double position, int root_id=0);

  int GetParent(int id)     { return (id-1) / 2; }
  int GetLeftChild(int id)  { return 2*id + 1; }
  int GetRightChild(int id) { return 2*id + 2; }
};

#endif
