/*
 *  cArgContainer.h
 *  Avida
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef cArgContainer_h
#define cArgContainer_h

#include "cString.h"

namespace Avida {
  class Feedback;
};

class cArgSchema;
template <class T> class tList;

using namespace Avida;


// A generic argument container that will load in named arguments from a string
// into a specific index in an array for the type specified in the argument schema.
class cArgContainer
{
private:
  Apto::Array<int> m_ints;
  Apto::Array<double> m_doubles;
  Apto::Array<cString> m_strings;

  
  cArgContainer() { ; }
  cArgContainer(const cArgContainer&); // @not_implemented
  cArgContainer& operator=(const cArgContainer&); // @not_implemented
  
public:
  static cArgContainer* Load(cString args, const cArgSchema& schema, Feedback& feedback);
  
  inline int GetInt(int i) const { return m_ints[i]; }
  inline double GetDouble(int i) const { return m_doubles[i]; }
  inline const cString& GetString(int i) const { return m_strings[i]; }

  inline void SetInt(int i, int v);
  inline void SetDouble(int i, double v);
  inline void SetString(int i, const cString& v);  
};


inline void cArgContainer::SetInt(int i, int v)
{
  if (m_ints.GetSize() <= i) m_ints.Resize(i + 1);
  m_ints[i] = v;
}

inline void cArgContainer::SetDouble(int i, double v)
{
  if (m_doubles.GetSize() <= i) m_doubles.Resize(i + 1);
  m_doubles[i] = v;
}

inline void cArgContainer::SetString(int i, const cString& v)
{
  if (m_strings.GetSize() <= i) m_strings.Resize(i + 1);
  m_strings[i] = v;
}

#endif
