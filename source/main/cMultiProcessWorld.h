/*
 *  cWorld.h
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cMultiProcessWorld_h
#define cMultiProcessWorld_h

/* THIS HEADER REQUIRES BOOST */
#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <boost/timer.hpp>
#include <vector>

#include "cWorld.h"
#include "cAvidaConfig.h"
#include "cStats.h"

/*! Multi-process Avida world.
 
 This class enables multi-process Avida, which provides a mechanism for much larger
 populations than are possible on a single machine.  This is made possible through
 a single new technique, that of "cross-world migration," where an individual organism
 is transferred to a different Avida world and injected into a random location in that
 world's population.
 */
class cMultiProcessWorld : public cWorld
	{
	private:
		cMultiProcessWorld(); // @not_implemented
		cMultiProcessWorld(const cMultiProcessWorld&); // @not_implemented
		cMultiProcessWorld& operator=(const cMultiProcessWorld&); // @not_implemented
		
	protected:
		boost::mpi::environment& m_mpi_env; //!< MPI environment.
		boost::mpi::communicator& m_mpi_world; //!< World-wide MPI communicator.
		std::vector<boost::mpi::request> m_reqs; //!< Requests outstanding since the last ProcessPostUpdate.
		int m_universe_dim; //!< Dimension (x & y) of the universe (number of worlds along the side of a grid of worlds).
		int m_universe_x; //!< X coordinate of this world.
		int m_universe_y; //!< Y coordinate of this world.
		int m_universe_popsize; //!< Total size of the universe, delayed one update.
		
		boost::timer m_update_timer; //!< Tracks the clock-time of updates.
		boost::timer m_post_update_timer; //!< Tracks the clock-time of post-update processing.
		boost::timer m_calc_update_timer; //!< Tracks the clock-time of calculating the update size.
		cStats::profiling_stats_t m_pf; //!< Buffers profiling stats until the post-update step.
		
		//! Constructor (prefer Initialize).
		cMultiProcessWorld(cAvidaConfig* cfg, const cString& cwd, boost::mpi::environment& env, boost::mpi::communicator& worldcomm);

	public:
		//! Create and initialize a cMultiProcessWorld.
		static cMultiProcessWorld* Initialize(cAvidaConfig* cfg, const cString& cwd, boost::mpi::environment& env, boost::mpi::communicator& worldcomm);
		
		//! Destructor.
		virtual ~cMultiProcessWorld() { }
		
		//! Migrate this organism to a different world.
		virtual void MigrateOrganism(cOrganism* org, const cPopulationCell& cell,
																 const cMerit& merit, int lineage);

		//! Returns true if an organism should be migrated to a different world, false otherwise.
		virtual bool TestForMigration();
		
		//! Returns true if the given cell is on the boundary of the world, false otherwise.
		virtual bool IsWorldBoundary(const cPopulationCell& cell);
		
		//! Process post-update events.
		virtual void ProcessPostUpdate(cAvidaContext& ctx);
		
		//! Returns true if this world allows early exits, e.g., when the population reaches 0.
		virtual bool AllowsEarlyExit() const;
		
		//! Calculate the size (in virtual CPU cycles) of the current update.
		virtual int CalculateUpdateSize();
	};

#endif
