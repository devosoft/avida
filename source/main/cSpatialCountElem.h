/*
 *  cSpatialCountElem.h
 *  Avida
 *
 *  Called "spatial_count_elem.hh" prior to 12/5/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology.
 *
 */

#ifndef cSpatialCountElem_h
#define cSpatialCountElem_h

#ifndef tArray_h
#include "tArray.h"
#endif

class cSpatialCountElem
{
private:
  mutable double amount, delta;
  tArray<int> elempt, xdist, ydist;
  tArray<double> dist;
  
public:
  cSpatialCountElem();
  cSpatialCountElem(double initamount);
  
  void Rate(double ratein) const { delta += ratein; }
  void State() { amount += delta; delta = 0.0; }
  const double GetAmount() const { return amount; }
  const double SetAmount(double res) const { amount = res; }
    void SetPtr(int innum, int inelempt, int inxdist, int  inydist, double indist);
  int GetElemPtr(int innum) { return elempt[innum]; }
  int GetPtrXdist(int innum) { return xdist[innum]; }
  int GetPtrYdist(int innum) { return ydist[innum]; }
  double GetPtrDist(int innum) { return dist[innum]; }
  friend void FlowMatter(cSpatialCountElem&, cSpatialCountElem&, double, double, double, double,
                         int, int, double);
};


#ifdef ENABLE_UNIT_TESTS
namespace nSpatialCountElem {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
