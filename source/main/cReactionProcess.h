/*
 *  cReactionProcess.h
 *  Avida
 *
 *  Created by David on 12/5/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2004 California Institute of Technology.
 *
 */

#ifndef cReactionProcess_h
#define cReactionProcess_h

#ifndef defs_h
#include "defs.h"
#endif
#ifndef nReaction_h
#include "nReaction.h"
#endif

#include <iostream>

using namespace std;

class cResource;

class cReactionProcess {
private:
  cResource* resource;  // Resource consumed.
  double value;          // Efficiency.
  int type;              // Method of applying bonuses.
  double max_number;     // Max quantity of resource usable.
  double min_number;     // Minimum quantity of resource needed (otherwise 0)
  double max_fraction;   // Max fraction of avaiable resources useable.
  cResource * product;   // Output resource.
  double conversion;     // Conversion factor.
  bool lethal;		 // Lethality of reaction
  int inst_id;           // Instruction to be triggered if reaction successful.

  // Resource detection
  cResource * detect;    // Resource Measured
  double detection_threshold; // Min quantity of resource to register present
  double detection_error; // Var of Detection Event (as % of resource present)
public:
  cReactionProcess()
    : resource(NULL)
    , value(1.0)
    , type(nReaction::PROCTYPE_ADD)
    , max_number(1.0)
    , min_number(0.0)
    , max_fraction(1.0)
    , product(NULL)
    , conversion(1.0)
    , lethal(0)
    , inst_id(-1)
    , detect(NULL)
    , detection_threshold(0.0)
    , detection_error(0.0)
  {
  }
  ~cReactionProcess() { ; }

  cResource * GetResource() const { return resource; }
  double GetValue() const { return value; }
  int GetType() const { return type; }
  double GetMaxNumber() const { return max_number; }
  double GetMinNumber() const { return min_number; }
  double GetMaxFraction() const { return max_fraction; }
  cResource * GetProduct() const { return product; }
  double GetConversion() const { return conversion; }
  int GetInstID() const { return inst_id; }
  bool GetLethal() const { return lethal; }
  cResource * GetDetect() const { return detect; }
  double GetDetectionThreshold() const { return detection_threshold; }
  double GetDetectionError() const { return detection_error; }

  void SetResource(cResource * _in) { resource = _in; }
  void SetValue(double _in) { value = _in; }
  void SetType(int _in) { type = _in; }
  void SetMaxNumber(double _in) { max_number = _in; }
  void SetMinNumber(double _in) { min_number = _in; }
  void SetMaxFraction(double _in) { max_fraction = _in; }
  void SetProduct(cResource * _in) { product = _in; }
  void SetConversion(double _in) { conversion = _in; }
  void SetInstID(int _in) { inst_id = _in; }
  void SetLethal(int _in) { lethal = _in; }
  void SetDetect(cResource * _in) { detect = _in; }
  void SetDetectionThreshold(double _in) { detection_threshold = _in; }
  void SetDetectionError(double _in) { detection_error = _in; }
};

#endif
