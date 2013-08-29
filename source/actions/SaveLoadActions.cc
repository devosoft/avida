/*
 *  SaveLoadActions.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
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

#include "SaveLoadActions.h"

#include "cAction.h"
#include "cActionLibrary.h"
#include "cArgContainer.h"
#include "cArgSchema.h"
#include "cPopulation.h"
#include "cStats.h"
#include "cStringUtil.h"
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
  bool m_load_groups;
  bool m_load_birth_cells;
  bool m_load_avatars;
  bool m_load_rebirth;
  bool m_load_parent_dat;
  int m_load_traceq;
  
public:
  cActionLoadPopulation(cWorld* world, const cString& args, Feedback&) : cAction(world, args), m_filename(""), m_update(-1), m_cellid_offset(0), m_lineage_offset(0), m_load_groups(0), m_load_birth_cells(0), m_load_avatars(0), m_load_rebirth(0), m_load_parent_dat(0), m_load_traceq(0)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_update = largs.PopWord().AsInt();
    if (largs.GetSize()) m_cellid_offset = largs.PopWord().AsInt();
    if (largs.GetSize()) m_lineage_offset = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_groups = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_birth_cells = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_avatars = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_rebirth = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_parent_dat = largs.PopWord().AsInt();
    if (largs.GetSize()) m_load_traceq = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> [int update=-1] [int cellid_offset=0] [int lineage_offset=0] [bool load_groups=0] [bool load_birth_cells=0] [bool load_avatars] [bool load_rebirth] [bool load_parent_dat] [int load_traceq]"; }
  
  void Process(cAvidaContext& ctx)
  {
    // set the update if requested
    if (m_update >= 0) m_world->GetStats().SetCurrentUpdate(m_update);
    
    if (!m_world->GetPopulation().LoadPopulation(m_filename, ctx, m_cellid_offset, m_lineage_offset, m_load_groups, m_load_birth_cells, m_load_avatars, m_load_rebirth, m_load_parent_dat, m_load_traceq)) {
      m_world->GetDriver().Feedback().Error("failed to load population");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
};

class cActionLoadParasiteGenotypeList : public cAction
{
private:
  cString m_filename;
public:
  cActionLoadParasiteGenotypeList(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().LoadParasiteGenotypeList(m_filename, ctx);
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='flame']"; }

};

class cActionLoadHostGenotypeList : public cAction
{
private:
  cString m_filename;
public:
  cActionLoadHostGenotypeList(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().LoadHostGenotypeList(m_filename, ctx);
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='flame']"; }
  
};

class cActionSavePopulation : public cAction
{
private:
  cString m_filename;
  bool m_save_historic;
  bool m_save_group_info;
  bool m_save_avatars;
  bool m_save_rebirth;
  
public:
  cActionSavePopulation(cWorld* world, const cString& args, Feedback& feedback)
    : cAction(world, args), m_filename(""), m_save_historic(true), m_save_group_info(false), m_save_avatars(false), m_save_rebirth(false)
  {
    cArgSchema schema(':','=');
    
    // String Entries
    schema.AddEntry("filename", 0, "detail");
    
    // Integer Entries
    schema.AddEntry("save_historic", 0, 0, 1, 1);
    schema.AddEntry("save_groups", 1, 0, 1, 0);
    schema.AddEntry("save_avatars", 2, 0, 1, 0);
    schema.AddEntry("save_rebirth", 3, 0, 1, 0);

    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (argc) {
      m_filename = argc->GetString(0);
      m_save_historic = argc->GetInt(0);
      m_save_group_info = argc->GetInt(1);
      m_save_avatars = argc->GetInt(2);
      m_save_rebirth = argc->GetInt(3);
    }
    
    delete argc;
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='detail'] [boolean save_historic=1] [boolean save_groups=0] [boolean save_avatars=0] [boolean save_rebirth=0]"; }
  
  void Process(cAvidaContext&)
  {
    int update = m_world->GetStats().GetUpdate();
    cString filename = cStringUtil::Stringf("%s-%d.spop", (const char*)m_filename, update);
    m_world->GetPopulation().SavePopulation(filename, m_save_historic, m_save_group_info, m_save_avatars, m_save_rebirth);
  }
};


class cActionLoadStructuredSystematicsGroup : public cAction
{
private:
  cString m_filename;
  cString m_role;
  
public:
  cActionLoadStructuredSystematicsGroup(cWorld* world, const cString& args, Feedback& feedback) : cAction(world, args), m_filename(""), m_role("")
  {
    cArgSchema schema(':','=');
    
    // String Entries
    schema.AddEntry("filename", 0, cArgSchema::SCHEMA_STRING);
    schema.AddEntry("role", 1, cArgSchema::SCHEMA_STRING);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (argc) {
      m_filename = argc->GetString(0);
      m_role = argc->GetString(1);
    }
    
    delete argc;
  }
  
  static const cString GetDescription() { return "Arguments: <string fname> <string role>"; }
  
  void Process(cAvidaContext& ctx)
  {    
    if (!m_world->GetPopulation().LoadStructuredSystematicsGroup(ctx, (const char*)m_role, m_filename)) {
      m_world->GetDriver().Feedback().Error("failed to load structured systematics group information");
      m_world->GetDriver().Abort(Avida::INVALID_CONFIG);
    }
  }
};


class cActionSaveStructuredSystematicsGroup : public cAction
{
private:
  cString m_filename;
  cString m_role;

public:
  cActionSaveStructuredSystematicsGroup(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_filename(""), m_role("")
  {
    cArgSchema schema(':','=');
    
    // String Entries
    schema.AddEntry("filename", 0, "");
    schema.AddEntry("role", 0, cArgSchema::SCHEMA_STRING);
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (argc) {
      m_filename = argc->GetString(0);
      m_role = argc->GetString(0);
    }
    
    delete argc;
  }
  
  static const cString GetDescription() { return "Arguments: [string filename=''] <string role>"; }
  
  void Process(cAvidaContext&)
  {
    int update = m_world->GetStats().GetUpdate();
    cString filename = cStringUtil::Stringf("%s-%d.ssg", (const char*)m_role, update);
    if (m_filename.GetSize()) filename = cStringUtil::Stringf("%s-%s", (const char*)m_filename, (const char*)filename);
    m_world->GetPopulation().SaveStructuredSystematicsGroup((const char*)m_role, m_filename);
  }
};

class cActionSaveFlameData : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveFlameData(cWorld* world, const cString& args, Feedback& feedback)
  : cAction(world, args), m_filename("")
  {
    cArgSchema schema(':','=');
    
    // String Entries
    schema.AddEntry("filename", 0, "flame_data");
    
    cArgContainer* argc = cArgContainer::Load(args, schema, feedback);
    
    if (args) {
      m_filename = argc->GetString(0);
    }
  }
  
  static const cString GetDescription() { return "Arguments: [string filename='flame']"; }
  
  void Process(cAvidaContext&)
  {
    int update = m_world->GetStats().GetUpdate();
    cString filename = cStringUtil::Stringf("flamedat/%s-%d.spop", (const char*)m_filename, update);
    m_world->GetPopulation().SaveFlameData(filename);
  }
};

void RegisterSaveLoadActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionLoadParasiteGenotypeList>("LoadParasiteGenotypeList");
  action_lib->Register<cActionLoadHostGenotypeList>("LoadHostGenotypeList");
  action_lib->Register<cActionLoadPopulation>("LoadPopulation");
  action_lib->Register<cActionSavePopulation>("SavePopulation");
  action_lib->Register<cActionLoadStructuredSystematicsGroup>("LoadStructuredSystematicsGroup");
  action_lib->Register<cActionSaveStructuredSystematicsGroup>("SaveStructuredSystematicsGroup");
  action_lib->Register<cActionSaveFlameData>("SaveFlameData");
}
