/*
 *  cWeighedIndex.cc
 *  Avida
 *
 *  Called "weighted_index.cc" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cWeightedIndex.h"

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
  if(subtree_weight[id] < 0.0001)  //bb: added to catch round off error
    subtree_weight[id] = 0.0;
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

