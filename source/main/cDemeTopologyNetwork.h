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
		MIN_CPL=0,
		MAX_CC=1,
		MIN_CC=2,
		TGT_CC=3,
		LENGTH_SUM=4,
	};
	
	//! Internal vertex properties.
	struct vertex_properties {
		vertex_properties() { }
		vertex_properties(std::pair<int,int> pos, int cell_id) : _x(pos.first), _y(pos.second), _cell_id(cell_id), _active_edge(-1) { }
		std::pair<double,double> location() const { return std::make_pair(static_cast<double>(_x),static_cast<double>(_y)); }
		int _x, _y, _cell_id; // coordinates and cell id of this vertex, used to relate it back to the population.
		int _active_edge; // edge that will be used for unicast/rotate/select operations.
	};
	
	//! Internal edge properties
	struct edge_properties {
		edge_properties() { }
		edge_properties(int t) : _t(t) { }
		int _t; //!< Time (update) at which this edge was (last!) added to the network.
	};
	
  //! An ease-of-use typedef to support the distributed construction of a network.
  typedef boost::adjacency_list<boost::setS, boost::vecS, boost::undirectedS, vertex_properties, edge_properties> Network;
  
  //! A map of cell IDs to vertex descriptors.
  typedef std::map<int, Network::vertex_descriptor> CellVertexMap;
  
  //! Map of cell IDs to counts.
  typedef std::map<int, unsigned int> CellCountMap;
	
  //! Function object to remove edges if they've decayed.
	struct edge_decayed {
		edge_decayed(int n, int d, Network& net) : _now(n), _decay(d), _network(net) { }
		bool operator()(Network::edge_descriptor e);
		int _now, _decay;
		Network& _network;
	};
	
	//! Constructor.
	cDemeTopologyNetwork(cWorld* world, cDeme& deme);
	
	//! Destructor.
	virtual ~cDemeTopologyNetwork() { }
	
	//! Called at the end of every update.
	virtual void ProcessUpdate();
	
	//! Connect u->v with weight w.
	virtual void Connect(cPopulationCell& u, cPopulationCell& v, double w=1.0);
	
	//! Broadcast a message to connected cells.
	virtual void BroadcastToNeighbors(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface);

	//! Unicast a message to the currently selected neighbor.
	virtual void Unicast(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface);
	
	//! Rotate the selected link from among the current neighbors.
	virtual void Rotate(cPopulationCell& s, int x);
	
	//! Select the current link from among the neighbors.
	virtual void Select(cPopulationCell& s, int x);
	
	//! Called when the organism living in cell u dies.
	virtual void OrganismDeath(cPopulationCell& u);
	
	//! Returns a network-defined fitness.
	virtual double Fitness(bool record_stats=true) const;
	
	//! Measure statistics of this network.
	virtual cStats::network_stats_t Measure() const;
	
	//! Print this network's topology.
	virtual void PrintTopology(cDataFile& df) const;
	
protected:
	//! Ensure that the active edge of the given vertex is valid.
	bool ActivateEdge(Network::vertex_descriptor u);
	
	Network m_network; //!< Underlying network model.
	CellVertexMap m_cv; //!< Map of cell ids to vertex descriptors.
	double m_link_length_sum; //!< Sum of all link lengths, at connection.
	
private:
	cDemeTopologyNetwork();
	cDemeTopologyNetwork(const cDemeTopologyNetwork&);
	cDemeTopologyNetwork& operator=(const cDemeTopologyNetwork&);
};

#endif
