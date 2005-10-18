//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef SPATIAL_COUNT_ELEM_HH
#include "cSpatialCountElem.h"
#endif

const int MAXFLOWPTS = 8;

cSpatialCountElem::cSpatialCountElem (double initamount) {
    amount = initamount;
    delta=0.0;
    elempt.Resize(MAXFLOWPTS);
    xdist.Resize(MAXFLOWPTS);
    ydist.Resize(MAXFLOWPTS);
    dist.Resize(MAXFLOWPTS);
}
cSpatialCountElem::cSpatialCountElem () {
    amount = 0.0;
    delta=0.0;
    elempt.Resize(MAXFLOWPTS);
    xdist.Resize(MAXFLOWPTS);
    ydist.Resize(MAXFLOWPTS);
    dist.Resize(MAXFLOWPTS);
}
void cSpatialCountElem::SetPtr (int innum, int inelempt, int inxdist, 
                        int inydist, double indist) {
  elempt[innum] = inelempt;
  xdist[innum] = inxdist;
  ydist[innum] = inydist;
  dist[innum] = indist;
}
