/*
 *  DriverActions.cc
 *  Avida
 *
 *  Created by David Bryson on 7/19/06.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "DriverActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cStats.h"
#include "cWorld.h"
#include "cWorldDriver.h"

#include <ctime>

class cActionExit : public cAction
{
public:
  cActionExit(cWorld* world, const cString& args) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext& ctx) { m_world->GetDriver().SetDone(); }
};

class cActionPause : public cAction
{
public:
  cActionPause(cWorld* world, const cString& args) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext& ctx) { m_world->GetDriver().SetPause(); }
};

class cActionExitAveLineageLabelGreater : public cAction
{
private:
  double m_threshold;
public:
  cActionExitAveLineageLabelGreater(cWorld* world, const cString& args) : cAction(world, args), m_threshold(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <double threshold>"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_world->GetStats().GetAveLineageLabel() > m_threshold) {
      m_world->GetDriver().SetDone();
    }
  }
};

class cActionExitAveLineageLabelLess : public cAction
{
private:
  double m_threshold;
public:
  cActionExitAveLineageLabelLess(cWorld* world, const cString& args) : cAction(world, args), m_threshold(0.0)
  {
    cString largs(args);
    if (largs.GetSize()) m_threshold = largs.PopWord().AsDouble();
  }
  
  static const cString GetDescription() { return "Arguments: <double threshold>"; }
  
  void Process(cAvidaContext& ctx)
  {
    if (m_world->GetStats().GetAveLineageLabel() < m_threshold) {
      m_world->GetDriver().SetDone();
    }
  }
};

class cActionStopFastForward : public cAction
{
private:
public:
  cActionStopFastForward(cWorld* world, const cString& args) : cAction(world, args)
  {
    cString largs(args);
  }
  
  static const cString GetDescription() { return "Arguments: none"; }
  
  void Process(cAvidaContext& ctx)
  {
      m_world->GetDriver().ClearFastForward();
  }
};


/*! Exit Avida when the average generation is greater than or equal to a
threshold value.  Respects demes / germlines configuration.

MUST appear earlier in event file than PrintGermlineData, if used.

@todo Doesn't currently work with demes w/o germlines.
*/
class cActionExitAveGeneration : public cAction {
public:
  /*! Constructor; parse out the targeted generation.
  */
  cActionExitAveGeneration(cWorld* world, const cString& args) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_tgt_gen = largs.PopWord().AsDouble();
    } else {
      // error; no default value for targeted generation.
      m_world->GetDriver().RaiseFatalException(-1, "ExitAveGeneration event requires generation.");
    }
    
    // Can't currently calc generation for non-germlines demes.
    if(m_world->GetConfig().NUM_DEMES.Get() > 1) {
      assert(m_world->GetConfig().DEMES_USE_GERMLINE.Get());
    }
  }

  static const cString GetDescription() { return "Arguments: <double generation>"; }

  /*! Check to see if we should exit Avida based on the average generation.  The
  average generation is calculated differently based on whether demes / germlines
  are used.  This method is called based on the events file.
  */
  void Process(cAvidaContext& ctx) {
    if(m_world->GetConfig().NUM_DEMES.Get() > 1) {
      // Using demes; generation might be different.
      if(m_world->GetConfig().DEMES_USE_GERMLINE.Get()
         && (m_world->GetStats().GetAveGermlineGeneration() > m_tgt_gen)) {
        m_world->GetDriver().SetDone();
      }
    } else {
      // No demes; generation is calculated in cStats.
      if(m_world->GetStats().GetGeneration() > m_tgt_gen) {
        m_world->GetDriver().SetDone();
      }
    }
  }
  
protected:
  double m_tgt_gen; //!< Target generation above which Avida should exit.
};


/*! Exit Avida when the elapsed wallclock time has exceeded a threshold number
of seconds, beginning from the construction of this object.
*/
class cActionExitElapsedTime : public cAction {
public:
  /*! Constructor; parse out the threshold time.
  */
  cActionExitElapsedTime(cWorld* world, const cString& args) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_time = largs.PopWord().AsInt();
    } else {
      // error; no default value for elapsed time.
      m_world->GetDriver().RaiseFatalException(-1, "ExitElapsedTime event requires elapsed time.");
    }
    
    // When did we start?
    m_then = time(0);
  }
  
  static const cString GetDescription() { return "Arguments: <int elapsed time [seconds]>"; }
  
  /*! Check to see if we should exit Avida based on the elapsed time since construction
  of this object.  This method is called based on the events file.
  */
  void Process(cAvidaContext& ctx) {
    if((time(0) - m_then) >= m_time) {
      m_world->GetDriver().SetDone();
    }
  }
  
protected:
  int m_time; //!< Number of seconds after which Avida should exit.
  int m_then; //!< Time at which this object was constructed (the 'start' of Avida).
};

/*! Exit Avida when a certain number of deme replications has occurred.
 */
class cActionExitDemeReplications : public cAction {
public:
  /*! Constructor; parse out the number of replications.
	 */
  cActionExitDemeReplications(cWorld* world, const cString& args) : cAction(world, args) {
    cString largs(args);
    if(largs.GetSize()) {
      m_deme_rep = largs.PopWord().AsInt();
    } else {
      // error; no default value for elapsed time.
      m_world->GetDriver().RaiseFatalException(-1, "ExitDemeReplications event requires a number of deme replications.");
    }
	}
  
  static const cString GetDescription() { return "Arguments: <int number of deme replications>"; }
  
  /*! Check to see if we should exit Avida based on the number of deme replications. 
	 This method is called based on the events file.
	 */
  void Process(cAvidaContext& ctx) {
    if(m_world->GetStats().GetNumDemeReplications() >= m_deme_rep) {
      m_world->GetDriver().SetDone();
    }
  }
  
protected:
  int m_deme_rep; //!< Number of deme replications after which Avida should exit.
};


void RegisterDriverActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionExit>("Exit");
  action_lib->Register<cActionExitAveLineageLabelGreater>("ExitAveLineageLabelGreater");
  action_lib->Register<cActionExitAveLineageLabelLess>("ExitAveLineageLabelLess");
  action_lib->Register<cActionExitAveGeneration>("ExitAveGeneration");
  action_lib->Register<cActionExitElapsedTime>("ExitElapsedTime");
  action_lib->Register<cActionStopFastForward>("StopFastForward");
	action_lib->Register<cActionExitDemeReplications>("ExitDemeReplications");
  action_lib->Register<cActionPause>("Pause");
}
