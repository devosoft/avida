/*
 *  cDeme.h
 *  Avida
 *
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include "cGermline.h"
#include "tArray.h"



/*! Demes are groups of cells in the population that are somehow bound together
as a unit.  The deme object is used from within cPopulation to manage these 
groups. */
class cDeme {
public:
  cDeme();
  ~cDeme();

  void Setup(const tArray<int> & in_cells, int in_width=-1);

  int GetSize() const { return cell_ids.GetSize(); }
  int GetCellID(int pos) const { return cell_ids[pos]; }
  int GetCellID(int x, int y) const;
  //! Returns an (x,y) pair for the position of the passed-in cell ID.
  std::pair<int, int> GetCellPosition(int cellid) const;

  int GetWidth() const { return width; }
  int GetHeight() const { return cell_ids.GetSize() / width; }

  void Reset();
  int GetBirthCount() const { return birth_count; }
  void IncBirthCount() { birth_count++; }

  int GetOrgCount() const { return org_count; }
  void IncOrgCount() { org_count++; }
  void DecOrgCount() { org_count--; }

  bool IsEmpty() const { return org_count == 0; }
  bool IsFull() const { return org_count == cell_ids.GetSize(); }
  
  // -= Germline =-
  //! Returns this deme's germline.
  cGermline& GetGermline() { return _germline; }
  //! Replaces this deme's germline.
  void ReplaceGermline(const cGermline& germline);
  
  // -= Update support =-
  //! Called once, at the end of every update.
  void ProcessUpdate() { ++_age; }
  /*! Returns the age of this deme, updates.  Age is defined as the number of 
    updates since the last time Reset() was called. */
  int GetAge() const { return _age; }
  
private:
  tArray<int> cell_ids;
  int width; //!< Width of this deme.
  int birth_count; //!< Number of organisms that have been born into this deme since reset.
  int org_count; //!< Number of organisms are currently in this deme.
  int _age; //!< Age of this deme, in updates.
  
  cGermline _germline; //!< The germline for this deme, if used.
};

#endif

