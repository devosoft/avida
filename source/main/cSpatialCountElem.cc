/*
 *  cSpatialCountElem.cc
 *  Avida
 *
 *  Called "spatial_count_elem.cc" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#include "cSpatialCountElem.h"

const int MAXFLOWPTS = 8;


/* Intial spatial resource count elements with an initial amount */

cSpatialCountElem::cSpatialCountElem(double initamount)
{
  amount = initamount;
  delta = 0.0;
  elempt.Resize(MAXFLOWPTS);
  xdist.Resize(MAXFLOWPTS);
  ydist.Resize(MAXFLOWPTS);
  dist.Resize(MAXFLOWPTS);
}

/* Intial spatial resource count elements without an initial amount */

cSpatialCountElem::cSpatialCountElem()
{
  amount = 0.0;
  delta = 0.0;
  elempt.Resize(MAXFLOWPTS);
  xdist.Resize(MAXFLOWPTS);
  ydist.Resize(MAXFLOWPTS);
  dist.Resize(MAXFLOWPTS);
}

/* Set pointer to one other elements from the current element */  

void cSpatialCountElem::SetPtr(int innum, int inelempt, int inxdist, int inydist, double indist)
{
  elempt[innum] = inelempt;
  xdist[innum] = inxdist;
  ydist[innum] = inydist;
  dist[innum] = indist;
}
