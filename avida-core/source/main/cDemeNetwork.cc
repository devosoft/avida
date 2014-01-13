/*
 *  cDemeNetwork.cpp
 *  Avida
 *
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#include "avida/Avida.h"

#include "avida/core/Feedback.h"
#include "avida/core/WorldDriver.h"

#include "cDeme.h"
#include "cDemeNetwork.h"
#include "cWorld.h"

/* By default, Boost is not available.  To enable Boost, either modify your environment,
 alter your build settings, or change this value -- BUT BE CAREFUL NOT TO CHECK IT IN LIKE THAT!
 */
#ifndef BOOST_IS_AVAILABLE
#define BOOST_IS_AVAILABLE 0
#endif

// Conditional includes (these use the BGL).
#if BOOST_IS_AVAILABLE
#include "cDemeTopologyNetwork.h"
#endif

using namespace Avida;


/*! Creates a cDemeNetwork object.
 
 WARNING: This method throws an exception if BOOST_IS_AVAILABLE is not 1, as this
 indicates at *least* a configuration error.
 
 WARNING: The returned pointer is owned by the caller (you're responsible for deleting it)!
 */
cDemeNetwork* cDemeNetwork::DemeNetworkFactory(cWorld* world, cDeme&)
{
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
	world->GetDriver().Feedback().Error("cDemeNetwork requires Boost; if Boost *is* available, #define BOOST_IS_AVAILABLE in defs.h, and (possibly) update your include path.");
  world->GetDriver().Abort(Avida::INTERNAL_ERROR);
#endif
	return 0; // never reached, to quell warnings only.
}

/*! Constructor.
 */
cDemeNetwork::cDemeNetwork(cWorld* world, cDeme& deme) : m_world(world), m_deme(deme) {
}
