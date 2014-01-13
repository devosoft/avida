/*
 * cTopology.h
 * Avida
 *
 * Copyright 2005-2011 Michigan State University. All rights reserved.
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

#ifndef cTopology_h
#define cTopology_h

/*! Builds different topologies out of ranges of cells.
 
 This file contains templated algorithms that create a particular cell
 topology out of a given range of cells.  In every case, the range of cells is
 specified by a begin/end iterator pair.
 */

#include "AvidaTools.h"

using namespace AvidaTools;

/*! Builds a torus topology out of the cells betwen the iterators.
 In a torus, each cell is connected to up to 8 neighbors (including diagonals),
 and connections DO wrap around the logical edges of the torus.
 */
template< typename ArraySlice >
void build_torus(ArraySlice slice, unsigned int x_size, unsigned int y_size) {
  // Get the offset from the start of this range.  This is used to modify the
  // parameters and return for GridNeighbor.
  int offset = slice[0].GetID();
  
  for (int i = 0; i < slice.GetSize(); ++i) {
    // The majority of all connections.
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, -1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 0, -1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, -1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, 0)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, 1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 0, 1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, 1)]);
    slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, 0)]);
  }
}


/*! Builds a grid topology out of the cells betwen the iterators.
 In a grid, each cell is connected to up to 8 neighbors (including diagonals),
 and connections do NOT wrap around the logical edges of the grid.
 */
template< typename ArraySlice >
void build_grid(ArraySlice slice, unsigned int x_size, unsigned int y_size) {
  // Start with a torus.
  build_torus(slice, x_size, y_size);
  int offset = slice[0].GetID();
  
  // And now remove the connections that wrap around.
  for (int i = 0; i < slice.GetSize(); ++i) {
    int id = slice[i].GetID();
    unsigned int x = (id-offset) % x_size;
    unsigned int y = (id-offset) / x_size;
    
    if (x == 0) {
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, -1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, 0)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, 1)]);
    }
    if (x == (x_size - 1)) {
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, -1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, 0)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, 1)]);
    }
    if (y == 0) {
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, -1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 0, -1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, -1)]);
    }
    if (y == (y_size - 1)) {
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, -1, 1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 0, 1)]);
      slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID() - offset, x_size, y_size, 1, 1)]);
    }
  }
}


/*! Builds a clique topology out of the cells betwen the iterators.
 In a clique, each cell is connected to all other cells in the given range.
 */
template< typename ArraySlice >
void build_clique(ArraySlice slice, unsigned int, unsigned int) {
  for (int i = 0; i < slice.GetSize(); ++i) {
    for (int j = 0; j < slice.GetSize(); ++j) {
      if (j != i) {
        slice[i].ConnectionList().Push(&slice[j]);
      }
    }
  }
}


/*! Builds a hexagonal grid topology out of the cells between the iterators.  In
 a hex grid, each cell has at most 6 neighbors, and connections do not wrap around
 edges.
 */
template< typename ArraySlice >
void build_hex(ArraySlice slice, unsigned int x_size, unsigned int y_size) {
  // Start with a grid:
  build_grid(slice, x_size, y_size);
  int offset = slice[0].GetID();
  // ... and remove connections to the NE,SW:
  for (int i = 0; i < slice.GetSize(); ++i) {
    slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID()-offset, x_size, y_size, 1, -1)]);
    slice[i].ConnectionList().Remove(&slice[GridNeighbor(slice[i].GetID()-offset, x_size, y_size, -1, 1)]);
  }
}

/*! Builds a 3-dimensional lattice of cells, where the dimensions of the lattice are specified by
 x, y, and z.
 
 A cell in the middle of the lattice is connected to 26 other cells (9 above, 9 below, and 8 on the
 same plane).  Edges do not wrap around in any direction.
 */
template< typename ArraySlice >
void build_lattice(ArraySlice slice, unsigned int x_size, unsigned int y_size, unsigned int z_size) {
  // First we're going to create z grids each sized x by y:
  unsigned int gridsize = x_size * y_size;
  
  for (unsigned int i = 0; i < z_size; ++i) {
    build_grid(slice.Range(gridsize * i, gridsize * (i + 1)), x_size, y_size);
  }
  
  // This is the offset from the beginning of the cell_array; req'd to support demes.
  int offset = slice[0].GetID();
  
  // Now, iterate through each cell, and link them to their neighbors above and below:
  for (int i = 0; i < slice.GetSize(); ++i) {
    unsigned int layer = (slice[i].GetID()-offset) / gridsize;
    unsigned int x = (slice[i].GetID()-offset) % x_size;
    unsigned int y = (slice[i].GetID()-offset) / x_size;
    
    // The below is a big mess.  The reason it's a mess is because we have to respect the boundaries
    // of the grid.  There are probably much cleaner ways to do this, but it's complicated
    // enough now without having to think that through.  And anyway, this is only run
    // at initialization.  Feel free to fix it, however.
    if (layer != 0) {
      // We're not at the bottom; link to the layer below us.
      if (x != 0) {
        if (y != 0) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, -1, -1)]);
        }
        if (y != (y_size-1)) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, -1, 1)]);
        }
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, -1, 0)]);
      }
      
      if (x != (x_size-1)) {
        if (y != 0) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, 1, -1)]);
        }
        if (y != (y_size-1)) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, 1, 1)]);
        }
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, 1, 0)]);
      }
			
      if (y != 0) {
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, 0, -1)]);
      }
      if(y != (y_size-1)) {
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset-gridsize, x_size, y_size, 0, 1)]);
      }
			
      // And now the cell right below this one:
      slice[i].ConnectionList().Push(&slice[slice[i].GetID()-offset-gridsize]);
    }
    
    if (layer != (z_size-1)) {
      // We're not at the top; link to the layer above us:
      if(x != 0) {
        if (y != 0) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, -1, -1)]);
        }
        if(y != (y_size-1)) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, -1, 1)]);
        }
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, -1, 0)]);
      }
			
      if (x != (x_size-1)) {
        if (y != 0) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, 1, -1)]);
        }
        if (y != (y_size-1)) {
          slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, 1, 1)]);
        }
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, 1, 0)]);
      }
      
      if (y != 0) {
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, 0, -1)]);
      }
      if(y != (y_size-1)) {
        slice[i].ConnectionList().Push(&slice[GridNeighbor(slice[i].GetID()-offset+gridsize, x_size, y_size, 0, 1)]);
      }
      
      // And now the cell right above this one:
      slice[i].ConnectionList().Push(&slice[slice[i].GetID()-offset+gridsize]);
    }
  }
}


/*
 Builds a random connected network topology for organisms to communicate through.
 
 */
template< typename ArraySlice >
void build_random_connected_network(ArraySlice slice, unsigned int x_size, unsigned int y_size, Apto::Random& rng) {
	
	// keep track of boundaries for this deme:
	int offset = slice[0].GetID();
	int demeSize = x_size * y_size;
	
	// keep track of cells that have been connected already:
	std::set<int> connected_Cells;
		
	
	for (int i = 0; i < slice.GetSize(); ++i) {
		// select a random cell in this deme to connect to:
		int targetCellID;
		do {
			targetCellID = rng.GetInt(0, demeSize);
		} while((targetCellID + offset) == slice[i].GetID());
		
		int j = targetCellID;
		
		// verify no connection exists between i and j:
		if (slice[i].ConnectionList().FindPtr(&slice[j]) == NULL) {
			// create bidirectional connections:
			slice[i].ConnectionList().Push(&slice[j]);
			slice[j].ConnectionList().Push(&slice[i]);
			
			// check if either i or j is connected to the
			// main graph:
			if(connected_Cells.count(slice[i].GetID()) == 0 && connected_Cells.count(slice[j].GetID()) == 0) {
				// neither i nor j is connected to the main graph
				
				// check if main network is empty:
				if(connected_Cells.empty()) {
					connected_Cells.insert(slice[i].GetID());
					connected_Cells.insert(slice[j].GetID());
				} else {
					// pick some random cell that is connected:
					int randomIndex = rng.GetInt(0, connected_Cells.size());
					int counter = 0;
					int idValue = 0;
					set<int>::iterator pos;
					for(pos = connected_Cells.begin(); pos != connected_Cells.end(); ++pos) {
						if(counter == randomIndex) {
							idValue = *pos;
							break;
						} else {
							counter++;
						}
					}
					
					// retrieve the actual cell:
					int random_Connected_Cell = idValue - offset;
					
					// randomly select i or j to connect with main network:
					int zeroOrOne = rng.GetInt(0,2);
					
					if(zeroOrOne) {
						// connect i to main network:
						slice[i].ConnectionList().Push(&slice[random_Connected_Cell]);
						slice[random_Connected_Cell].ConnectionList().Push(&slice[i]);
					} else {
						// connect j to main network:
						slice[j].ConnectionList().Push(&slice[random_Connected_Cell]);
						slice[random_Connected_Cell].ConnectionList().Push(&slice[j]);
					}
					
					// add both cells to the main network:
					// don't care about duplicates...
					connected_Cells.insert(slice[i].GetID());
					connected_Cells.insert(slice[j].GetID());
				}
				
			} else {
				connected_Cells.insert(slice[i].GetID());
				connected_Cells.insert(slice[j].GetID());
			}
		}
	}
	
	
	// the code above ensures that we have *at the least* a random connected
	// bidirectional network.
	// sprinkle additional edges between the cells:
	
	// we are going to add random extra edges... note demeSize bound is arbitrary.
	int extraEdges = rng.GetInt(0, demeSize);
	int a, b;
	
	for(int n = 0; n < extraEdges; ++n) {
		// must select two random cells from the network:
		a = rng.GetInt(0,demeSize);
		b = rng.GetInt(0,demeSize);
		
		while (a == b) b = rng.GetInt(0,demeSize);
		
		int i = a;
		int j = b;
		
		// check for existing connection between the two:
		if (slice[i].ConnectionList().FindPtr(&slice[j]) == NULL) {
			slice[i].ConnectionList().Push(&slice[j]);
			slice[j].ConnectionList().Push(&slice[i]);
		}
	}
}


//! Helper function to connect two cells.
template <typename InputIterator>
void connect(InputIterator u, InputIterator v) {
	assert(u != v);
	u->ConnectionList().Push(v);
	v->ConnectionList().Push(u);
}

//! Helper function to test if two cells are already connected.
template <typename InputIterator>
bool edge(InputIterator u, InputIterator v) {
	assert(u != v);
	return u->ConnectionList().FindPtr(v) || v->ConnectionList().FindPtr(u);
}


/*! Builds a scale-free network from the given range of cells.
 
 This function is an implementation of the Barab\'asi-Albert "preferential attachment"
 algorithm for iteratively constructing a scale-free network.
 
 If we think of the population as a graph G, where cells are vertices and connections
 are edges, then |E(G)| is the number of edges in the graph and d(u \in V(G)) is the
 degree of a vertex in that graph, the algorithm for preferential attachment is defined as:
 
 Input:
 m = number of edges to be added for each new vertex
 alpha = "power," the degree to which vertices with large numbers of edges are weighted.
 zero_appeal = offset to prefer vertices with 0 edges
 
 Initialization:
 G = a graph, where all nodes have degree >= 1
 
 foreach vertex u to be added to G:
 foreach vertex v \in G != u, where !e(u,v), and until m edges are added:
 connect u-v with probability: (d(v)/|E(G)|)^alpha + zero_appeal
 */
template <typename ArraySlice>
void build_scale_free(ArraySlice slice, int m, double alpha, double zero_appeal, Apto::Random& rng) {
	assert(slice.GetSize() > 1); // at least two vertices.
	// Connect the first and second cells:
	connect(&slice[0], &slice[1]);
	// And initialize the edge and vertex counts:
	int edge_count=1;
	int vertex_count=2;
	
	// Now, for each new vertex (that is, vertices 2+):
	for(int u = 2; u < slice.GetSize(); ++u, ++vertex_count) {
		// Figure out how many edges we can add:
		int to_add = std::min(vertex_count, m);
		int added=0;
		// Loop through all the vertices currently in the graph:
		int v = 0;
		while(added < to_add) {
			// If we haven't already connected u and v:
			if(!edge(&slice[u], &slice[v])) {
				// Connect them with P = (d(v)/|E(G)|)^alpha + zero_appeal:
				double p_edge = (double)slice[v].ConnectionList().GetSize() / edge_count;
				p_edge = pow(p_edge, alpha) + zero_appeal;
				// Protect against negative and over-large probabilities:
				assert(p_edge >= 0.0);
				p_edge = std::min(p_edge, 1.0);
				// Probabilistically connect u and v:
				if(rng.P(p_edge)) {
					connect(&slice[u], &slice[v]);
					++edge_count;
					++added;
				}
			}
			// Loop back around to the beginning - note that u is the current end, 'cause
			// building this graph is an iterative process.
			if(++v == u) { v = 0; }
		}		
	}
}


#endif
