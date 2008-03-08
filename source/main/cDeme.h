/*
 *  cDeme.h
 *  Avida
 *
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef cDeme_h
#define cDeme_h

#include "cDemeCellEvent.h"
#include "cGermline.h"
#include "cMerit.h"
#include "tArray.h"
#include "cResourceCount.h"
#include "cStringList.h"

class cResource;
class cWorld;

/*! Demes are groups of cells in the population that are somehow bound together
as a unit.  The deme object is used from within cPopulation to manage these 
groups. */

class cDeme
{
private:
  cWorld* m_world;
  int _id; //!< ID of this deme (position in cPopulation::deme_array).
  tArray<int> cell_ids;
  int width; //!< Width of this deme.

// The following should be moved to cDemePhenotype
  int birth_count; //!< Number of organisms that have been born into this deme since reset.
  int org_count; //!< Number of organisms are currently in this deme.
  int _age; //!< Age of this deme, in updates.
  int generation; //!< Generation of this deme
  double total_org_energy; //! total amount of energy in organisms in this deme
  double deme_time_used; //!< number of cpu cycles, normalized by current orgs, this deme has used
// End of phenotypic traits
  
  cGermline _germline; //!< The germline for this deme, if used.

  cDeme(const cDeme&); // @not_implemented
  
  cResourceCount deme_resource_count; //!< Resources available to the deme
  tArray<int> energy_res_ids; //!< IDs of energy resources
  
  tArray<cDemeCellEvent> cell_events;
  
  cMerit _current_merit; //!< Deme merit applied to all organisms living in this deme.
  cMerit _next_merit; //!< Deme merit that will be inherited upon deme replication.
  
public:
  cDeme() : _id(0), width(0), birth_count(0), org_count(0), _age(0), generation(0), total_org_energy(0.0), deme_time_used(0.0), deme_resource_count(0) { ; }
  ~cDeme() { ; }

  void Setup(int id, const tArray<int>& in_cells, int in_width = -1, cWorld* world = NULL);

  int GetID() const { return _id; }
  int GetSize() const { return cell_ids.GetSize(); }
  int GetCellID(int pos) const { return cell_ids[pos]; }
  int GetCellID(int x, int y) const;
  //! Returns an (x,y) pair for the position of the passed-in cell ID.
  std::pair<int, int> GetCellPosition(int cellid) const;

  int GetWidth() const { return width; }
  int GetHeight() const { return cell_ids.GetSize() / width; }

  void Reset(int previous_generation, bool resetResources = true);
  void Reset(double deme_energy, int previous_generation, bool resetResources = true); //! used to pass energy to offspring deme
  //! Kills all organisms currently in this deme.
  void KillAll();
  int GetBirthCount() const { return birth_count; }
  void IncBirthCount() { birth_count++; }

  int GetOrgCount() const { return org_count; }
  void IncOrgCount() { org_count++; }
  void DecOrgCount() { org_count--; }
  
  int GetGeneration() const { return generation; }

  bool IsEmpty() const { return org_count == 0; }
  bool IsFull() const { return org_count == cell_ids.GetSize(); }
  
  // -= Germline =-
  //! Returns this deme's germline.
  cGermline& GetGermline() { return _germline; }
  //! Replaces this deme's germline.
  void ReplaceGermline(const cGermline& germline);
  
  //! Update this deme's merit by rotating the heritable merit to the current merit.
  void UpdateDemeMerit();
  //! Update this deme's merit from the given source; merit will be applied to organisms now.
  void UpdateDemeMerit(cDeme& source);
  //! Update the heritable merit; will be applied to this deme and it's offspring upon replication.
  void UpdateHeritableDemeMerit(double value) { _next_merit = value; }
  //! Retrieve this deme's current merit; to be applied to organisms living in this deme now.
  const cMerit& GetDemeMerit() const { return _current_merit; }
  //! Retrieve this deme's heritable merit.
  const cMerit& GetHeritableDemeMerit() const { return _next_merit; }
  
  bool HasDemeMerit() const { return _current_merit.GetDouble() != 1.0; }

  // -= Update support =-
  //! Called once, at the end of every update.
  void ProcessUpdate();
  /*! Returns the age of this deme, updates.  Age is defined as the number of 
    updates since the last time Reset() was called. */
  int GetAge() const { return _age; }
  
  const cResourceCount& GetDemeResourceCount() const { return deme_resource_count; }
  void SetDemeResourceCount(const cResourceCount in_res) { deme_resource_count = in_res; }
  void ResizeSpatialGrids(const int in_x, const int in_y) { deme_resource_count.ResizeSpatialGrids(in_x, in_y); }
  void ModifyDemeResCount(const tArray<double> & res_change, const int absolute_cell_id);
  double GetAndClearCellEnergy(int absolute_cell_id);
  void GiveBackCellEnergy(int absolute_cell_id, double value);
  void SetupDemeRes(int id, cResource * res, int verbosity);
  void UpdateDemeRes() { deme_resource_count.GetResources(); }
  void Update(double time_step) { deme_resource_count.Update(time_step); }
  int GetRelativeCellID(int absolute_cell_id) { return absolute_cell_id % GetSize(); } //!< assumes all demes are the same size

  void SetCellEvent(int x1, int y1, int x2, int y2, int delay, int duration);
  
  double CalculateTotalEnergy();
  
  void IncTimeUsed() { deme_time_used += 1.0/(double)org_count; }
  double GetTimeUsed() { return deme_time_used; }
};

#endif
