/*
 *  viewer-core/cDriver.h
 *  avida-core
 *
 *  Created by David on 10/28/10.
 *  Copyright 2010-2011 Michigan State University. All rights reserved.
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

#ifndef AvidaCoreViewDriver_h
#define AvidaCoreViewDriver_h

#include "avida/core/cWorldDriver.h"

#include "apto/core.h"

class cWorld;


namespace Avida {
  namespace CoreView {

    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------  
    
    class cMap;
    class cListener;


    // Enumerations
    // --------------------------------------------------------------------------------------------------------------  
    
    enum eDriverPauseState {
      DRIVER_PAUSED,
      DRIVER_UNPAUSED
    };

    
    // Driver Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class cDriver : public Apto::Thread, public Avida::cWorldDriver
    {
    private:
      cWorld* m_world;
      
      Apto::Mutex m_mutex;
      Apto::ConditionVariable m_pause_cv;
      eDriverPauseState m_pause_state;
      bool m_done;
      bool m_paused;
      
      cMap* m_map;
      
      Apto::Set<cListener*> m_listeners;
      
      
      cDriver(); // @not_implemented
      cDriver(const cDriver&); // @not_implemented
      cDriver& operator=(const cDriver&); // @not_implemented
      
      
    public:
      cDriver(cWorld* world);
      ~cDriver();
      
      static cDriver* InitWithDirectory(const Apto::String& dir);
      
      
      eDriverPauseState GetPauseState() const { return m_pause_state; }
      bool IsPaused() const { return m_paused; }
      
      void AttachListener(cListener* listener);
      void DetachListener(cListener* listener) { m_listeners.Remove(listener); }

      
      // cWorldDriver Protocol
      // ------------------------------------------------------------------------------------------------------------  
      
    public:
      void SignalBreakpoint() { ; }
      void SetDone() { m_mutex.Lock(); m_done = true; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
      void SetPause() { m_mutex.Lock(); m_pause_state = DRIVER_PAUSED; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
      void Resume() { m_mutex.Lock(); m_pause_state = DRIVER_UNPAUSED; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
      
      void RaiseException(const cString& in_string);
      void RaiseFatalException(int exit_code, const cString& in_string);
      
      // Notifications
      void NotifyComment(const cString& in_string);
      void NotifyWarning(const cString& in_string);
      
      
      // Apto::Thread Interface
      // ------------------------------------------------------------------------------------------------------------  
      
    protected:
      void Run();
    };

  };
};

#endif
