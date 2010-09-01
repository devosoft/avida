/*
 *  cDemeTopologyNetwork.h
 *  Avida
 *
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#ifndef cDemeTopologyNetwork_h
#define cDemeTopologyNetwork_h

/* THIS HEADER REQUIRES BOOST */
#include <boost/graph/adjacency_list.hpp>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "cDemeNetwork.h"

class cDeme;
class cWorld;
class cPopulationCell;

/*!
 */
class cDemeTopologyNetwork : public cDemeNetwork {
public:	
	enum FitnessType {
		MIN_SIZE,
		MIN_DIAMETER,
		MIN_CPL,
		MAX_CC,
		MIN_CC,
		TGT_CC,
		CPL_AND_EDGES_1,
		CPL_AND_EDGES_2,
		CPL_AND_EDGES_3,
		CPL_AND_EDGES_LADDER_1,
		CPL_AND_EDGES_LADDER_2
	};
	
	//! The internal vertex properties.
	struct vertex_properties {
		vertex_properties() { }
		vertex_properties(std::pair<int,int> pos, int cell_id) : _x(pos.first), _y(pos.second), _cell_id(cell_id) { }
		int _x, _y, _cell_id;
	};
	
  //! An ease-of-use typedef to support the distributed construction of a network.
  typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, vertex_properties> Network;
  
  //! A map of cell IDs to vertex descriptors.
  typedef std::map<int, Network::vertex_descriptor> CellVertexMap;
  
  //! Map of cell IDs to counts.
  typedef std::map<int, unsigned int> CellCountMap;
  
	//! Constructor.
	cDemeTopologyNetwork(cWorld* world, cDeme& deme);
	
	//! Called at the end of every update.
	virtual void ProcessUpdate() { }
	
	//! Connect u->v with weight w.
	virtual void Connect(cPopulationCell& u, cPopulationCell& v, double w=1.0);
	
	//! Called when the organism living in cell u dies.
	virtual void OrganismDeath(cPopulationCell& u) { }
	
	//! Returns a network-defined fitness.
	virtual double Fitness() const;

protected:
	Network m_network; //!< Underlying network model.
	CellVertexMap m_cv; //!< Map of cell ids to vertex descriptors.
};

#endif
