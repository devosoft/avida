/*
 *  cDemeTopologyNetwork.cc
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

#include "Avida.h"

#if BOOST_IS_AVAILABLE
#include <iterator>
#include "cDemeTopologyNetwork.h"
#include "cDemeNetworkUtils.h"
#include "cDeme.h"
#include "cPopulationCell.h"
#include "cPopulationInterface.h"
#include "cStats.h"
#include "cWorld.h"
#include "cWorldDriver.h"
#include "cDataFile.h"

static const char* E="edges [e]";
static const char* V="vertices [v]";
static const char* COMPLETE="complete [complete]";
static const char* CONNECTED="connected [connected]";
static const char* CPL="characteristic path length [cpl]";
static const char* CC="clustering coefficient [cc]";
static const char* LINK_LENGTH_SUM="link length sum [link_sum]";


/*! Constructor.
 */
cDemeTopologyNetwork::cDemeTopologyNetwork(cWorld* world, cDeme& deme)
: cDemeNetwork(world, deme)
, m_link_length_sum(0.0) {
}


/*! Returns true if an edge should be decayed, false otherwise.
 */
bool cDemeTopologyNetwork::edge_decayed::operator()(Network::edge_descriptor e) {
	if((_now - _network[e]._t) > _decay) {
		return true;
	}
	return false;
}


/*! Called at the end of every update.
 
 This removes "old" edges, if so configured.
 */
void cDemeTopologyNetwork::ProcessUpdate() {
	if((m_world->GetConfig().DEME_NETWORK_LINK_DECAY.Get() != 0) && (boost::num_edges(m_network)>0)) {
		boost::remove_edge_if(edge_decayed(m_world->GetStats().GetUpdate(),
																			 m_world->GetConfig().DEME_NETWORK_LINK_DECAY.Get(),
																			 m_network),
													m_network);
	}
}


/*! Called when the organism living in cell u dies.
 */
void cDemeTopologyNetwork::OrganismDeath(cPopulationCell& u) {
	if(m_world->GetConfig().DEME_NETWORK_REMOVE_NODE_ON_DEATH.Get()) {
		CellVertexMap::iterator ui=m_cv.find(u.GetID());
		if(ui!=m_cv.end()) {
			// it would be nice if this worked generally:
			boost::clear_vertex(ui->second, m_network);
			// but, warning: this can trigger a double-delete bug if there are self-loops.
			
			// it would also be nice to do this:
			//			boost::remove_vertex(ui->second, m_network);
			//			m_cv.erase(ui);
			// but because we're using a vecS for the vertex list, this invalidates *all*
			// the other vertex descriptors in m_cv.  we also can't change to a listS
			// because some of the functionality over in cDemeNetworkUtils.h requires
			// a vecS.  oh well.
		}
	}
}


/*! Connect u->v with weight w.
 */
void cDemeTopologyNetwork::Connect(cPopulationCell& u, cPopulationCell& v, double w) {
	// no self-loops:
	if(u.GetID() == v.GetID()) {
		return;
	}
	
	// find or create the vertex for u
	CellVertexMap::iterator ui=m_cv.find(u.GetID());
	if(ui==m_cv.end()) {
		ui = m_cv.insert(std::make_pair(u.GetID(), boost::add_vertex(vertex_properties(u.GetPosition(), u.GetID()), m_network))).first;
	}
	
	// find or create the vertex for v
	CellVertexMap::iterator vi=m_cv.find(v.GetID());
	if(vi==m_cv.end()) {
		vi = m_cv.insert(std::make_pair(v.GetID(), boost::add_vertex(vertex_properties(v.GetPosition(), v.GetID()), m_network))).first;
	}
	
	// sanity
	assert(ui->second != vi->second);
	
	// create the edge if it doesn't already exist
	std::pair<Network::edge_descriptor,bool> e = boost::edge(ui->second, vi->second, m_network);
	if(!e.second) {
		// create the edge
		boost::add_edge(ui->second, vi->second, edge_properties(m_world->GetStats().GetUpdate()), m_network);
		// we have to track link lengths here in order to bypass a bug that's triggered when
		// links decay.  if links decay, the network could actually have dissipated by the time
		// we get around to calculating fitness.
		m_link_length_sum += distance(ui->second, vi->second, m_network);
	} else {
		// update the create time of the edge
		m_network[e.first]._t = m_world->GetStats().GetUpdate();
	}
}


/*! Broadcast a message to connected cells.
 */
void cDemeTopologyNetwork::BroadcastToNeighbors(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface) {
	// if the sender isn't part of the network, we're all done:
	CellVertexMap::iterator ui=m_cv.find(s.GetID());
	if(ui==m_cv.end()) {
		return;
	}
	
	Network::adjacency_iterator ai,ae;
	boost::tie(ai,ae) = boost::adjacent_vertices(ui->second, m_network);
	
	// now, send a message to all the cells that were returned:
	for( ; ai!=ae; ++ai) {
		pop_interface->SendMessage(msg, m_network[*ai]._cell_id);
	}
}


/*! Unicast a message to the currently selected neighbor.
 */
void cDemeTopologyNetwork::Unicast(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface) {
	// if the sender isn't part of the network, we're all done:
	CellVertexMap::iterator ui=m_cv.find(s.GetID());
	if(ui==m_cv.end()) {
		return;
	}
	
	// activate an edge for this vertex; if we can't we're all done.
	if(ActivateEdge(ui->second)) {
		assert(m_network[ui->second]._active_edge >= 0);
		assert(static_cast<std::size_t>(m_network[ui->second]._active_edge) < boost::out_degree(ui->second, m_network));
		Network::adjacency_iterator ai,ae;
		boost::tie(ai,ae) = boost::adjacent_vertices(ui->second, m_network);
		std::advance(ai, m_network[ui->second]._active_edge);
		pop_interface->SendMessage(msg, m_network[*ai]._cell_id);
	}
}


/*! Rotate the selected link from among the current neighbors.
 */
void cDemeTopologyNetwork::Rotate(cPopulationCell& s, int x) {
	// if the cell isn't part of the network, we're all done:
	CellVertexMap::iterator ui=m_cv.find(s.GetID());
	if(ui==m_cv.end()) {
		return;
	}
	
	m_network[ui->second]._active_edge += x;
	ActivateEdge(ui->second);
}


/*! Select the current link from among the neighbors.
 */
void cDemeTopologyNetwork::Select(cPopulationCell& s, int x) {
	// if the cell isn't part of the network, we're all done:
	CellVertexMap::iterator ui=m_cv.find(s.GetID());
	if(ui==m_cv.end()) {
		return;
	}
	
	m_network[ui->second]._active_edge = x;
	ActivateEdge(ui->second);	
}


/*! Ensure that the active edge of the given vertex is valid.
 */
bool cDemeTopologyNetwork::ActivateEdge(Network::vertex_descriptor u) {
	// if we haven't yet initialized this vertex's active_edge, we're done:
	if(m_network[u]._active_edge == -1) {
		return false;
	}
	
	// reset the active edge so that it can't index beyond the bounds of this vertex's degree:
	m_network[u]._active_edge %= boost::out_degree(u, m_network);
	return true;
}


/*! Returns the fitness of the network, as determined by the DEME_NETWORK_TOPOLOGY_FITNESS config option.
 
 This is funky because we want to report as many network stats as we can...
 */
double cDemeTopologyNetwork::Fitness(bool record_stats) const {
	
	bool calc_fitness=true; // are we going to calculate fitness or not?
	cStats::network_stats_t stats; // stats that we'll use for fitness
	
	stats[E] = boost::num_edges(m_network);
	stats[V] = boost::num_vertices(m_network);
	
	// Are we requiring connectedness before fitness is calculated?
	if(m_world->GetConfig().DEME_NETWORK_REQUIRES_CONNECTEDNESS.Get()) {
		// Short-circuit if we don't have all the vertices:
		if(stats[V] >= (std::size_t)m_deme.GetSize()) {
			stats[COMPLETE] = 1.0;
		} else {
			stats[COMPLETE] = 0.0;
			calc_fitness = false;
		}
		
		// short-circuit if we're not connected:
		if(stats[COMPLETE] == 1.0) {
			stats[CONNECTED] = network_is_connected(m_network) ? 1.0 : 0.0;
		} else {
			stats[CONNECTED] = 0.0;
			calc_fitness = false;
		}
	}
	
	switch(m_world->GetConfig().DEME_NETWORK_TOPOLOGY_FITNESS.Get()) {
		case MIN_CPL: { stats[CPL] = calc_fitness ? characteristic_path_length(m_network) : 0.0; break; }
		case MAX_CC: 
		case MIN_CC:
		case TGT_CC: { stats[CC] = calc_fitness ? clustering_coefficient(m_network) : 0.0; break; }
		case LENGTH_SUM: { stats[LINK_LENGTH_SUM] = calc_fitness ? m_link_length_sum : 0.0; break; }
		default: {
			m_world->GetDriver().RaiseFatalException(-1, "Unrecognized network fitness type in cDemeTopologyNetwork::Fitness().");
		}			
	}
	
	// Yay, stats tracking.
	if(record_stats) {
		m_world->GetStats().NetworkTopology(stats);
	}
	
	if(calc_fitness) {
		switch(m_world->GetConfig().DEME_NETWORK_TOPOLOGY_FITNESS.Get()) {
			case MIN_CPL: {
				return pow(0.75*stats[V] - stats[CPL] + 1, 2);
			}        
			case MAX_CC: {
				return pow(100.0*stats[CC] + 1, 2);
			}
			case MIN_CC: {
				return pow(100.0 - 100.0*stats[CC] + 1, 2);
			}
			case TGT_CC: {
				double cc = stats[CC];
				double target_cc = 0.5;
				
				// Want to reward for absolute "closeness" to target.
				// matlab: x=0:0.001:1; target=0.5; plot(x, (power(1/100,abs(target-x))*100).^2)
				// >> plot(x, 1 ./ power(2,abs(target-x)*10))
				// return pow(pow(0.01, abs(target_coeff - coeff))*100, 2);
				return 1.0 / pow(2, abs(target_cc - cc)*10);
			}
			case LENGTH_SUM: {
				return stats[LINK_LENGTH_SUM];
			}
			default: {
				m_world->GetDriver().RaiseFatalException(-1, "Unrecognized network fitness type in cDemeTopologyNetwork::Fitness().");
			}
		}
	}
	return 0.0;
}

/*! Measure statistics of this network.
 */
cStats::network_stats_t cDemeTopologyNetwork::Measure() const {
	cStats::network_stats_t stats;
	
	stats[E] = boost::num_edges(m_network);
	stats[V] = boost::num_vertices(m_network);
	
	if(stats[V] >= (std::size_t)m_deme.GetSize()) {
		stats[COMPLETE] = 1.0;
	} else {
		stats[COMPLETE] = 0.0;
	}
	
	if(stats[COMPLETE] == 1.0) {
		stats[CONNECTED] = network_is_connected(m_network) ? 1.0 : 0.0;
	} else {
		stats[CONNECTED] = 0.0;
	}
	
	if(stats[CONNECTED] == 1.0) {
		stats[CPL] = characteristic_path_length(m_network);
		stats[CC] = clustering_coefficient(m_network);
		stats[LINK_LENGTH_SUM] = m_link_length_sum;
	} else {
		stats[CPL] = 0.0;
		stats[CC] = 0.0;
		stats[LINK_LENGTH_SUM] = 0.0;
	}
	
	return stats;
}

/*! Print this network's topology.
 */
void cDemeTopologyNetwork::PrintTopology(cDataFile& df) const {
	Network::edge_iterator ei,ei_end;
	for(boost::tie(ei,ei_end)=boost::edges(m_network); ei!=ei_end; ++ei) {
		df.Write(m_world->GetStats().GetUpdate(), "Update [update]");
		df.Write(m_deme.GetDemeID(), "Deme ID [deme]");
		df.Write(m_network[boost::source(*ei, m_network)]._cell_id, "Source [src]");
		df.Write(m_network[boost::target(*ei, m_network)]._cell_id, "Destination [dst]");
		df.Write(m_world->GetStats().GetUpdate() - m_network[*ei]._t, "Age [age]");
		df.Endl();
	}
}

#endif
