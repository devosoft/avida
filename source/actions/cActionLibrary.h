/*
 *  cActionLibrary.h
 *  Avida
 *
 *  Created by David on 4/8/06.
 *  Copyright 1999-2008 Michigan State University. All rights reserved.
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

#ifndef cActionLibrary_h
#define cActionLibrary_h

#ifndef cAction_h
#include "cAction.h"
#endif
#ifndef tObjectFactory_h
#include "tObjectFactory.h"
#endif

#if USE_tMemTrack
# ifndef tMemTrack_h
#  include "tMemTrack.h"
# endif
#endif


class cWorld;
class cString;

class cActionLibrary
{
#if USE_tMemTrack
  tMemTrack<cActionLibrary> mt;
#endif
private:
  typedef const cString (*ClassDescFunction)();
  
  tObjectFactoryNoCase<cAction* (cWorld*, const cString&)> m_factory;
  tDictionary<ClassDescFunction> m_desc_funcs;

public:
  cActionLibrary() { ; }
  
  template<typename ClassType> bool Register(const cString& key)
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return false;
    m_desc_funcs.Add(key, &ClassType::GetDescription);
    return m_factory.Register<ClassType>(key);
  }
  bool Unregister(const cString& key)
  {
    m_desc_funcs.Remove(key);
    return m_factory.Unregister(key);
  }
  
  cAction* Create(const cString& key, cWorld* world, const cString& args) { return m_factory.Create(key, world, args); }
  bool Supports(const cString& key) const { return m_factory.Supports(key); }  
  
  const cString Describe(const cString& key) const
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return func();
    return "(Not Available)";
  }  
  const cString DescribeAll() const;
  
  static cActionLibrary* ConstructDefaultActionLibrary();
};


#ifdef ENABLE_UNIT_TESTS
namespace nActionLibrary {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

#endif
