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

#include "cFile.h"


Avida::Viewer::Freezer::Freezer(const Apto::String& dir) : m_dir(dir)
{
  // Initialize next identifiers
  m_next_id[0] = m_next_id[1] = m_next_id[2] = 0;
  
  
  // Check for existing freezer dir
  // @TODO 
  
    // Create freezer if it doesn't exist
    // @TODO 
  
  // Open entries
  // @TODO 
}

Avida::Viewer::Freezer::~Freezer()
{
  // Search for inactive entries and remove them from the freezer
  for (FreezerObjectType t = CONFIG; t <= WORLD; t++) {
    for (int i = 0; i < m_entries[t].GetSize(); i++) {
      if (!m_entries[t][i].active) {
        // @TODO remove inactive entry
      }
    }
  }
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveConfig(World* world, const Apto::String& name)
{
  Apto::String entry_path = Apto::FormatStr("c%d", m_next_id[CONFIG]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(CONFIG, -1);
  
  // Attempt to Save World
  // @TODO 
  
  // If failed, remove directory return invalid id
  // @TODO 
  
  // On success, save name file
  // @TODO 
  
  // Create entry and return
  m_entries[CONFIG].Push(Entry(name, entry_path));
  return FreezerID(CONFIG, m_entries[CONFIG].GetSize() - 1);
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveGenome(GenomePtr genome)
{
  Apto::String entry_path = Apto::FormatStr("g%d", m_next_id[GENOME]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  Apto::String name = genome->Properties().Get("Name");
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(GENOME, -1);
  
  // Attempt to Save Genome
  Apto::String genome_path = Apto::FileSystem::PathAppend(full_path, "genome.seq");
  cFile file;
  if (!file.Open((const char*)genome_path, std::ios::out)) {
    // @TODO - remove directory due to failed save
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& gfs = *file.GetFileStream();
  gfs << genome->AsString() << std::endl;
  
  file.Close();
  
  // On success, save name file
  Apto::String name_path = Apto::FileSystem::PathAppend(full_path, "entry_name.txt");
  if (!file.Open((const char*)name_path, std::ios::out)) {
    // @TODO - remove directory due to failed save
    return FreezerID(GENOME, -1);
  }
  
  std::fstream& nfs = *file.GetFileStream();
  nfs << name << std::endl;
  
  file.Close();
  
  // Create entry and return
  m_entries[GENOME].Push(Entry(name, entry_path));
  return FreezerID(GENOME, m_entries[GENOME].GetSize() - 1);  
}


Avida::Viewer::FreezerID Avida::Viewer::Freezer::SaveWorld(World* world, const Apto::String& name)
{
  Apto::String entry_path = Apto::FormatStr("c%d", m_next_id[WORLD]++);
  Apto::String full_path = Apto::FileSystem::PathAppend(m_dir, entry_path);
  
  if (!Apto::FileSystem::MkDir(full_path)) return FreezerID(WORLD, -1);
  
  // Attempt to Save World
  // @TODO 
  
  // If failed, remove directory return invalid id
  // @TODO 
  
  // On success, save name file
  // @TODO 
  
  // Create entry and return
  m_entries[WORLD].Push(Entry(name, entry_path));
  return FreezerID(WORLD, m_entries[WORLD].GetSize() - 1);
}


bool Avida::Viewer::Freezer::Instantiate(FreezerID entry_id, const Apto::String& working_directory) const
{
  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;

  // Copy contained files to the destination directory
  // @TODO
  
  return false;
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
  if (entry_id.identifier >= m_entries[entry_id.type].GetSize()) return false;
  
  // Save new entry name to entry path
  cFile file;
  Apto::String name_path = Apto::FileSystem::PathAppend(m_dir, m_entries[entry_id.type][entry_id.identifier].path);
  name_path = Apto::FileSystem::PathAppend(name_path, "entry_name.txt");
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

