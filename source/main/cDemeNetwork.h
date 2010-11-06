/*
 *  cDemeNetwork.h
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

#ifndef cDemeNetwork_h
#define cDemeNetwork_h

#include "cStats.h"

class cDeme;
class cDataFile;
class cWorld;
class cPopulationCell;
class cPopulationInterface;

/*! Provides the organisms in a deme with a shared network object.
 
 The idea here is that the organisms in a deme cooperate to construct a network that
 meets certain criteria.  As of this writing, there's only a single "type" of network
 that can be manipulated in this way, but others are in the works.
 
 This can be loosely described as "cooperative construction," but the term "network
 creation" also applies.  See~\cite{knoester2008cooperative} for more details.
 
 Oh - Because this abstraction relies heavily upon the Boost Graph Library (BGL), we play
 some tricks so that we don't require that all Avida developers have Boost.  If
 you want to use cDemeNetwork, either change the #define for #define BOOST_IS_AVAILABLE over in
 defs.h, alter your environment, or change your build settings.  And you actually need
 Boost installed, of course (you'll probably also need to update your include path).
 
 Because of the different types of networks that are available, there's a factory
 method, cDemeNetwork::DemeNetworkFactory() that creates cDemeNetworks.
 */
class cDemeNetwork {
public:
	/*! Available network types.
	 
	 TOPOLOGY: Original network-creation approach; each cell is a node, organisms connect to
	 neighboring cells.  The network's performance is based on its structure (evaluated as a graph).
	 */
	enum NetworkType { TOPOLOGY };
	
	//! Factory method, the only way to create a deme network (pointer is owned by the caller).
	static cDemeNetwork* DemeNetworkFactory(cWorld* world, cDeme& deme);
	
	//! Destructor.
	virtual ~cDemeNetwork() { }

	//! Called at the end of every update.
	virtual void ProcessUpdate() = 0;

	//! Connect u->v with weight w.
	virtual void Connect(cPopulationCell& u, cPopulationCell& v, double w=1.0) = 0;
	
	//! Broadcast a message to connected cells.
	virtual void BroadcastToNeighbors(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface) { }

	//! Unicast a message to the currently selected neighbor.
	virtual void Unicast(cPopulationCell& s, cOrgMessage& msg, cPopulationInterface* pop_interface) { }
	
	//! Rotate the selected link from among the current neighbors.
	virtual void Rotate(cPopulationCell& s, int x) { }
	
	//! Select the current link from among the neighbors.
	virtual void Select(cPopulationCell& s, int x) { }

	//! Called when the organism living in cell u dies.
	virtual void OrganismDeath(cPopulationCell& u) = 0;
	
	//! Returns a network-defined fitness.
	virtual double Fitness(bool record_stats=true) const = 0;
	
	//! Measure statistics of this network.
	virtual cStats::network_stats_t Measure() const = 0;
	
	//! Print this network's topology.
	virtual void PrintTopology(cDataFile& df) const = 0;
	
protected:
	//! Constructor, called by derived classes only.
	cDemeNetwork(cWorld* world, cDeme& deme);
	
	cWorld* m_world; //!< The world.
	cDeme& m_deme; //!< The owning deme.

private:
	cDemeNetwork();
	cDemeNetwork(const cDemeNetwork&);
	cDemeNetwork& operator=(const cDemeNetwork&);	
};

#endif
