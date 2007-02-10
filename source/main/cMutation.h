/*
 *  cMutation.h
 *  Avida
 *
 *  Called "mutation.hh" prior to 12/5/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef cMutation_h
#define cMutation_h

#ifndef cString_h
#include "cString.h"
#endif

class cMutation
{
private:
  cString name;
  int id;
  int trigger;
  int scope;
  int type;
  double rate;


  cMutation(); // @not_implemented
  
public:
  cMutation(const cString& _name, int _id, int _trigger, int _scope, int _type, double _rate)
    : name(_name), id(_id), trigger(_trigger), scope(_scope), type(_type), rate(_rate)
  {
  }
  ~cMutation() { ; }

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


#ifdef ENABLE_UNIT_TESTS
namespace nMutation {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif

#endif
