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

#include <boost/mpi.hpp>
#include <boost/mpi/environment.hpp>
#include <boost/mpi/communicator.hpp>
#include <vector>

#include "cWorld.h"
#include "cAvidaConfig.h"

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
		cMultiProcessWorld(const cMultiProcessWorld&); // @not_implemented
		cMultiProcessWorld& operator=(const cMultiProcessWorld&); // @not_implemented
		
	protected:
		boost::mpi::environment& m_mpi_env; //!< MPI environment.
		boost::mpi::communicator& m_mpi_world; //!< World-wide MPI communicator.
		std::vector<boost::mpi::request> m_reqs; //!< Requests outstanding since the last ProcessPostUpdate.
		
	public:
		//! Constructor.
		cMultiProcessWorld(cAvidaConfig* cfg, boost::mpi::environment& env, boost::mpi::communicator& world);
		
		//! Destructor.
		virtual ~cMultiProcessWorld() { }
		
		//! Migrate this organism to a different world.
		virtual void MigrateOrganism(cOrganism* org);
		
		//! Process post-update events.
		virtual void ProcessPostUpdate(cAvidaContext& ctx);
		
		// Save to archive 
		template<class Archive>
		void save(Archive & a, const unsigned int version) const {
			a.ArkvObj("m_analyze", m_analyze);
			a.ArkvObj("m_conf", m_conf);
			a.ArkvObj("m_ctx", m_ctx);
			a.ArkvObj("m_class_mgr", m_class_mgr);
			a.ArkvObj("m_data_mgr", m_data_mgr);
			a.ArkvObj("m_env", m_env);
			a.ArkvObj("m_event_list", m_event_list);
			a.ArkvObj("m_hw_mgr", m_hw_mgr);
			a.ArkvObj("m_pop", m_pop);
			a.ArkvObj("m_stats", m_stats);
			a.ArkvObj("m_driver", m_driver);
			a.ArkvObj("m_rng", m_rng);
			int __m_test_on_div = (m_test_on_div == false)?(0):(1);
			int __m_test_sterilize = (m_test_sterilize == false)?(0):(1);
			int __m_own_driver = (m_own_driver == false)?(0):(1);
			a.ArkvObj("m_test_on_div", __m_test_on_div);
			a.ArkvObj("m_test_sterilize", __m_test_sterilize);
			a.ArkvObj("m_own_driver", __m_own_driver);
		}
		
		// Load from archive 
		template<class Archive>
		void load(Archive & a, const unsigned int version){
			a.ArkvObj("m_analyze", m_analyze);
			a.ArkvObj("m_conf", m_conf);
			a.ArkvObj("m_ctx", m_ctx);
			a.ArkvObj("m_class_mgr", m_class_mgr);
			a.ArkvObj("m_data_mgr", m_data_mgr);
			a.ArkvObj("m_env", m_env);
			a.ArkvObj("m_event_list", m_event_list);
			a.ArkvObj("m_hw_mgr", m_hw_mgr);
			a.ArkvObj("m_pop", m_pop);
			a.ArkvObj("m_stats", m_stats);
			a.ArkvObj("m_driver", m_driver);
			a.ArkvObj("m_rng", m_rng);
			int __m_test_on_div;
			int __m_test_sterilize;
			int __m_own_driver;
			a.ArkvObj("m_test_on_div", __m_test_on_div);
			a.ArkvObj("m_test_sterilize", __m_test_sterilize);
			a.ArkvObj("m_own_driver", __m_own_driver);
			m_test_on_div = (__m_test_on_div == 0)?(false):(true);
			m_test_sterilize = (__m_test_sterilize == 0)?(false):(true);
			m_own_driver = (__m_own_driver == 0)?(false):(true);
		} 
    
		// Ask archive to handle loads and saves separately
		template<class Archive>
		void serialize(Archive & a, const unsigned int version){
			a.SplitLoadSave(*this, version);
		}
	};


#ifdef ENABLE_UNIT_TESTS
namespace nMultiProcessWorld {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
