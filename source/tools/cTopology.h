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
void build_torus(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size) {
  // Get the offset from the start of this range.  This is used to modify the 
  // parameters and return for GridNeighbor.
  int offset = begin->GetID();
  
  for(InputIterator i=begin; i!=end; ++i) {
    // The majority of all connections.
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 0, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, -1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, 0)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 0, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 1)]);
    i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 0)]);
  }
}


/*! Builds a grid topology out of the cells betwen the iterators.
In a grid, each cell is connected to up to 8 neighbors (including diagonals), 
and connections do NOT wrap around the logical edges of the grid.
*/
template< typename InputIterator >
void build_grid(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size) {
  // Start with a torus.
  build_torus(begin, end, x_size, y_size);
  int offset = begin->GetID();

  // And now remove the connections that wrap around.
  for(InputIterator i=begin; i!=end; ++i) {
    int id = i->GetID();
    unsigned int x = (id-offset) % x_size;
    unsigned int y = (id-offset) / x_size;
    
    if(x==0) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 0)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 1)]);
    }
    if(x==(x_size-1)) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, 0)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, 1)]);      
    }
    if(y==0) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 0, -1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, -1)]);      
    }
    if(y==(y_size-1)) {
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 0, 1)]);
      i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, 1)]);      
    }
  }
}


/*! Builds a clique topology out of the cells betwen the iterators.
In a clique, each cell is connected to all other cells in the given range.
*/
template< typename InputIterator >
void build_clique(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size) {
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
void build_hex(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size) {
  // Start with a grid:
  build_grid(begin, end, x_size, y_size);
  int offset = begin->GetID();  
  // ... and remove connections to the NE,SW:
  for(InputIterator i=begin; i!=end; ++i) {
    i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, 1, -1)]);
    i->ConnectionList().Remove(&begin[GridNeighbor(i->GetID()-offset, x_size, y_size, -1, 1)]);
  }
}

/*! Builds a 3-dimensional lattice of cells, where the dimensions of the lattice are specified by 
 x, y, and z.
 
 A cell in the middle of the lattice is connected to 26 other cells (9 above, 9 below, and 8 on the
 same plane).  Edges do not wrap around in any direction.
 */
template< typename InputIterator >
void build_lattice(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size, unsigned int z_size) {
	// First we're going to create z grids each sized x by y:
	unsigned int gridsize = x_size * y_size;
	for(unsigned int i=0; i<z_size; ++i) {
		build_grid(&begin[gridsize*i], &begin[gridsize*(i+1)], x_size, y_size);
	}

	// This is the offset from the beginning of the cell_array; req'd to support demes.
	int offset = begin->GetID();

	// Now, iterate through each cell, and link them to their neighbors above and below:
	for(InputIterator i=begin; i!=end; ++i) {
		unsigned int layer = (i->GetID()-offset) / gridsize;
		unsigned int x = (i->GetID()-offset) % x_size;
		unsigned int y = (i->GetID()-offset) / x_size;		
		
		// The below is a big mess.  The reason it's a mess is because we have to respect the boundaries
		// of the grid.  There are probably much cleaner ways to do this, but it's complicated
		// enough now without having to think that through.  And anyway, this is only run
		// at initialization.  Feel free to fix it, however.
		if(layer != 0) {
			// We're not at the bottom; link to the layer below us.
			if(x != 0) {
				if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, -1)]); }
				if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, 1)]); }
				i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, 0)]);
			}
			
			if(x != (x_size-1)) {
				if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, -1)]); }
				if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, 1)]); }
				i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, 0)]);
			}
			
			if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 0, -1)]); }
			if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 0, 1)]); }
			
			// And now the cell right below this one:
			i->ConnectionList().Push(&begin[i->GetID()-offset-gridsize]);
		}
		
		if(layer != (z_size-1)) {
			// We're not at the top; link to the layer above us:
			if(x != 0) {
				if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, -1)]); }
				if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, 1)]); }
				i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, 0)]);
			}
			
			if(x != (x_size-1)) {
				if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, -1)]); }
				if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, 1)]); }
				i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, 0)]);
			}
			
			if(y != 0) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 0, -1)]); }
			if(y != (y_size-1)) { i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 0, 1)]); }

			// And now the cell right above this one:
			i->ConnectionList().Push(&begin[i->GetID()-offset+gridsize]);
		}
	}
}

#endif
