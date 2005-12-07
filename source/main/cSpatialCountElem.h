/*
 *  cSpatialCountElem.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cSpatialCountElem_h
#define cSpatialCountElem_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cSpatialCountElem {
  mutable double amount, delta;
  tArray<int> elempt, xdist, ydist;
  tArray<double> dist;
public:
  cSpatialCountElem (double initamount);
  cSpatialCountElem ();
  void Rate (double ratein) const {delta += ratein;}
  void State () {amount += delta; delta = 0.0;}
  const double GetAmount() const {return amount;}
  void SetPtr (int innum, int inelempt, int inxdist, int  inydist, 
               double indist);
  int GetElemPtr (int innum) {return elempt[innum];}
  int GetPtrXdist (int innum) {return xdist[innum];}
  int GetPtrYdist (int innum) {return ydist[innum];}
  double GetPtrDist (int innum) {return dist[innum];}
  friend void FlowMatter (cSpatialCountElem&, cSpatialCountElem&, double, 
                          double, double, double, int, int, double);
};

#endif
