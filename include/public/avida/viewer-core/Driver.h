/*
 *  viewer-core/Driver.h
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

#include "avida/core/WorldDriver.h"
#include "avida/data/Recorder.h"

#include "apto/core.h"

class cWorld;


namespace Avida {
  namespace CoreView {

    // Class Declarations
    // --------------------------------------------------------------------------------------------------------------  
    
    class Map;
    class Listener;


    // Enumerations
    // --------------------------------------------------------------------------------------------------------------  
    
    enum eDriverPauseState {
      DRIVER_PAUSED,
      DRIVER_UNPAUSED
    };

    
    // Driver Definition
    // --------------------------------------------------------------------------------------------------------------  
    
    class Driver : public Apto::Thread, public Avida::WorldDriver
    {
    private:
      cWorld* m_world;
      
      Apto::Mutex m_mutex;
      Apto::ConditionVariable m_pause_cv;
      eDriverPauseState m_pause_state;
      bool m_done;
      bool m_paused;
      
      Map* m_map;
      
      Apto::Set<Listener*> m_listeners;
      
      
      Driver(); // @not_implemented
      Driver(const Driver&); // @not_implemented
      Driver& operator=(const Driver&); // @not_implemented
      
      
    public:
      Driver(cWorld* world);
      ~Driver();
      
      static Driver* InitWithDirectory(const Apto::String& dir);
      
      
      eDriverPauseState GetPauseState() const { return m_pause_state; }
      bool IsPaused() const { return m_paused; }
      
      void AttachListener(Listener* listener);
      void DetachListener(Listener* listener) { m_listeners.Remove(listener); }

      void AttachRecorder(Data::RecorderPtr recorder);
      void DetachRecorder(Data::RecorderPtr recorder);

      
      // WorldDriver Protocol
      // ------------------------------------------------------------------------------------------------------------  
      
    public:
      void Finish() { m_mutex.Lock(); m_done = true; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
      void Pause() { m_mutex.Lock(); m_pause_state = DRIVER_PAUSED; m_mutex.Unlock(); m_pause_cv.Broadcast(); }
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
