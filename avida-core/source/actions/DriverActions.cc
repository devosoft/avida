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

#include "avida/core/Context.h"
#include "avida/core/EventList.h"
#include "avida/core/Library.h"
#include "avida/core/UniverseDriver.h"
#include "avida/util/ArgParser.h"

#include <ctime>

using namespace Avida;


class ActionExit : public EventAction
{
public:
  ActionExit() { ; }

  static Util::ArgSchema Schema()
  {
    return Util::ArgSchema();
  }
  
  static EventAction* Create(Universe* universe, Context& ctx, Util::Args* args)
  {
    (void)universe;
    (void)ctx;
    delete args;
    return new ActionExit();
  }
  
  void Process(Context& ctx, Update current_update)
  {
    (void)current_update;
    ctx.Driver().Finish();
  }
};


// Exit Avida when the average generation is greater than or equal to a threshold value.
class ActionExitAveGeneration : public EventAction
{
private:
  Universe* m_universe;
  Util::Args* m_args;

public:
  ActionExitAveGeneration(Universe* universe, Util::Args* args) : m_universe(universe), m_args(args) { (void)m_universe; }

  static Util::ArgSchema Schema()
  {
    Util::ArgSchema schema;
    schema.Define("target", Util::DOUBLE);
    
    return schema;
  }
  
  static EventAction* Create(Universe* universe, Context& ctx, Util::Args* args)
  {
    (void)ctx;
    return new ActionExitAveGeneration(universe, args);
  }

  void Process(Context& ctx, Update current_update)
  {
    (void)current_update;
    
    // double ave_generation = m_world->GetStats().GetGeneration();
    double ave_generation = -1.0; assert(false);
    if (ave_generation > m_args->Double(0)) ctx.Driver().Finish();
  }
  
};


// Exit when the elapsed wallclock time has exceeded a threshold number of seconds, beginning from the construction of this object.
class ActionExitElapsedTime : public EventAction
{
private:
  Util::Args* m_args;
  time_t m_start_time; // Time at which this object was constructed (the 'start' of Avida).

public:
  ActionExitElapsedTime(Util::Args* args) : m_args(args)
  {
    m_start_time = time(0);
  }
  
  static Util::ArgSchema Schema()
  {
    Util::ArgSchema schema;
    schema.Define("target", Util::INT);
    
    return schema;
  }
  
  static EventAction* Create(Universe* universe, Context& ctx, Util::Args* args)
  {
    (void)universe;
    (void)ctx;
    return new ActionExitElapsedTime(args);
  }
  
  
  void Process(Context& ctx, Update current_update)
  {
    if ((time(0) - m_start_time) >= m_args->Int(0)) ctx.Driver().Finish();
  }
  
};




void RegisterDriverActions()
{
#define REGISTER_ACTION(ACT) Core::Library::Instance().RegisterEventType(#ACT, Action ## ACT ::Schema(), &Action ## ACT ::Create)
  REGISTER_ACTION(Exit);
  REGISTER_ACTION(ExitAveGeneration);
  REGISTER_ACTION(ExitElapsedTime);
#undef REGISTER_ACTION
}
