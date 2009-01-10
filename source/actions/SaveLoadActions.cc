/*
 *  SaveLoadActions.cc
 *  Avida
 *
 *  Created by David on 5/20/06.
 *  Copyright 1999-2009 Michigan State University. All rights reserved.
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
 Saves the population for cloning.

 Parameters:
   filename (string) default: clone.*
     The name of the file into which the population should
     be saved. If it is not given, then the name 'clone.*'
     is used, with '*' replaced by the current update.
*/
class cActionSaveClone : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveClone(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }

  static const cString GetDescription() { return "Arguments: [string fname='']"; }

  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("clone.%d", m_world->GetStats().GetUpdate());
    m_world->GetPopulation().SaveClone(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


/*
 Loads a population clone.
 
 Parameters:
   filename (string)
     The name of the file to open.
*/
class cActionLoadClone : public cAction
{
private:
  cString m_filename;
  
public:
  cActionLoadClone(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname>"; }
  
  void Process(cAvidaContext& ctx)
  {
    std::ifstream fp(m_filename);
    m_world->GetPopulation().LoadClone(fp);
  }
};


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
  
public:
  cActionLoadPopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename(""), m_update(-1)
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
    if (largs.GetSize()) m_update = largs.PopWord().AsInt();
  }
  
  static const cString GetDescription() { return "Arguments: <cString fname> [int update=-1]"; }
  
  void Process(cAvidaContext& ctx)
  {
    m_world->GetPopulation().LoadDumpFile(m_filename, m_update);
  }
};


/*
 Writes out a line of data for each genotype in the current population. The
 line contains the genome as string, the number of organisms of that genotype,
 and the genotype ID.

 Parameters:
   filename (string) default: "dump-<update>.pop"
    The name of the file into which the population dump should be written.
*/
class cActionDumpPopulation : public cAction
{
private:
  cString m_filename;
  
public:
  cActionDumpPopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("dump-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpTextSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};

/*
 Like dump_pop, but more detailed data is written out.
 
 Parameters:
   filename (string) default: "detail-<update>.pop"
     The name of the file into which the population dump should be written.
*/
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
    if (filename == "") filename.Set("detail-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpDetailedSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


/*
 Like detail_pop, but for sexual populations. 
 Info for both parents is writen out.

 Parameters:
   filename (string) default: "detail-<update>.pop"
     The name of the file into which the population dump should be written.
*/
class cActionSaveSexPopulation : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveSexPopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("detail-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpDetailedSexSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


class cActionSaveParasitePopulation : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveParasitePopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("parasite-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpInjectDetailedSummary(filename, m_world->GetStats().GetUpdate());
  }
};


/*
 Similar to detail_pop. However, only genotypes that are not in the
 current population anymore are included. Genotypes that are not in
 the line of descent of any of the current genotypes to the ultimate
 ancestor are excluded.

 Parameters:
   back_dist (int) default: -1
     How many updates back should we print?  -1 goes forever.  Use the
     distance to the last dump historic if you only want a "diff".
   filename (string) default: "historic-<update>.pop"
     The name of the file into which the historic dump should be written.
*/
class cActionSaveHistoricPopulation : public cAction
{
private:
  int m_backdist;
  cString m_filename;
  
public:
  cActionSaveHistoricPopulation(cWorld* world, const cString& args)
    : cAction(world, args), m_backdist(-1), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_backdist = largs.PopWord().AsInt();
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [int back_dist=-1] [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("historic-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpHistoricSummary(m_world->GetDataFileOFStream(filename), m_backdist);
    m_world->GetDataFileManager().Remove(filename);
  }
};


/*
 Similar to dump_historic_pop, but for sexual populations. 
 ID of both parents is writen out. 

 Parameters:
   filename (string) default: "historic-<update>.pop"
     The name of the file into which the population dump should be written.
*/
class cActionSaveHistoricSexPopulation : public cAction
{
private:
  cString m_filename;
  
public:
  cActionSaveHistoricSexPopulation(cWorld* world, const cString& args) : cAction(world, args), m_filename("")
  {
    cString largs(args);
    if (largs.GetSize()) m_filename = largs.PopWord();
  }
  
  static const cString GetDescription() { return "Arguments: [string fname='']"; }
  
  void Process(cAvidaContext& ctx)
  {
    cString filename(m_filename);
    if (filename == "") filename.Set("historic-%d.pop", m_world->GetStats().GetUpdate());
    m_world->GetClassificationManager().DumpHistoricSexSummary(m_world->GetDataFileOFStream(filename));
    m_world->GetDataFileManager().Remove(filename);
  }
};


void RegisterSaveLoadActions(cActionLibrary* action_lib)
{
  action_lib->Register<cActionSaveClone>("SaveClone");
  action_lib->Register<cActionLoadClone>("LoadClone");

  action_lib->Register<cActionLoadPopulation>("LoadPopulation");

  action_lib->Register<cActionDumpPopulation>("DumpPopulation");
  action_lib->Register<cActionSavePopulation>("SavePopulation");
  action_lib->Register<cActionSaveSexPopulation>("SaveSexPopulation");
  action_lib->Register<cActionSaveParasitePopulation>("SaveParasitePopulation");
  action_lib->Register<cActionSaveHistoricPopulation>("SaveHistoricPopulation");
  action_lib->Register<cActionSaveHistoricSexPopulation>("SaveHistoricSexPopulation");

  // @DMB - The following actions are DEPRECATED aliases - These will be removed in 2.7.
  action_lib->Register<cActionSaveClone>("save_clone");
  action_lib->Register<cActionLoadClone>("load_clone");

  action_lib->Register<cActionLoadPopulation>("load_dump_file");

  action_lib->Register<cActionDumpPopulation>("dump_pop");
  action_lib->Register<cActionSavePopulation>("detail_pop");
  action_lib->Register<cActionSaveSexPopulation>("detail_sex_pop");
  action_lib->Register<cActionSaveParasitePopulation>("detail_parasite_pop");
  action_lib->Register<cActionSaveHistoricPopulation>("dump_historic_pop");
  action_lib->Register<cActionSaveHistoricSexPopulation>("dump_historic_sex_pop");
}
