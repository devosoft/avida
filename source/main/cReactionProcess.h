/*
 *  cReactionProcess.h
 *  Avida
 *
 *  Called "reaction_process.hh" prior to 12/5/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
 *
 *
 *  This file is part of Avida.
 *
 *  Avida is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 *
 *  Avida is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with Avida.
 *  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef cReactionProcess_h
#define cReactionProcess_h

class cResourceDef;

class cReactionProcess
{
public:
  enum Type {
    PROCTYPE_ADD = 0,
    PROCTYPE_MULT,
    PROCTYPE_POW,
    PROCTYPE_LIN,
    PROCTYPE_ENZYME,
    PROCTYPE_EXP
  };
  
private:
  cResourceDef* resource;  // Resource consumed.
  double value;          // Efficiency.
  int type;              // Method of applying bonuses.
  double max_number;     // Max quantity of resource usable.
  double min_number;     // Minimum quantity of resource needed (otherwise 0)
  double max_fraction;   // Max fraction of avaiable resources useable.
  double k_sub_m;        // k sub m variable needed for enzyme reaction
  cResourceDef* product;    // Output resource.
  double conversion;     // Conversion factor.
  double lethal;		 // Lethality of reaction
  bool sterilize; //!< Whether performance of this reaction sterilizes the organism.
  cString inst;           // Instruction to be triggered if reaction successful.
  bool depletable;       // Does completing consume resource?
                         // (This is not quite redundant with an infinite resource
                         // because it allows the resource level to be sensed @JEB)
  bool internal;

  // Resource detection
  cResourceDef* detect;    // Resource Measured
  double detection_threshold; // Min quantity of resource to register present
  double detection_error; // Var of Detection Event (as % of resource present)


  cReactionProcess(const cReactionProcess&); // @not_implemented
  cReactionProcess& operator=(const cReactionProcess&); // @not_implemented

public:
  cReactionProcess()
    : resource(NULL)
    , value(1.0)
    , type(PROCTYPE_ADD)
    , max_number(1.0)
    , min_number(0.0)
    , max_fraction(1.0)
    , k_sub_m(0.0)
    , product(NULL)
    , conversion(1.0)
    , lethal(0)
    , sterilize(false)
    , depletable(true)
    , internal(false)
    , detect(NULL)
    , detection_threshold(0.0)
    , detection_error(0.0)
  {
  }
  ~cReactionProcess() { ; }

  cResourceDef* GetResDef() const { return resource; }
  double GetValue() const { return value; }
  int GetType() const { return type; }
  double GetMaxNumber() const { return max_number; }
  double GetMinNumber() const { return min_number; }
  double GetMaxFraction() const { return max_fraction; }
  double GetKsubM() const { return k_sub_m; }
  cResourceDef* GetProduct() const { return product; }
  double GetConversion() const { return conversion; }
  const cString& GetInst() const { return inst; }
  bool GetDepletable() const { return depletable; }
  double GetLethal() const { return lethal; }
  bool GetSterilize() const { return sterilize; }
  cResourceDef* GetDetect() const { return detect; }
  double GetDetectionThreshold() const { return detection_threshold; }
  double GetDetectionError() const { return detection_error; }
  double GetInternal() const { return internal; }

  void SetResourceDef(cResourceDef* _in) { resource = _in; }
  void SetValue(double _in) { value = _in; }
  void SetType(int _in) { type = _in; }
  void SetMaxNumber(double _in) { max_number = _in; }
  void SetMinNumber(double _in) { min_number = _in; }
  void SetMaxFraction(double _in) { max_fraction = _in; }
  void SetKsubM(double _in) { k_sub_m = _in; }
  void SetProductDef(cResourceDef* _in) { product = _in; }
  void SetConversion(double _in) { conversion = _in; }
  void SetInst(const cString& _in) { inst = _in; }
  void SetDepletable(bool _in) { depletable = _in; }
  void SetLethal(double _in) { lethal = _in; }
  void SetSterile(int _in) { sterilize = _in; }
  void SetDetect(cResourceDef* _in) { detect = _in; }
  void SetDetectionThreshold(double _in) { detection_threshold = _in; }
  void SetDetectionError(double _in) { detection_error = _in; }
  void SetInternal(bool _in) { internal = _in; }
};

#endif
