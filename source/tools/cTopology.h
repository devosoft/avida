/*
 * cTopology.h
 * Avida
 *
 * Copyright 2005-2010 Michigan State University. All rights reserved.
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
void build_lattice(InputIterator begin, InputIterator end,
		   unsigned int x_size, unsigned int y_size, unsigned int z_size) {
  // First we're going to create z grids each sized x by y:
  unsigned int gridsize = x_size * y_size;

  for (unsigned int i=0; i<z_size; ++i) {
    build_grid(&begin[gridsize*i], &begin[gridsize*(i+1)], x_size, y_size);
  }

  // This is the offset from the beginning of the cell_array; req'd to support demes.
  int offset = begin->GetID();
  
  // Now, iterate through each cell, and link them to their neighbors above and below:
  for (InputIterator i=begin; i!=end; ++i) {
    unsigned int layer = (i->GetID()-offset) / gridsize;
    unsigned int x = (i->GetID()-offset) % x_size;
    unsigned int y = (i->GetID()-offset) / x_size;		
    
    // The below is a big mess.  The reason it's a mess is because we have to respect the boundaries
    // of the grid.  There are probably much cleaner ways to do this, but it's complicated
    // enough now without having to think that through.  And anyway, this is only run
    // at initialization.  Feel free to fix it, however.
    if (layer != 0) {
      // We're not at the bottom; link to the layer below us.
      if (x != 0) {
	if (y != 0) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, -1)]);
	}
	if (y != (y_size-1)) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, 1)]);
	}
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, -1, 0)]);
      }
      
      if (x != (x_size-1)) {
	if (y != 0) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, -1)]);
	}
	if (y != (y_size-1)) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, 1)]);
	}
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 1, 0)]);
      }
			
      if (y != 0) {
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 0, -1)]);
      }
      if(y != (y_size-1)) {
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset-gridsize, x_size, y_size, 0, 1)]);
      }
			
      // And now the cell right below this one:
      i->ConnectionList().Push(&begin[i->GetID()-offset-gridsize]);
    }
    
    if (layer != (z_size-1)) {
      // We're not at the top; link to the layer above us:
      if(x != 0) {
	if (y != 0) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, -1)]);
	}
	if(y != (y_size-1)) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, 1)]);
	}
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, -1, 0)]);
      }
			
      if (x != (x_size-1)) {
	if (y != 0) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, -1)]);
	}
	if (y != (y_size-1)) {
	  i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, 1)]);
	}
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 1, 0)]);
      }
      
      if (y != 0) {
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 0, -1)]);
      }
      if(y != (y_size-1)) {
	i->ConnectionList().Push(&begin[GridNeighbor(i->GetID()-offset+gridsize, x_size, y_size, 0, 1)]);
      }

      // And now the cell right above this one:
      i->ConnectionList().Push(&begin[i->GetID()-offset+gridsize]);
    }
  }
}


/*
 Builds a random connected network topology for organisms to communicate through.
 
 */
template< typename InputIterator >
void build_random_connected_network(InputIterator begin, InputIterator end, unsigned int x_size, unsigned int y_size, cRandom& rng) {
	
	// keep track of boundaries for this deme:
	int offset = begin->GetID();
	int demeSize = x_size * y_size;
	
	// keep track of cells that have been connected already:
	std::set<int> connected_Cells;
	
	InputIterator i, j, random_Connected_Cell;
	
	
	
	for(i = begin; i != end; ++i) {
		// select a random cell in this deme to connect to:
		int targetCellID;
		do {
			targetCellID = rng.GetInt(0, demeSize);
		} while((targetCellID + offset) == i->GetID());
		
		j = &begin[targetCellID];
		
		// verify no connection exists between i and j:
		if(i->ConnectionList().FindPtr(j) == NULL) {
			// create bidirectional connections:
			i->ConnectionList().Push(j);
			j->ConnectionList().Push(i);
			
			// check if either i or j is connected to the
			// main graph:
			if(connected_Cells.count(i->GetID()) == 0 && connected_Cells.count(j->GetID()) == 0) {
				// neither i nor j is connected to the main graph
				
				// check if main network is empty:
				if(connected_Cells.empty()) {
					connected_Cells.insert(i->GetID());
					connected_Cells.insert(j->GetID());
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
					random_Connected_Cell = &begin[idValue-offset];
					
					// randomly select i or j to connect with main network:
					int zeroOrOne = rng.GetInt(0,2);
					
					if(zeroOrOne) {
						// connect i to main network:
						i->ConnectionList().Push(random_Connected_Cell);
						random_Connected_Cell->ConnectionList().Push(i);
					} else {
						// connect j to main network:
						j->ConnectionList().Push(random_Connected_Cell);
						random_Connected_Cell->ConnectionList().Push(j);
					}
					
					// add both cells to the main network:
					// don't care about duplicates...
					connected_Cells.insert(i->GetID());
					connected_Cells.insert(j->GetID());
				}
				
			} else {
				connected_Cells.insert(i->GetID());
				connected_Cells.insert(j->GetID());
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
		
		while(a == b)
			b = rng.GetInt(0,demeSize);
		
		i = &begin[a];
		j = &begin[b];
		
		// check for existing connection between the two:
		if(i->ConnectionList().FindPtr(j) == NULL) {
			i->ConnectionList().Push(j);
			j->ConnectionList().Push(i);
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
template <typename InputIterator>
void build_scale_free(InputIterator begin, InputIterator end, int m, double alpha, double zero_appeal, cRandom& rng) {
	assert(begin != end);
	assert(&begin[1] != end); // at least two vertices.
	// Connect the first and second cells:
	connect(&begin[0], &begin[1]);
	// And initialize the edge and vertex counts:
	int edge_count=1;
	int vertex_count=2;
	
	// Now, for each new vertex (that is, vertices 2+):
	for(InputIterator u=&begin[2]; u!=end; ++u, ++vertex_count) {
		// Figure out how many edges we can add:
		int to_add = std::min(vertex_count, m);
		int added=0;
		// Loop through all the vertices currently in the graph:
		InputIterator v=begin;
		while(added < to_add) {
			// If we haven't already connected u and v:
			if(!edge(u, v)) {
				// Connect them with P = (d(v)/|E(G)|)^alpha + zero_appeal:
				double p_edge = (double)v->ConnectionList().GetSize() / edge_count;
				p_edge = pow(p_edge, alpha) + zero_appeal;
				// Protect against negative and over-large probabilities:
				assert(p_edge >= 0.0);
				p_edge = std::min(p_edge, 1.0);
				// Probabilistically connect u and v:
				if(rng.P(p_edge)) {
					connect(u, v);
					++edge_count;
					++added;
				}
			}
			// Loop back around to the beginning - note that u is the current end, 'cause
			// building this graph is an iterative process.
			if(++v == u) { v = begin; }
		}		
	}
}


#endif
