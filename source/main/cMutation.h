//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_HH
#define MUTATION_HH

#ifndef STRING_HH
#include "cString.h"
#endif

class cMutation {
private:
  cString name;
  int id;
  int trigger;
  int scope;
  int type;
  double rate;
public:
  cMutation(const cString & _name, int _id, int _trigger, int _scope,
	    int _type, double _rate);
  ~cMutation();

  const cString & GetName() const { return name; }
  int GetID() const { return id; }
  int GetTrigger() const { return trigger; }
  int GetScope() const { return scope; }
  int GetType() const { return type; }
  double GetRate() const { return rate; }

  /*
  added to satisfy Boost.Python; the semantics are fairly useless --
  equality of two references means that they refer to the same object.
  */
  bool operator==(const cMutation &in) const { return &in == this; }
};

#endif
