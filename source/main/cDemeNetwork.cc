/*
 *  cDemeNetwork.cpp
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

#include "cDeme.h"
#include "cDemeNetwork.h"
#include "cWorld.h"
#include "cWorldDriver.h"
// Conditional includes (these use the BGL).
#if BOOST_IS_AVAILABLE
#include "cDemeTopologyNetwork.h"
#endif

/*! Creates a cDemeNetwork object.
 
 WARNING: This method throws an exception if BOOST_IS_AVAILABLE is not 1, as this
 indicates at *least* a configuration error.
 
 WARNING: The returned pointer is owned by the caller (you're responsible for deleting it)!
 */
cDemeNetwork* cDemeNetwork::DemeNetworkFactory(cWorld* world, cDeme& deme) {
#if BOOST_IS_AVAILABLE
	switch(world->GetConfig().DEME_NETWORK_TYPE.Get()) {
		case cDemeNetwork::TOPOLOGY: {
			return new cDemeTopologyNetwork(world, deme);
		}
		default: {
			world->GetDriver().RaiseFatalException(-1, "Unrecognized network type in cDemeNetwork::DemeNetworkFactory().");
		}
	}	
#else
	world->GetDriver().RaiseFatalException(-1, "cDemeNetwork requires Boost; if Boost *is* available, #define BOOST_IS_AVAILABLE in defs.h, and (possibly) update your include path.");
#endif
	return 0; // never reached, to quell warnings only.
}

/*! Constructor.
 */
cDemeNetwork::cDemeNetwork(cWorld* world, cDeme& deme) : m_world(world), m_deme(deme) {
}
