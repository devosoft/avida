/*
 *  cFallbackWorldDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
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

#ifndef cFallbackWorldDriver_h
#define cFallbackWorldDriver_h

#include "avida/core/cWorldDriver.h"

using namespace Avida;


class cFallbackWorldDriver : public cWorldDriver
{
private:
  cFallbackWorldDriver(const cFallbackWorldDriver&); // @not_implemented
  cFallbackWorldDriver& operator=(const cFallbackWorldDriver&); // @not_implemented
  
public:
  cFallbackWorldDriver() { GlobalObjectManager::Register(this); }
  ~cFallbackWorldDriver() { GlobalObjectManager::Unregister(this); }
  
  // Driver Actions
  void SignalBreakpoint() { return; }
  void SetDone() { return; }
  void SetPause() { return; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);
};

#endif
