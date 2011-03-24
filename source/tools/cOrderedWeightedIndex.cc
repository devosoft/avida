/*
 *  cWeighedIndex.cc
 *  Avida
 *
 *  Called "weighted_index.cc" prior to 12/7/05.
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

#include "cOrderedWeightedIndex.h"
#include <iostream>

using namespace std;

cOrderedWeightedIndex::cOrderedWeightedIndex()
  : item_weight(0)
  , cum_weight(0)
  , item_value(0)
{
}


cOrderedWeightedIndex::~cOrderedWeightedIndex()
{
}



 
void cOrderedWeightedIndex::SetWeight(int value, double in_weight)
{
  int cur_size = item_value.GetSize();
  item_weight.Resize(cur_size + 1);
  cum_weight.Resize(cur_size + 1);
  item_value.Resize(cur_size + 1);

  item_value[cur_size] = value;
  item_weight[cur_size] = in_weight;
  cum_weight[cur_size] = (cur_size == 0) ? in_weight : cum_weight[cur_size-1] + in_weight;
 }

int cOrderedWeightedIndex::FindPosition(double position){
  return Lookup(position, 0, GetSize()-1);
}


int cOrderedWeightedIndex::Lookup(double weight, int ndxA, int ndxE)
{
  int mid = ndxA + (ndxE - ndxA) / 2;

  if (cum_weight[mid]-item_weight[mid] <= weight && cum_weight[mid] > weight)
  {
    return item_value[mid];
  }
  
  if (cum_weight[mid] >  weight)
  { 
    return Lookup(weight, ndxA, mid-1);
  }
  else{
    return Lookup(weight, mid+1, ndxE);
  }
}



