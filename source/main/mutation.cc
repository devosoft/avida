//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef MUTATION_HH
#include "mutation.hh"
#endif


///////////////
//  cMutation
///////////////

cMutation::cMutation(const cString & _name, int _id, int _trigger,
		     int _scope, int _type, double _rate)
  : name(_name)
    , id(_id)
    , trigger(_trigger)
    , scope(_scope)
    , type(_type)
    , rate(_rate)
{
}

cMutation::~cMutation()
{
}
