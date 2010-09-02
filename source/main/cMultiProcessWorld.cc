/*
 *  cMultiProcessWorld.cc
 *  Avida
 *
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "defs.h"
#if BOOST_IS_AVAILABLE
#include "cGenome.h"
#include "cOrganism.h"
#include "cMerit.h"
#include "cWorld.h"
#include "cPopulation.h"
#include "cMultiProcessWorld.h"
#include <map>
#include <iostream>
#include <sstream>
#include <boost/optional.hpp>


/*! Constructor.
 
 Since we're running in a multi-process environment from a single command line,
 we need to tweak the random seed and data dirs a bit.
 */
cMultiProcessWorld::cMultiProcessWorld(cAvidaConfig* cfg, boost::mpi::environment& env, boost::mpi::communicator& world) 
	: cWorld(cfg)
	, m_mpi_env(env)
	, m_mpi_world(world) {
}


/*! Message that is sent from one MPI world to another during organism migration.
 */
struct migration_message {
	migration_message() { }
	migration_message(cOrganism* org) {
		_genome = org->GetGenome().AsString();
		//_merit = org->GetPhenotype().GetMerit().GetDouble();
	}
	
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & _genome;
		//ar & _merit;
	}
	
	std::string _genome; //!< Genome of the organism.
	//double _merit; //!< Merit of the organism.
};


/*! Migrate this organism to a different world.
 
 Send this organism to a different world, selected at random.
 */
void cMultiProcessWorld::MigrateOrganism(cOrganism* org) {
	assert(org!=0);
	int dst = GetRandom().GetInt(m_mpi_world.size());
	// the tag is set to the number of messages previously sent; this is to allow
	// the receiver to sort messages for consistency.
	m_reqs.push_back(m_mpi_world.isend(dst, m_reqs.size(), migration_message(org)));
}


/*! Process post-update events.
 
 This method is called after each update of the local population completes.  Here
 we check to see if this world has received any migrants from other worlds, and if
 so, we inject them into the local population.  Note that this is an unconditional
 injection -- that is, migrants are "pushed" to this world.
 
 Migrants are always injected at a random location in this world.
 
 \todo What to do about cross-world lineage labels?
 
 \todo At the moment, this method forces synchronization on update boundaries across
 *all* worlds.  This could slow things down quite a bit, so we may need to consider a
 second mode that relaxes consistency in favor of speed.
 */
void cMultiProcessWorld::ProcessPostUpdate(cAvidaContext& ctx) {
	namespace mpi = boost::mpi;
	using namespace boost;
	
	// wait until we're sure that this process has sent all its messages:
	mpi::wait_all(m_reqs.begin(), m_reqs.end());
	m_reqs.clear();
	
	// at this point, we know that *this* process has sent everything.  but, we don't
	// know if it's *received* everything.  so, we're going to put in a synchronization
	// barrier, which means that every process must reach the barrier before any are allowed
	// to proceed.  since we just finished waiting for all communication to complete,
	// this means that all messages must have been received, too.
	m_mpi_world.barrier();
	
	// now, receive all the messages, but store them in order by source and tag:
	typedef std::map<int,migration_message> rx_tag_t;
	typedef std::map<int,rx_tag_t> rx_src_t;
	rx_src_t recvd;
	optional<mpi::status> s = m_mpi_world.iprobe(mpi::any_source,mpi::any_tag);
	while(s.is_initialized()) {
		migration_message msg;
		m_mpi_world.recv(s->source(), s->tag(), msg);
		recvd[s->source()][s->tag()] = msg;		
		// any others?
		s = m_mpi_world.iprobe(mpi::any_source,mpi::any_tag);		
	}
		
	// iterate over received messages in-order, injecting genomes into our population:
	for(rx_src_t::iterator i=recvd.begin(); i!=recvd.end(); ++i) {
		for(rx_tag_t::iterator j=i->second.begin(); j!=i->second.end(); ++j) {
			//std::cerr << "**********  INJECTING *********** " << i->first << " " << j->first << std::endl;
			// ok, add this migrant to the current population:
			GetPopulation().InjectGenome(GetRandom().GetInt(GetPopulation().GetSize()), // random cell
																	 SRC_ORGANISM_RANDOM, // for right now, we'll treat this as a random organism injection
																	 cGenome(cString(j->second._genome.c_str())), // genome unpacked from message
																	 -1); // lineage label??
		}
	}
	
	// oh, sweet sanity; make sure that we actually processed all messages.
	assert(!m_mpi_world.iprobe(mpi::any_source,mpi::any_tag).is_initialized());
	
	// finally, we need another barrier down here, in the off chance that one of the
	// processes is really speedy and manages to migrate another org to this world
	// before we finished the probe-loop.
	m_mpi_world.barrier();
}

#endif // boost_is_available
