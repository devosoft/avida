/*
 *  Avida2Driver.h
 *  avida
 *
 *  Created by David on 12/11/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
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

#ifndef Avida2Driver_h
#define Avida2Driver_h

#include "avida/core/Feedback.h"
#include "avida/core/Types.h"
#include "avida/core/WorldDriver.h"

class cWorld;


class Avida2Driver : public Avida::WorldDriver
{
protected:
  cWorld* m_world;
  Avida::World* m_new_world;
  bool m_done;
  
  class StdIOFeedback : public Avida::Feedback
  {
    void Error(const char* fmt, ...);
    void Warning(const char* fmt, ...);
    void Notify(const char* fmt, ...);
  } m_feedback;
  
public:
  Avida2Driver(cWorld* world, Avida::World* new_world);
  ~Avida2Driver();  
  
  // Actions
  void Run();

  void Finish() { m_done = true; }
  void Pause() { return; }
  void Abort(Avida::AbortCondition condition);
  
  // Facilities
  Avida::Feedback& Feedback() { return m_feedback; }

  // Callback
  void RegisterCallback(Avida::DriverCallback callback) { (void)callback; }
};

#endif
