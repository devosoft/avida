/*
 *  SaveLoadActions.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
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

#include "SaveLoadActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cClassificationManager.h"
#include "cPopulation.h"
#include "cStats.h"
#include "cWorld.h"

#include <iostream>


/*
 Sets up a population based on a dump file such as written out by
 detail_pop. It is also possible to append a history file to the dump
 file, in order to preserve the history of a previous run.
 
 Parameters:
   filename (string)
     The name of the file to open.
   update (int) *optional*
     ??
 */
class cActionLoadPopulation : public cAction
{
private:
  cString m_filename;
  int m_update;
  int m_cellid_offset;
  int m_lineage_offset;
  
public:
  cActionLoadPopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename(""), m_update(-1), m_cellid_offset(0), m_lineage_offset(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_update = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cellid_offset = largs.PopWord().AsInt();
    if (largs.GetSize()) m_lineage_offset = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> [int update=-1] [int cellid_offset=0] [int lineage_offset=0]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // set the update if requested
    if (m_update >= 0) m_world->GetStats().SetCurrentUpdate(m_update);
    
    if (!m_world->GetPopulation().LoadPopulation(m_filename, m_cellid_offset, m_lineage_offset)) {
      m_world->GetDriver().RaiseFatalException(-1, "failed to load population");
    }
  }
};



class cActionSavePopulation : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSavePopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("detail-%d.spop", m_world->GetStats().GetUpdate());
    m_world->GetPopulation().SavePopulation(filename);
  }
};


void RegisterSaveLoadActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionLoadPopulation>("LoadPopulation");
  action_lib->Register<cActionSavePopulation>("SavePopulation");
}
