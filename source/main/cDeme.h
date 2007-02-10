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

// DESCRIPTION:
// Demes are groups of cells in the population that are somehow bound togehter
// as a unit.  The deme object is used from withing cPopulation to manage these
// groups.

#ifndef cDeme_h
#define cDeme_h

#ifndef tArray_h
#include "tArray.h"
#endif


class cDeme {
private:
  tArray<int> cell_ids;
  int width;        // How wide is the deme?
  int birth_count;  // How many organisms have been born into deme since reset?
  int org_count;    // How many organisms are currently in the deme?

public:
  cDeme();
  ~cDeme();

  void Setup(const tArray<int> & in_cells, int in_width=-1);

  int GetSize() const { return cell_ids.GetSize(); }
  int GetCellID(int pos) const { return cell_ids[pos]; }
  int GetCellID(int x, int y) const;

  int GetWidth() const { return width; }
  int GetHeight() const { return cell_ids.GetSize() / width; }

  void Reset() { birth_count = 0; }
  int GetBirthCount() const { return birth_count; }
  void IncBirthCount() { birth_count++; }

  int GetOrgCount() const { return org_count; }
  void IncOrgCount() { org_count++; }
  void DecOrgCount() { org_count--; }

  bool IsEmpty() const { return org_count == 0; }
  bool IsFull() const { return org_count == cell_ids.GetSize(); }
};

#endif

