/*
 *  cSpeciesControl.h
 *  Avida
 *
 *  Called "species_control.hh" prior to 11/30/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
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

#ifndef cSpeciesControl_h
#define cSpeciesControl_h

#ifndef cSpeciesQueue_h
#include "cSpeciesQueue.h"
#endif

class cGenotype;
class cSpecies;
class cStats;
class cWorld;

class cSpeciesControl {
private:
  cWorld* m_world;
  cSpeciesQueue active_queue;
  cSpeciesQueue inactive_queue;
  cSpeciesQueue garbage_queue;
  

  cSpeciesControl(); // @not_implemented
  cSpeciesControl(const cSpeciesControl&); // @not_implemented
  cSpeciesControl& operator=(const cSpeciesControl&); // @not_implemented

public:
  cSpeciesControl(cWorld* world) : m_world(world) { ; }
  ~cSpeciesControl() { ; }

  void Remove(cSpecies & in_species);
  void Adjust(cSpecies & in_species);
  void SetInactive(cSpecies & in_species);
  void SetActive(cSpecies & in_species);
  void SetGarbage(cSpecies & in_species);
  void Purge(cStats & stats);

  bool OK();

  int FindPos(cSpecies & in_species, int max_depth = -1);
  cSpecies * Find(cGenotype & in_genotype, int record_level);

  inline cSpecies * GetFirst() const { return active_queue.GetFirst(); }
  inline cSpecies * GetFirstInactive() const
    { return inactive_queue.GetFirst(); }
  inline cSpecies * GetFirstGarbage() const
    { return garbage_queue.GetFirst(); }
  inline int GetSize() const { return active_queue.GetSize(); }
  inline int GetInactiveSize() const { return inactive_queue.GetSize(); }
};


#ifdef ENABLE_UNIT_TESTS
namespace nSpeciesControl {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
