/*
 *  cCoreViewDriver.h
 *  Avida
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cCoreViewDriver_h
#define cCoreViewDriver_h

#include "cAvidaDriver.h"
#include "cWorldDriver.h"

class cWorld;


class cCoreViewDriver : public cAvidaDriver, public cWorldDriver
{
private:
  cWorld* m_world;
  eDriverPauseState m_pause_state;
  bool m_paused;
  
  
  cCoreViewDriver(); // @not_implemented
  cCoreViewDriver(const cCoreViewDriver&); // @not_implemented
  cCoreViewDriver& operator=(const cCoreViewDriver&); // @not_implemented
  
  
public:
  cCoreViewDriver(cWorld* world);
  ~cCoreViewDriver();
  
  void Run();
  
  // Driver Actions
  void SignalBreakpoint() { ; }
  void SetDone() { ; }
  void SetPause() { m_pause_state = DRIVER_PAUSED; m_paused = true; }
  void Resume() { m_pause_state = DRIVER_UNPAUSED; m_paused = false; }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);  
  
  
  eDriverPauseState GetPauseState() const { return m_pause_state; }
  bool IsPaused() const { return m_paused; }
};

#endif
