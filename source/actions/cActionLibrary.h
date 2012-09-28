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

#include "avida/core/Feedback.h"

#include "cAction.h"
#include "tObjectFactory.h"

class cWorld;
class cString;

using namespace Avida;


class cActionLibrary
{
private:
  typedef const cString (*ClassDescFunction)();
  
  tObjectFactory<cAction* (cWorld*, const cString&, Feedback&)> m_factory;
  Apto::Map<Apto::String, ClassDescFunction> m_desc_funcs;
  
  cActionLibrary() { ; }

  
public:
  ~cActionLibrary() { ; }
  
  static void Initialize();
  static cActionLibrary& GetInstance();
  
  template<typename ClassType> bool Register(const Apto::String& key)
  {
    ClassDescFunction func;
    Apto::String lkey(key.AsLower());
    if (m_desc_funcs.Get(lkey, func)) return false;
    m_desc_funcs.Set(lkey, &ClassType::GetDescription);
    return m_factory.Register<ClassType>(lkey);
  }
  bool Unregister(const Apto::String& key)
  {
    Apto::String lkey(key.AsLower());
    m_desc_funcs.Remove(lkey);
    return m_factory.Unregister(lkey);
  }
  
  cAction* Create(const Apto::String& key, cWorld* world, const cString& args, Feedback& feedback)
  {
    return m_factory.Create(key.AsLower(), world, args, feedback);
  }
  
  bool Supports(const Apto::String& key) const { return m_factory.Supports(key.AsLower()); }
  
  const cString Describe(const Apto::String& key) const
  {
    ClassDescFunction func;
    if (m_desc_funcs.Get(key.AsLower(), func)) return func();
    return "(Not Available)";
  }  
  Apto::String DescribeAll() const;
  
  
private:
  static cActionLibrary* buildDefaultActionLibrary();
};


#endif
