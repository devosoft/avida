/*
 *  cWeightedIndex.h
 *  Avida
 *
 *  Called "weighted_index.hh" prior to 12/7/05.
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

#ifndef cWeightedIndex_h
#define cWeightedIndex_h

#include "avida/core/Types.h"

#ifndef NULL
#define NULL 0
#endif

/**
 * This class allows indecies to be assigned a "weight" and then indexed by
 * that weight.
 **/

class cWeightedIndex
{
protected:
  int size;
  Apto::Array<double> item_weight;
  Apto::Array<double> subtree_weight;

  
  cWeightedIndex(); // @not_implemented
  
public:
  cWeightedIndex(int in_size);
  ~cWeightedIndex();

  void SetWeight(int id, double weight);
  double GetWeight(int id) { return item_weight[id]; }

  double GetTotalWeight() { return subtree_weight[0]; }
  int GetSize() const {return size;}
  int FindPosition(double position, int root_id=0);

  int GetParent(int id)     { return (id-1) / 2; }
  int GetLeftChild(int id)  { return 2*id + 1; }
  int GetRightChild(int id) { return 2*id + 2; }
};

#endif
