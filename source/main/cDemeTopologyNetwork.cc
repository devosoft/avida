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

#include "defs.h" // Must be first.
#if BOOST_IS_AVAILABLE
#include "cDemeTopologyNetwork.h"
#include "cDemeNetworkUtils.h"
#include "cDeme.h"
#include "cStats.h"
#include "cWorld.h"
#include "cWorldDriver.h"

/*! Constructor.
 */
cDemeTopologyNetwork::cDemeTopologyNetwork(cWorld* world, cDeme& deme) : cDemeNetwork(world, deme) {
}

/*! Connect u->v with weight w.
 */
void cDemeTopologyNetwork::Connect(cPopulationCell& u, cPopulationCell& v, double w) {
}

/*! Returns the fitness of the network, as determined by the DEME_NETWORK_TOPOLOGY_FITNESS config option.
 */
double cDemeTopologyNetwork::Fitness() const {
	// Yay, stats tracking.
	m_world->GetStats().NetworkTopology(m_network);

	// Are we requiring connectedness before fitness is calculated?
	if(m_world->GetConfig().DEME_NETWORK_REQUIRES_CONNECTEDNESS.Get()) {
		// Short-circuit if we don't have all the vertices (cheap!):
		if((boost::num_vertices(m_network) < (size_t)m_deme.GetSize())
			 // and then check for connectedness:
			 || !network_is_connected(m_network)) {
			return 0.0;
		}
	}
	
	// We're going to arbitrarily define the maximum number of edges that can exist
	// in this network to be the number of edges that would be in a fully-connected
	// network of m_deme.GetSize() number of vertices (Euler's formula).
	double max_edges = m_deme.GetSize() * (m_deme.GetSize()-1) / 2;
	
	// Ok, calculate fitness.
	switch(m_world->GetConfig().DEME_NETWORK_TOPOLOGY_FITNESS.Get()) {
		case MIN_SIZE: {
			return pow(max_edges - boost::num_edges(m_network) + 1, 2);
		}
		case MIN_DIAMETER: {
			// Note: we don't need a +1 here (max diameter is less than num_vertices).
			return pow((double)boost::num_vertices(m_network) - diameter(m_network), 2);
		}
		case MIN_CPL: {
			return pow(0.75*boost::num_vertices(m_network) - characteristic_path_length(m_network) + 1, 2);
		}        
		case MAX_CC: {
			return pow(100.0*clustering_coefficient(m_network) + 1, 2);
		}
		case MIN_CC: {
			return pow(100.0 - 100.0*clustering_coefficient(m_network) + 1, 2);
		}
		case TGT_CC: {
			double coeff = clustering_coefficient(m_network);
			double target_coeff = 0.5;
			
			// Want to reward for absolute "closeness" to target.
			// matlab: x=0:0.001:1; target=0.5; plot(x, (power(1/100,abs(target-x))*100).^2)
			return pow(pow(0.01, abs(target_coeff - coeff))*100, 2);
		}
		case CPL_AND_EDGES_1: {
			// Replicate demes that are connected, and set the merit based on a combination of
			// characteristic path length and edge count (1):
			// \[Merit=\Big((CPL_{max}-CPL_{mean}+1) * (L_{max}-L(G)+1)\Big)^2\]
			return pow((0.75*boost::num_vertices(m_network) - characteristic_path_length(m_network) + 1)
															 * (max_edges - boost::num_edges(m_network) + 1), 2);
		}
		case CPL_AND_EDGES_2: {
			// Replicate demes that are connected, and set the merit based on a combination of
			// characteristic path length and edge count (2):
			// \[Merit=(CPL_{max}-CPL_{mean}+1)^2 + (L_{max}-L(G)+1)^2\]
			return pow(0.75*boost::num_vertices(m_network) - characteristic_path_length(m_network) + 1, 2)
													 + pow(max_edges - boost::num_edges(m_network) + 1.0, 2);
		}
		case CPL_AND_EDGES_3: {
			// Replicate demes that are connected, and set the merit based on a combination of
			// characteristic path length and edge count, where each is normalized (3):
			// \[Merit=\Big(100*\frac{CPL_{max}-CPL_{mean}}{CPL_{max}} + 100*\frac{L_{max}-L(G)}{L_{max}}+1\Big)^2\]
			return pow(100*(0.75*boost::num_vertices(m_network)
											- characteristic_path_length(m_network)) / (0.75*boost::num_vertices(m_network))
								 + 100*((max_edges - boost::num_edges(m_network)) / max_edges)
								 + 1
								 , 2);
		}
		case CPL_AND_EDGES_LADDER_1: {
			// Replicate demes that are connected, and set the merit to a ladder fitness function 
			// based on CPL and edge count (min edges -> CPL) (~5% error):
			double source_germline_merit = pow(max_edges - boost::num_edges(m_network) + 1.0, 2);
			// ~5% error margin...
			if(boost::num_edges(m_network) <= (boost::num_vertices(m_network) + 2)) {
				source_germline_merit += pow(0.75*boost::num_vertices(m_network) - characteristic_path_length(m_network) + 1, 2);
			}
			return source_germline_merit;
		}
		case CPL_AND_EDGES_LADDER_2: {
			// Replicate demes that are connected, and set the merit to a ladder fitness function 
			// based on CPL and edge count (min CPL -> edges) (error margin):
			double cpl = characteristic_path_length(m_network);
			double source_germline_merit = pow(0.75*boost::num_vertices(m_network) - cpl + 1, 2);
			if((cpl > 3.25) && (cpl < 3.75)) {
				source_germline_merit += pow(max_edges - boost::num_edges(m_network) + 1.0, 2);
			}
			return source_germline_merit;
		}
		default: {
			m_world->GetDriver().RaiseFatalException(-1, "Unrecognized network fitness type in cDemeTopologyNetwork::Fitness().");
		}
	}
	return 0.0;
}

#endif
