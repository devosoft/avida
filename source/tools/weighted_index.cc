//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHTED_INDEX_HH
#include "weighted_index.hh"
#endif
#include <iostream>


using namespace std;


cWeightedIndex::cWeightedIndex(int in_size)
  : size(in_size)
  , item_weight(size)
  , subtree_weight(size)
{
  item_weight.SetAll(0);
  subtree_weight.SetAll(0);
}

cWeightedIndex::~cWeightedIndex()
{
}

void cWeightedIndex::AdjustSubtree(int id, double weight_change)
{
  subtree_weight[id] += weight_change;
  if (id != 0) {
    AdjustSubtree(GetParent(id), weight_change);
  }
}

void cWeightedIndex::SetWeight(int id, double in_weight)
{
  const double weight_change = in_weight - item_weight[id];
  item_weight[id] = in_weight;
  AdjustSubtree(id, weight_change);
}

// This order of testing is about 10% faster than the one used below.
// Alas, it scans the array out of bounds...  For a real test we need to
// look at timings in optimized mode.
// int cWeightedIndex::FindPosition(double position, int root_id)
// {
//   // Check left...
//   const int left_id = GetLeftChild(root_id);
//   if (position < subtree_weight[left_id]) {
//     return FindPosition(position, left_id);
//   }

//   // Then right...
//   position -= subtree_weight[left_id];
//   const int right_id = GetRightChild(root_id);
//   if (position < subtree_weight[right_id]) {
//     return FindPosition(position, right_id);
//   }
  
//   // Then just return this!
//   return root_id;
// }

int cWeightedIndex::FindPosition(double position, int root_id)
{
//   cout << "Seeking " << position
//        << " at root " << root_id 
//        << " subtree size = " << subtree_weight[root_id]
//        << " (left=" << subtree_weight[GetLeftChild(root_id)]
//        << " , right=" << subtree_weight[GetRightChild(root_id)]
//        << " , this=" << item_weight[root_id] << ")"
//        << endl;
  assert(position < subtree_weight[root_id]);

  // First, see if we should just return this node.
  if (position < item_weight[root_id]) {
    return root_id;
  }

  // If not, then see if we should search in the left subtree...
  position -= item_weight[root_id];
  const int left_id = GetLeftChild(root_id);
  assert (left_id < size);
  if (position < subtree_weight[left_id]) {
    return FindPosition(position, left_id);
  }

  // Otherwise we must look in the right subtree...
  position -= subtree_weight[left_id];
  const int right_id = GetRightChild(root_id);
  assert (right_id < size);
  assert (position < subtree_weight[right_id]);
  return FindPosition(position, right_id);
}

