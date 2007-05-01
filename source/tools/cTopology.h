/*! Builds different topologies out of ranges of cells.

Avida; cTopology.h
Copyright 2005-2006 Michigan State University. All rights reserved.

This file contains templated algorithms that create a particular cell
topology out of a given range of cells.  In every case, the range of cells is 
specified by a begin/end iterator pair.
*/
#ifndef _C_TOPOLOGY_H_
#define _C_TOPOLOGY_H_

#include "functions.h"

/*! Builds a torus topology out of the cells betwen the iterators.
In a torus, each cell is connected to up to 8 neighbors (including diagonals), 
and connections DO wrap around the logical edges of the torus.
*/
template< typename InputIterator >
void build_torus(InputIterator begin, InputIterator end, unsigned int rowSize, unsigned int colSize) {
  // Get the offset from the start of this range.  This is used to modify the 
  // parameters and return for GridNeighbor.
  int offset = begin->GetID();
  
  for(InputIterator i=begin; i!=end; ++i) {
    // The majority of all connections.
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 0, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, 0)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 0, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 0)]);
  }
}


/*! Builds a grid topology out of the cells betwen the iterators.
In a grid, each cell is connected to up to 8 neighbors (including diagonals), 
and connections do NOT wrap around the logical edges of the grid.
*/
template< typename InputIterator >
void build_grid(InputIterator begin, InputIterator end, unsigned int rowSize, unsigned int colSize) {
  // Start with a torus.
  build_torus(begin, end, rowSize, colSize);
  int offset = begin->GetID();

  // And now remove the connections that wrap around.
  for(InputIterator i=begin; i!=end; ++i) {
    int id = i->GetID();
    unsigned int x = (id-offset) % colSize;
    unsigned int y = (id-offset) / colSize;
    
    if(x==0) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 0)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 1)]);
    }
    if(x==(colSize-1)) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, 0)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, 1)]);      
    }
    if(y==0) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 0, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, -1)]);      
    }
    if(y==(rowSize-1)) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 0, 1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, 1)]);      
    }
  }
}


/*! Builds a clique topology out of the cells betwen the iterators.
In a clique, each cell is connected to all other cells in the given range.
*/
template< typename InputIterator >
void build_clique(InputIterator begin, InputIterator end, unsigned int rowSize, unsigned int colSize) {
  for(InputIterator i=begin; i!=end; ++i) {
    for(InputIterator j=begin; j!=end; ++j) {
      if(j!=i) {
        i->ConnectionList().Push(j);
      }
    }
  }
}


/*! Builds a hexagonal grid topology out of the cells between the iterators.  In
a hex grid, each cell has at most 6 neighbors, and connections do not wrap around
edges.
*/
template< typename InputIterator >
void build_hex(InputIterator begin, InputIterator end, unsigned int rowSize, unsigned int colSize) {
  // Start with a grid:
  build_grid(begin, end, rowSize, colSize);
  int offset = begin->GetID();  
  // ... and remove connections to the NE,SW:
  for(InputIterator i=begin; i!=end; ++i) {
    i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, 1, -1)]);
    i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, colSize, rowSize, -1, 1)]);
  }
}

#endif
