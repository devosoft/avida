/*
 *  cDeme.cc
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

#include "cDeme.h"

cDeme::cDeme()
  : width(0)
  , birth_count(0)
  , org_count(0)
{
}

cDeme::~cDeme()
{
}

void cDeme::Setup(const tArray<int> & in_cells, int in_width)
{
  cell_ids = in_cells;
  birth_count = 0;
  org_count = 0;

  // If width is negative, set it to the full number of cells.
  width = in_width;
  if (width < 1) width = cell_ids.GetSize();
}

int cDeme::GetCellID(int x, int y) const
{
  assert(x >= 0 && x < GetWidth());
  assert(y >= 0 && y < GetHeight());

  const int pos = y * width + x;
  return cell_ids[pos];
}

