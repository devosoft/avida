/*
 *  cActionLibrary.h
 *  Avida
 *
 *  Created by David on 4/8/06.
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

#ifndef cActionLibrary_h
#define cActionLibrary_h

#include "avida/core/cFeedback.h"

#include "cAction.h"
#include "tObjectFactory.h"

class cWorld;
class cString;


class cActionLibrary
{
private:
  typedef const cString (*ClassDescFunction)();
  
  tObjectFactoryNoCase<cAction* (cWorld*, const cString&, cFeedback&)> m_factory;
  tDictionary<ClassDescFunction> m_desc_funcs;
  
  cActionLibrary() { ; }

  
public:
  ~cActionLibrary() { ; }
  
  static void Initialize();
  static cActionLibrary& GetInstance();
  
  template<typename ClassType> bool Register(const cString& key)
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return false;
    m_desc_funcs.Set(key, &ClassType::GetDescription);
    return m_factory.Register<ClassType>(key);
  }
  bool Unregister(const cString& key)
  {
    m_desc_funcs.Remove(key);
    return m_factory.Unregister(key);
  }
  
  cAction* Create(const cString& key, cWorld* world, const cString& args, cFeedback& feedback)
  {
    return m_factory.Create(key, world, args, feedback);
  }
  
  bool Supports(const cString& key) const { return m_factory.Supports(key); }  
  
  const cString Describe(const cString& key) const
  {
    ClassDescFunction func;
    if (m_desc_funcs.Find(key, func)) return func();
    return "(Not Available)";
  }  
  const cString DescribeAll() const;
  
  
private:
  static cActionLibrary* buildDefaultActionLibrary();
};


#endif
