/*
 *  DriverActions.cc
 *  Avida
 *
 *  Created by David Bryson on 7/19/06.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

class cActionExit : public cAction
{
public:
  cActionExit(cWorld* world, const cString& args) : cAction(world, args) { ; }
  static const cString GetDescription() { return "No Arguments"; }
  void Process(cAvidaContext& ctx) { m_world->GetDriver().SetDone(); }
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

void RegisterDriverActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionExit>("Exit");
  action_lib->Register<cActionExitAveLineageLabelGreater>("ExitAveLineageLabelGreater");
  action_lib->Register<cActionExitAveLineageLabelLess>("ExitAveLineageLabelLess");
  action_lib->Register<cActionStopFastForward>("StopFastForward");

  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionExit>("exit");
  action_lib->Register<cActionExitAveLineageLabelGreater>("exit_if_ave_lineage_label_larger");
  action_lib->Register<cActionExitAveLineageLabelLess>("exit_if_ave_lineage_label_smaller");
}
