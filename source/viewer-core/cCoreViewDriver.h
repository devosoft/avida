/*
 *  cCoreViewDriver.h
 *  Avida
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010 Michigan State University. All rights reserved.
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

#ifndef cCoreViewDriver_h
#define cCoreViewDriver_h

#include "cAvidaDriver.h"
#include "cWorldDriver.h"

#include "cConditionVariable.h"
#include "cMutex.h"
#include "cThread.h"
#include "tArraySet.h"

class cCoreView_Map;
class cCoreViewListener;
class cWorld;


class cCoreViewDriver : public cThread, public cWorldDriver
{
private:
  cWorld* m_world;
  
  cMutex m_mutex;
  cConditionVariable m_pause_cv;
  eDriverPauseState m_pause_state;
  bool m_done;
  bool m_paused;
  
  cCoreView_Map* m_map;
  
  tArraySet<cCoreViewListener*> m_listeners;

  
  cCoreViewDriver(); // @not_implemented
  cCoreViewDriver(const cCoreViewDriver&); // @not_implemented
  cCoreViewDriver& operator=(const cCoreViewDriver&); // @not_implemented
  
  
public:
  cCoreViewDriver(cWorld* world);
  ~cCoreViewDriver();
  
  // Driver Actions
  void SignalBreakpoint() { ; }
  void SetDone() { m_mutex.Lock(); m_done = true; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
  void SetPause() { m_mutex.Lock(); m_pause_state = DRIVER_PAUSED; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
  void Resume() { m_mutex.Lock(); m_pause_state = DRIVER_UNPAUSED; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
  
  void RaiseException(const cString& in_string);
  void RaiseFatalException(int exit_code, const cString& in_string);
  
  // Notifications
  void NotifyComment(const cString& in_string);
  void NotifyWarning(const cString& in_string);  
  
  
  eDriverPauseState GetPauseState() const { return m_pause_state; }
  bool IsPaused() const { return m_paused; }
  
  void AttachListener(cCoreViewListener* listener);
  void DetachListener(cCoreViewListener* listener) { m_listeners.Remove(listener); }

protected:
  void Run();
};

#endif
