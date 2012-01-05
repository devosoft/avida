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
        
        file.Close();
      }
      
      void WriteDefaultConfig(const Apto::String& path)
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
        *fs << "COPY_MUT_PROB 0.0075" << std::endl;
        *fs << "BIRTH_METHOD 4" << std::endl;
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
        *fs << "@default" << std::endl;
        file.Close();
      }
      
      
      void CreateDefaultFreezer(const Apto::String& path)
      {
        Apto::String entry_path;
        
        // Default Config
        entry_path = Apto::FileSystem::PathAppend(path, "c0");
        Apto::FileSystem::MkDir(entry_path);
        WriteDefaultConfig(entry_path);
        
        // Write Default Org
        // @TODO
        
        // Write sample population
        // @TODO
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
        *fs << "COPY_MUT_PROB " << world->GetConfig().COPY_MUT_PROB.Get() << std::endl;
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
        file.Open((const char*)file_path, std::ios::app);
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
  for (FreezerObjectType t = CONFIG; t <= WORLD; t++) {
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


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveGenome(GenomePtr genome)
{
  if (!m_opened) return FreezerID(GENOME, -1);
  
  Apto::String entry_path = Apto::FormatStr("g%d", m_next_id[GENOME]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  Apto::String name = genome->Properties().Get("Name");
  
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

  Apto::String entry_path = Apto::FormatStr("c%d", m_next_id[WORLD]++);
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
  return FreezerID(WORLD, m_entries[WORLD].GetSize() - 1);
}


bool Avida::Viewer::Freezer::Instantiate(FreezerID entry_id, const Apto::String& working_directory) const
{
  if (!m_opened) return false;

  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;

  // Copy contained files to the destination directory
  Apto::String src_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  Apto::FileSystem::CpDir(src_path, working_directory);
  
  return true;
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

