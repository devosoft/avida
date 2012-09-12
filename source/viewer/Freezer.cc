/*
 *  viewer/Freezer.cc
 *  Avida
 *
 *  Created by David on 1/4/12.
 *  Copyright 2012 Michigan State University. All rights reserved.
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
 */

#include "avida/viewer/Freezer.h"

#include "avida/core/Genome.h"

#include "cAvidaConfig.h"
#include "cEnvironment.h"
#include "cFile.h"
#include "cPopulation.h"
#include "cWorld.h"


namespace Avida {
  namespace Viewer {
    namespace Private {
      
      void WriteInstSet(const Apto::String& path)
      {
        cFile file;
        std::fstream* fs;
        
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "instset.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "INSTSET heads_default:hw_type=0" << std::endl;
        *fs << "" << std::endl;
        *fs << "# No-ops" << std::endl;
        *fs << "INST nop-A         # a" << std::endl;
        *fs << "INST nop-B         # b" << std::endl;
        *fs << "INST nop-C         # c" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Flow control operations" << std::endl;
        *fs << "INST if-n-equ      # d" << std::endl;
        *fs << "INST if-less       # e" << std::endl;
        *fs << "INST if-label      # f" << std::endl;
        *fs << "INST mov-head      # g" << std::endl;
        *fs << "INST jmp-head      # h" << std::endl;
        *fs << "INST get-head      # i" << std::endl;
        *fs << "INST set-flow      # j" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Single Argument Math" << std::endl;
        *fs << "INST shift-r       # k" << std::endl;
        *fs << "INST shift-l       # l" << std::endl;
        *fs << "INST inc           # m" << std::endl;
        *fs << "INST dec           # n" << std::endl;
        *fs << "INST push          # o" << std::endl;
        *fs << "INST pop           # p" << std::endl;
        *fs << "INST swap-stk      # q" << std::endl;
        *fs << "INST swap          # r" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Double Argument Math" << std::endl;
        *fs << "INST add           # s" << std::endl;
        *fs << "INST sub           # t" << std::endl;
        *fs << "INST nand          # u" << std::endl;
        *fs << "" << std::endl;
        *fs << "# Biological Operations" << std::endl;
        *fs << "INST h-copy        # v" << std::endl;
        *fs << "INST h-alloc       # w" << std::endl;
        *fs << "INST h-divide      # x" << std::endl;
        *fs << "" << std::endl;
        *fs << "# I/O and Sensory" << std::endl;
        *fs << "INST IO            # y" << std::endl;
        *fs << "INST h-search      # z" << std::endl;
        
        file.Close();
      }
      
      void WriteDefaultConfig(const Apto::String& path, const Apto::String& name)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        
        // avida.cfg
        file_path = Apto::FileSystem::PathAppend(path, "avida.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "WORLD_X 60" << std::endl;
        *fs << "WORLD_Y 60" << std::endl;
        *fs << "WORLD_GEOMETRY 1" << std::endl;
        *fs << "COPY_MUT_PROB 0.0075" << std::endl;
        *fs << "DIVIDE_INS_PROB 0.0" << std::endl;
        *fs << "DIVIDE_DEL_PROB 0.0" << std::endl;
        *fs << "OFFSPRING_SIZE_RANGE 1.0" << std::endl;
        *fs << "BIRTH_METHOD 0" << std::endl;
        *fs << "RANDOM_SEED 0" << std::endl;
        *fs << "#include instset.cfg" << std::endl;
        
        file.Close();

        
        // instset.cfg
        WriteInstSet(path);
        
        
        // events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        file.Close();

        
        // environment.cfg
        file_path = Apto::FileSystem::PathAppend(path, "environment.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "REACTION  NOT  not   process:value=1.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NAND nand  process:value=1.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  AND  and   process:value=2.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ORN  orn   process:value=2.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  OR   or    process:value=3.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ANDN andn  process:value=3.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NOR  nor   process:value=4.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  XOR  xor   process:value=4.0:type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  EQU  equ   process:value=5.0:type=pow  requisite:max_count=1" << std::endl;
        
        file.Close();        

        
        // entryname.txt
        file_path = Apto::FileSystem::PathAppend(path, "entryname.txt");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << name << std::endl;
        file.Close();
      }
      
      
      void WriteDefaultGenome(const Apto::String& path)
      {
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "genome.seq");
        cFile file;
        file.Open((const char*)file_path, std::ios::out);
        std::fstream* fs = file.GetFileStream();
        *fs << "0,heads_default,wzcagcccccccccccccccccccccccccccccccccccczvfcaxgab" << std::endl;
        file.Close();
        
        // entryname.txt
        file_path = Apto::FileSystem::PathAppend(path, "entryname.txt");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "@ancestor" << std::endl;
        file.Close();
      }
      
      
      void WriteExamplePopulation(const Apto::String& path, const Apto::String& example_key)
      {
        Apto::String file_path = Apto::FileSystem::PathAppend(path, "detail.spop");
        cFile file;
        file.Open((const char*)file_path, std::ios::out);
        std::fstream* fs = file.GetFileStream();
        
        *fs << "#filetype genotype_data" << std::endl;
        *fs << "#format id src src_args parents num_units total_units length merit gest_time fitness gen_born update_born update_deactivated depth hw_type inst_set sequence cells gest_offset lineage" << std::endl;

        if (example_key == "@example") {
          *fs << "1 div:ext (none) (none) 1 1 100 0 0 0 0 -1 -1 0 0 heads_default wzcagcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccczvfcaxgab 1770 66 0" << std::endl;
        }
        
        file.Close();

        // Update events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "u begin LoadPopulation detail.spop" << std::endl;
        file.Close();
      }
      
      
      void CreateDefaultFreezer(const Apto::String& path)
      {
        Apto::String entry_path;
        
        // Default Config
        entry_path = Apto::FileSystem::PathAppend(path, "c0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path, "@default");
        
        // Default Genome
        entry_path = Apto::FileSystem::PathAppend(path, "g0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultGenome(entry_path);
                
        // Sample Worlds
        entry_path = Apto::FileSystem::PathAppend(path, "w0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path, "@example");
        WriteExamplePopulation(entry_path, "@example");        
      }
      
      
      bool SaveConfig(cWorld* world, const Apto::String& path)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        
        // avida.cfg
        file_path = Apto::FileSystem::PathAppend(path, "avida.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        *fs << "WORLD_X " << world->GetConfig().WORLD_X.Get() << std::endl;
        *fs << "WORLD_Y " << world->GetConfig().WORLD_Y.Get() << std::endl;
        *fs << "WORLD_GEOMETRY 1" << std::endl;
        *fs << "COPY_MUT_PROB " << world->GetConfig().COPY_MUT_PROB.Get() << std::endl;
        *fs << "DIVIDE_INS_PROB 0.0" << std::endl;
        *fs << "DIVIDE_DEL_PROB 0.0" << std::endl;
        *fs << "OFFSPRING_SIZE_RANGE 1.0" << std::endl;
        *fs << "BIRTH_METHOD " << world->GetConfig().BIRTH_METHOD.Get() << std::endl;
        *fs << "RANDOM_SEED " << world->GetConfig().RANDOM_SEED.Get() << std::endl;
        *fs << "#include instset.cfg" << std::endl;
        
        file.Close();
        
        
        // write instset.cfg
        WriteInstSet(path);
        
        
        // events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();        
        file.Close();
        
        
        // environment.cfg
        file_path = Apto::FileSystem::PathAppend(path, "environment.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        
        // @TODO 
        cEnvironment& env = world->GetEnvironment();
        *fs << "REACTION  NOT  not   process:value=" << env.GetReactionValue("NOT")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NAND nand  process:value=" << env.GetReactionValue("NAND") << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  AND  and   process:value=" << env.GetReactionValue("AND")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ORN  orn   process:value=" << env.GetReactionValue("ORN")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  OR   or    process:value=" << env.GetReactionValue("OR")   << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  ANDN andn  process:value=" << env.GetReactionValue("ANDN") << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  NOR  nor   process:value=" << env.GetReactionValue("NOR")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  XOR  xor   process:value=" << env.GetReactionValue("XOR")  << ":type=pow  requisite:max_count=1" << std::endl;
        *fs << "REACTION  EQU  equ   process:value=" << env.GetReactionValue("EQU")  << ":type=pow  requisite:max_count=1" << std::endl;
        
        file.Close();
        
        return true;
      }
      
      
      bool SavePopulation(cWorld* world, const Apto::String& path)
      {
        Apto::String file_path;
        cFile file;
        std::fstream* fs;
        
        // Save Population
        file_path = Apto::FileSystem::PathAppend(path, "detail.spop");
        if (!world->GetPopulation().SavePopulation((const char*)file_path, true)) return false;
        
        // Update events.cfg
        file_path = Apto::FileSystem::PathAppend(path, "events.cfg");
        file.Open((const char*)file_path, std::ios::out);
        fs = file.GetFileStream();
        *fs << "u begin LoadPopulation detail.spop" << std::endl;
        file.Close();
        
        return true;
      }
      
    }
  }
}

Avida::Viewer::Freezer::Freezer(const Apto::String& dir) : m_dir(dir), m_opened(false)
{
  // Initialize next identifiers
  m_next_id[0] = m_next_id[1] = m_next_id[2] = -1;
  
  
  // Check for existing freezer dir
  if (!Apto::FileSystem::IsDir(m_dir)) {
    if (!Apto::FileSystem::MkDir(m_dir)) return;
    
    // Set up defaults
    Private::CreateDefaultFreezer(m_dir);
  }
    
  // Open entries
  Apto::Array<Apto::String, Apto::Smart> direntries;
  if (!Apto::FileSystem::ReadDir(m_dir, direntries)) return;
  
  for (int i = 0; i < direntries.GetSize(); i++) {
    if (direntries[i].GetSize() < 2) continue;
    
    Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, direntries[i]);
    if (!Apto::FileSystem::IsDir(full_path)) continue;
    
    int identifier = Apto::StrAs(direntries[i].Substring(1));
    
    // Attempt to read name file
    Apto::String name = direntries[i];
    Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
    if (Apto::FileSystem::IsFile(name_path)) {
      // Open name file and read contents
      cFile file;
      file.Open((const char*)name_path, std::ios::in);
      cString line;
      file.ReadLine(line);
      file.Close();
      
      line.Trim();
      name = line;
    }
    
    FreezerObjectType t;
    switch (direntries[i][0]) {
      case 'c': t = CONFIG; break;
      case 'g': t = GENOME; break;
      case 'w': t = WORLD;  break;
      default: continue;
    }
    m_next_id[t] = (m_next_id[t] <= identifier) ? (identifier + 1) : m_next_id[t];
    m_entries[t].Push(Entry(name, direntries[i]));
  }
  
  m_opened = true;
}

Avida::Viewer::Freezer::~Freezer()
{
  if (!m_opened) return;
  
  // Search for inactive entries and remove them from the freezer
  for (int t = CONFIG; t <= WORLD; t++) {
    for (int i = 0; i < m_entries[t].GetSize(); i++) {
      if (!m_entries[t][i].active) {
        Apto::FileSystem::RmDir(Apto::FileSystem::PathAppend(m_dir, m_entries[t][i].path), true);
      }
    }
  }
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveConfig(cWorld* world, const Apto::String& name)
{
  if (!m_opened) return FreezerID(CONFIG, -1);

  Apto::String entry_path = Apto::FormatStr("c%d", m_next_id[CONFIG]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(CONFIG, -1);
  
  // Attempt to Save Config
  if (!Private::SaveConfig(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[CONFIG].Push(Entry(name, entry_path));
  return FreezerID(CONFIG, m_entries[CONFIG].GetSize() - 1);
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveGenome(GenomePtr genome, const Apto::String& name)
{
  if (!m_opened) return FreezerID(GENOME, -1);
  
  Apto::String entry_path = Apto::FormatStr("g%d", m_next_id[GENOME]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(GENOME, -1);
  
  // Attempt to Save Genome
  Apto::String genome_path = Apto::FileSystem::PathAppend(full_path, "genome.seq");
  cFile file;
  if (!file.Open((const char*)genome_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& gfs = *file.GetFileStream();
  gfs << genome->AsString() << std::endl;
  
  file.Close();
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[GENOME].Push(Entry(name, entry_path));
  return FreezerID(GENOME, m_entries[GENOME].GetSize() - 1);  
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveWorld(cWorld* world, const Apto::String& name)
{
  if (!m_opened) return FreezerID(WORLD, -1);

  Apto::String entry_path = Apto::FormatStr("w%d", m_next_id[WORLD]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(WORLD, -1);
  
  // Attempt to Save Config
  if (!Private::SaveConfig(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
  
  // Attempt to Save Population
  if (!Private::SavePopulation(world, full_path)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(CONFIG, -1);    
  }
    
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    Apto::FileSystem::RmDir(full_path, true);
    return FreezerID(WORLD, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[WORLD].Push(Entry(name, entry_path));
  
  FreezerID new_id(WORLD, m_entries[WORLD].GetSize() - 1);
  SaveAttachment(new_id, "update", Apto::AsStr(world->GetStats().GetUpdate()));
  return new_id;
}


bool Avida::Viewer::Freezer::InstantiateWorkingDir(FreezerID entry_id, const Apto::String& working_directory) const
{
  if (!m_opened) return false;

  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;

  // Copy contained files to the destination directory
  Apto::String src_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  Apto::FileSystem::CpDir(src_path, working_directory);
  
  return true;
}

Avida::GenomePtr Avida::Viewer::Freezer::InstantiateGenome(FreezerID entry_id) const
{
  if (!m_opened) return GenomePtr();
  if (entry_id.type != GENOME || entry_id.identifier >= m_entries[GENOME].GetSize()) return GenomePtr();
  
  Apto::String seq_str;
  if (Apto::FileSystem::IsFile(PathOf(entry_id))) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)PathOf(entry_id), std::ios::in);
    cString line;
    if (!file.Eof() && file.ReadLine(line)) seq_str = line;
    file.Close();
  }

  return GenomePtr(new Genome(seq_str));
}


Apto::String Avida::Viewer::Freezer::PathOf(FreezerID entry_id) const
{
  // Check if it is a genome, and if so return the path to the file itself
  if (entry_id.type == GENOME)
  {
    Apto::String filepath = Apto::FileSystem::PathAppend(m_entries[entry_id.type][entry_id.identifier].path, "genome.seq");
    return Apto::FileSystem::PathAppend(m_dir, filepath);
  }
  
  // For all others, return the path of the containing directory
  return Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
}



bool Avida::Viewer::Freezer::SaveAttachment(FreezerID entry_id, const Apto::String& name, const Apto::String& value)
{
  if (!IsValid(entry_id) || entry_id.type == GENOME) return false;
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(PathOf(entry_id), name);
  cFile file;
  if (!file.Open((const char*)name_path, std::ios::out)) {
    return false;
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << value;
  
  file.Close();
  
  return true;
}

Apto::String Avida::Viewer::Freezer::LoadAttachment(FreezerID entry_id, const Apto::String& name)
{
  if (!IsValid(entry_id) || entry_id.type == GENOME) return "";
  
  Apto::String rtn;
  Apto::String name_path = Apto::FileSystem::PathAppend(PathOf(entry_id), name);
  if (Apto::FileSystem::IsFile(name_path)) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)name_path, std::ios::in);
    cString line;
    if (!file.Eof() && file.ReadLine(line)) rtn = line;
    while (!file.Eof() && file.ReadLine(line)) {
      rtn += "\n";
      rtn += line;
    }
    file.Close();
  }
  return rtn;
}



bool Avida::Viewer::Freezer::Rename(FreezerID entry_id, const Apto::String& name)
{
  if (!m_opened) return false;
  
  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;
  
  // Save new entry name to entry path
  cFile file;
  Apto::String name_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  name_path = Apto::FileSystem::PathAppend(name_path, "entryname.txt");
  if (!file.Open((const char*)name_path, std::ios::out)) {
    return false;
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Rename in memory entry
  m_entries[entry_id.type][entry_id.identifier].name = name;
  
  return true;
}

Apto::String Avida::Viewer::Freezer::NewUniqueNameForType(FreezerObjectType type, const Apto::String& name)
{
  int suffix = 1;
  Apto::String suggest = name;
  for (int i = 0; i < m_entries[type].GetSize(); i++) {
    if (m_entries[type][i].name == suggest) {
      suggest = name;
      suggest += " ";
      suggest += Apto::AsStr(suffix++);
      
      // reset index to re-check with new suggestion
      i = 0;
    }
  }
  
  return suggest;
}


void Avida::Viewer::Freezer::DuplicateFreezerAt(Apto::String destination)
{
  // Copy the workspace
  Apto::FileSystem::CpDir(m_dir, destination);
  
  // Delete any inactive items from new duplicate
  // Search for inactive entries and remove them from the freezer
  for (int t = CONFIG; t <= WORLD; t++) {
    for (int i = 0; i < m_entries[t].GetSize(); i++) {
      if (!m_entries[t][i].active) {
        Apto::FileSystem::RmDir(Apto::FileSystem::PathAppend(destination, m_entries[t][i].path), true);
      }
    }
  }
}

void Avida::Viewer::Freezer::ExportItem(FreezerID entry_id, Apto::String destination)
{
  Apto::FileSystem::CpDir(Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path), destination);
  
  // Write out entry type file
  cFile file;
  destination = Apto::FileSystem::PathAppend(destination, "entrytype.txt");
  if (!file.Open((const char*)destination, std::ios::out)) return;
  
  std::fstream& nfs = *file.GetFileStream();
  switch (entry_id.type) {
    case CONFIG: nfs << "c" << std::endl; break;
    case GENOME: nfs << "g" << std::endl; break;
    case WORLD:  nfs << "w" << std::endl; break;
  }
  
  file.Close();
}

Avida::Viewer::FreezerID Avida::Viewer::Freezer::ImportItem(Apto::String src)
{
  // Attempt to read name file
  Apto::String type_path = Apto::FileSystem::PathAppend(src, "entrytype.txt");
  if (!Apto::FileSystem::IsFile(type_path)) return FreezerID(WORLD, -1);
  
  // Open name file and read contents
  cFile file;
  file.Open((const char*)type_path, std::ios::in);
  cString line;
  file.ReadLine(line);
  file.Close();
  
  if (line.GetSize() == 0) return FreezerID(WORLD, -1);

  Apto::String entry_path;
  switch (line[0]) {
    case 'c': entry_path = Apto::FormatStr("c%d", m_next_id[CONFIG]++); break;
    case 'g': entry_path = Apto::FormatStr("g%d", m_next_id[GENOME]++); break;
    case 'w': entry_path = Apto::FormatStr("w%d", m_next_id[WORLD]++); break;
  }
  
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  Apto::FileSystem::CpDir(src, full_path);
  
  Apto::String name = entry_path;
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entryname.txt");
  if (Apto::FileSystem::IsFile(name_path)) {
    // Open name file and read contents
    cFile file;
    file.Open((const char*)name_path, std::ios::in);
    cString line;
    file.ReadLine(line);
    file.Close();
    
    line.Trim();
    name = line;
  }
  
  
  switch (line[0]) {
    case 'c': m_entries[CONFIG].Push(Entry(name, entry_path)); return FreezerID(CONFIG, m_entries[CONFIG].GetSize() - 1);
    case 'g': m_entries[GENOME].Push(Entry(name, entry_path)); return FreezerID(GENOME, m_entries[GENOME].GetSize() - 1);
    case 'w': m_entries[WORLD].Push(Entry(name, entry_path)); return FreezerID(WORLD, m_entries[WORLD].GetSize() - 1);
  }
  
  return FreezerID(WORLD, -1);
}
