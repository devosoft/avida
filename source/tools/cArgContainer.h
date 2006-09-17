/*
 *  cArgContainer.h
 *  Avida
 *
 *  Created by David Bryson on 9/12/06.
 *  Copyright 2006 Michigan State University. All rights reserved.
 *
 */

#ifndef cArgContainer_h
#define cArgContainer_h

#ifndef cString_h
#include "cString.h"
#endif
#ifndef tArray_h
#include "tArray.h"
#endif

class cArgSchema;
template <class T> class tList;

// A generic argument container that will load in named arguments from a string
// into a specific index in an array for the type specified in the argument schema.
class cArgContainer
{
private:
  tArray<int> m_ints;
  tArray<double> m_doubles;
  tArray<cString> m_strings;
  

  inline void SetInt(int i, int v);
  inline void SetDouble(int i, double v);
  inline void SetString(int i, const cString& v);

  
  cArgContainer() { ; }
  cArgContainer(const cArgContainer&); // @not_implemented
  cArgContainer& operator=(const cArgContainer&); // @not_implemented
  
public:
  static cArgContainer* Load(cString args, const cArgSchema& schema, tList<cString>* errors = NULL);
  
  inline int GetInt(int i) const { return m_ints[i]; }
  inline double GetDouble(int i) const { return m_doubles[i]; }
  inline const cString& GetString(int i) const { return m_strings[i]; }
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
