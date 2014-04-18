/*
 *  DriverActions.cc
 *  Avida
 *
 *  Created by David Bryson on 7/19/06.
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

#include "DriverActions.h"

#include "avida/core/EventList.h"
#include "avida/core/Feedback.h"
#include "avida/core/UniverseDriver.h"

#include <ctime>


class ActionExit : public Avida::EventAction
{
public:
  ActionExit(cWorld* world, const cString& args, Feedback&) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext&) { m_world->GetDriver().Finish(); }
};


// Exit Avida when the average generation is greater than or equal to a threshold value.
class ActionExitAveGeneration : public Avida::EventAction
{
private:
  double m_tgt_gen; // Target generation above which Avida should exit.

public:
  ActionExitAveGeneration(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_tgt_gen = largs.PopWord().AsDouble();
    } else {
      // error; no default value for targeted generation.
      m_world->GetDriver().Feedback().Error("ExitAveGeneration event requires generation.");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }

  static const cString GetDescription() { return "Arguments: <double generation>"; }

  void Process(cAvidaContext&) {
      if(m_world->GetStats().GetGeneration() > m_tgt_gen) {
        m_world->GetDriver().Finish();
      }
  }
  
};


// Exit when the elapsed wallclock time has exceeded a threshold number of seconds, beginning from the construction of this object.
class ActionExitElapsedTime : public Avida::EventAction
{
private:
  time_t m_time; // Number of seconds after which Avida should exit.
  time_t m_then; // Time at which this object was constructed (the 'start' of Avida).

public:
  ActionExitElapsedTime(cWorld* world, const cString& args, Feedback&) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_time = largs.PopWord().AsInt();
    } else {
      // error; no default value for elapsed time.
      m_world->GetDriver().Feedback().Error("ExitElapsedTime event requires elapsed time.");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
    
    // When did we start?
    m_then = time(0);
  }
  
  static const cString GetDescription() { return "Arguments: <int elapsed time [seconds]>"; }
  
  void Process(cAvidaContext&) {
    if((time(0) - m_then) >= m_time) {
      m_world->GetDriver().Finish();
    }
  }
  
};




void RegisterDriverActions()
{
  action_lib->Register<ActionExit>("Exit");
  action_lib->Register<ActionExitAveGeneration>("ExitAveGeneration");
  action_lib->Register<ActionExitElapsedTime>("ExitElapsedTime");
  action_lib->Register<ActionPause>("Pause");
}
