/*
 *  cDeme.cc
 *  Avida
 *
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#include "cDeme.h"

cDeme::cDeme()
  : width(0)
  , birth_count(0)
{
}

cDeme::~cDeme()
{
}

void cDeme::Setup(const tArray<int> & in_cells, int in_width)
{
  cell_ids = in_cells;
  birth_count = 0;

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

