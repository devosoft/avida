/*
 *  cResource.h
 *  Avida
 *
 *  Called "resource.hh" prior to 12/5/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

/*! Classes to hold global and local information about a given resource */

#ifndef cResource_h
#define cResource_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif


/*! class to hold resource information for individual cells (mini-chemostats) */

class cCellResource
{
private:
  int cell_id;
  double initial;
  double inflow;
  double outflow;

public:
  cCellResource();
  cCellResource(int _cell_id, double _initial, double _inflow, double _outflow);
  int GetId() const { return cell_id; }
  double GetInitial() const { return initial; }
  double GetInflow() const { return inflow; }
  double GetOutflow() const { return outflow; }
  void SetInitial(double _initial) { initial = _initial; }
  void SetInflow(double _inflow) { inflow = _inflow; }
  void SetOutflow(double _outflow) { outflow = _outflow; }
};

/* class to hold all information for a single resource */

class cResource
{
private:
  cString name;
  int id;
  double initial;
  double inflow;
  double outflow;
  int geometry;
  int inflowX1;
  int inflowX2;
  int inflowY1;
  int inflowY2;
  int outflowX1;
  int outflowX2;
  int outflowY1;
  int outflowY2;
  double xdiffuse;
  double xgravity;
  double ydiffuse;
  double ygravity;
  bool deme_resource;
  bool energy_resource;  // only implemented for spacial resource
  tArray<cCellResource> cell_list;
  tArray<int> cell_id_list;  
	bool hgt_metabolize;
  
  cResource(); // @not_implemented
  
public:
  cResource(const cString& _name, int _id);
  ~cResource() { ; }

  const cString & GetName() const { return name; }
  int GetID() const { return id; }
  double GetInitial() const { return initial; }
  double GetInflow() const { return inflow; }
  double GetOutflow() const { return outflow; }
  int GetGeometry() const { return geometry; }
  int GetInflowX1() const { return inflowX1; }
  int GetInflowX2() const { return inflowX2; }
  int GetInflowY1() const { return inflowY1; }
  int GetInflowY2() const { return inflowY2; }
  int GetOutflowX1() const { return outflowX1; }
  int GetOutflowX2() const { return outflowX2; }
  int GetOutflowY1() const { return outflowY1; }
  int GetOutflowY2() const { return outflowY2; }
  double GetXDiffuse() const { return xdiffuse; }
  double GetXGravity() const { return xgravity; }
  double GetYDiffuse() const { return ydiffuse; }
  double GetYGravity() const { return ygravity; }
  bool GetDemeResource() const { return deme_resource; }
  bool GetEnergyResource() const { return energy_resource; }
  tArray<cCellResource> *GetCellListPtr() { return &cell_list; }
  tArray<int> *GetCellIdListPtr() { return &cell_id_list; }
	bool GetHGTMetabolize() const { return hgt_metabolize; }

  void SetInitial(double _initial) { initial = _initial; }
  void SetInflow (double _inflow ) { inflow  = _inflow; }
  void SetOutflow(double _outflow) { outflow = _outflow; }
  bool SetGeometry(cString _geometry);
  void SetInflowX1(int _inflowX1) { inflowX1 = _inflowX1; }
  void SetInflowX2(int _inflowX2) { inflowX2 = _inflowX2; }
  void SetInflowY1(int _inflowY1) { inflowY1 = _inflowY1; }
  void SetInflowY2(int _inflowY2) { inflowY2 = _inflowY2; }
  void SetOutflowX1(int _outflowX1) { outflowX1 = _outflowX1; }
  void SetOutflowX2(int _outflowX2) { outflowX2 = _outflowX2; }
  void SetOutflowY1(int _outflowY1) { outflowY1 = _outflowY1; }
  void SetOutflowY2(int _outflowY2) { outflowY2 = _outflowY2; }
  void SetXDiffuse(double _xdiffuse) { xdiffuse = _xdiffuse; }
  void SetXGravity(double _xgravity) { xgravity = _xgravity; }
  void SetYDiffuse(double _ydiffuse) { ydiffuse = _ydiffuse; }
  void SetYGravity(double _ygravity) { ygravity = _ygravity; }
  bool SetDemeResource(cString _deme_resource);
  bool SetEnergyResource(cString _energy_resource);
  void AddCellResource(cCellResource new_cell) { cell_list.Push(new_cell); }
  cCellResource *GetCellResourcePtr(int _id);
  void UpdateCellResource(cCellResource *_CellResoucePtr, double _initial, 
                          double _inflow, double _outflow);
  void SetCellIdList(tArray<int>& id_list); //SLG partial resources
	void SetHGTMetabolize(int _in) { hgt_metabolize = _in; }
};

#endif
