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
#include <iostream>
#include <boost/optional.hpp>


/*! Constructor.
 */
cMultiProcessWorld::cMultiProcessWorld(cAvidaConfig* cfg) : cWorld(cfg) {
  //std::cout << "I am process " << m_mpi_world.rank() << " of " << m_mpi_world.size() << "." << std::endl;
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
	m_reqs.push_back(m_mpi_world.isend(dst, 0, migration_message(org)));
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
 configurable sync barrier.
 
 \todo This method has "loose" consistency - the only inconsistent part is related to 
 ordering of message reception.  This can be fixed if we sort by tag / source.
 
 \todo Random seeds; yeeouch.  Will probably have to set random seeds based on rank.
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
	
	// now, iterate over all the messages that we have received:
	optional<mpi::status> s = m_mpi_world.iprobe(mpi::any_source,0);
	while(s.is_initialized()) {
		migration_message msg;
		m_mpi_world.recv(s->source(), s->tag(), msg);

		// ok, add this migrant to the current population:
		int cell_id = GetRandom().GetInt(GetPopulation().GetSize());
		//std::cerr << "**********  INJECTING *********** " << s->source() << " " << m_mpi_world.rank() << std::endl;
		GetPopulation().InjectGenome(GetRandom().GetInt(GetPopulation().GetSize()), // random cell
																 cGenome(cString(msg._genome.c_str())), // genome unpacked from message
																 -1); // lineage label??
		
		// do we have any more messages to process?
		s = m_mpi_world.iprobe(mpi::any_source,0);
	}
}

#endif
