/*
 *  core/cWorldDriver.h
 *  Avida
 *
 *  Created by David on 12/10/05.
 *  Copyright 2005-2011 Michigan State University. All rights reserved.
 *  http://avida.devosoft.org/
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
 *  Authors: David M. Bryson <david@programerror.com>
 *
 */

#ifndef AvidaCoreWorldDriver_h
#define AvidaCoreWorldDriver_h

#include "avida/core/cDriverManagerObject.h"

#include <iostream>

class cString;


// This class is an abstract base class that is used by actions within
// a cWorld to notify its driver of various states and conditions.

enum eDriverPauseState {
  DRIVER_PAUSED,
  DRIVER_UNPAUSED
};

class cWorldDriver : public virtual cDriverManagerObject
{
private:
  cWorldDriver(const cWorldDriver&); // @not_implemented
  cWorldDriver& operator=(const cWorldDriver&); // @not_implemented
  
public:
  cWorldDriver() { ; }
  virtual ~cWorldDriver() { ; }
  
  // Driver Actions
  virtual void SignalBreakpoint() = 0;
  virtual void SetDone() = 0;
  virtual void SetPause() = 0;

  virtual void RaiseException(const cString& in_string) = 0;
  virtual void RaiseFatalException(int exit_code, const cString& in_string) = 0;
  
  // Notifications
  virtual void NotifyComment(const cString& in_string) = 0;
  virtual void NotifyWarning(const cString& in_string) = 0;

  // Input/Output
  virtual bool IsInteractive() { return false; }
  virtual void Flush() { std::cout.flush(); std::cerr.flush(); }
  virtual bool ProcessKeypress(int keypress) { return false; }
  
  // Fast-forward through epochs when no replication is happening -- @JEB
  // These are only implemented in the DefaultWorldDriver
  virtual void ClearFastForward() { }
  virtual bool GetFastForward() { return false; }

};

#endif
