/*
 *  cSpeciesQueue.h
 *  Avida
 *
 *  Called "species_queue" prior to 11/30/05.
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

#ifndef cSpeciesQueue_h
#define cSpeciesQueue_h

#ifndef defs_h
#include "defs.h"
#endif

class cSpecies;

class cSpeciesQueue {
private:
  int size;
  cSpecies* first;
  
  
  cSpeciesQueue(const cSpeciesQueue&); // @not_implemented
  cSpeciesQueue& operator=(const cSpeciesQueue&); // @not_implemented
  
public:
  cSpeciesQueue() : size(0), first(NULL) { ; }
  ~cSpeciesQueue() { ; }

  void InsertRear(cSpecies & new_species);
  void Remove(cSpecies & in_species);
  void Adjust(cSpecies & in_species);
  void Purge();
  bool OK(int queue_type);

  inline int GetSize() const { return size; }
  inline cSpecies * GetFirst() const { return first; }
};


#ifdef ENABLE_UNIT_TESTS
namespace nSpeciesQueue {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
