/*
 *  cDataFileManager.cc
 *  Avida
 *
 *  Called "data_file_manager.cc" prior to 10/18/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology
 *
 */

#include "cDataFileManager.h"

#include "apto/core/FileSystem.h"

using namespace std;


cDataFileManager::cDataFileManager(const cString& target_dir, bool) : m_target_dir(target_dir)
{
  m_target_dir.Trim();
  
  m_target_dir = Apto::FileSystem::GetAbsolutePath(Apto::String(m_target_dir), Apto::FileSystem::GetCWD());
  
  if (m_target_dir.GetSize() > 0) {
    char dir_tail = m_target_dir[m_target_dir.GetSize() - 1];
    if (dir_tail != '\\' && dir_tail != '/') m_target_dir += "/";
    Apto::FileSystem::MkDir(Apto::String(m_target_dir));
  }
}

cDataFileManager::~cDataFileManager()
{
  tList<cString> names;
  tList<cDataFile*> files;
  m_datafiles.AsLists(names, files);
  tListIterator<cDataFile*> list_it(files);
  while (list_it.Next() != NULL) delete *list_it.Get();
}

cDataFile& cDataFileManager::Get(const cString& name)
{
  assert(name.GetSize());

  cDataFile* found_file;
  
  // If found, return file
  if (m_datafiles.Find(name, found_file)) return *found_file;
  
  
  // Create and sanitize a local copy of the file name
  cString target(name);
  target.Trim();
  
  // Determine directory prefix, default being the current data directory
  cString dir_prefix(m_target_dir);
  if ((target[0] == '.' &&             // Must start with '.' to trigger further testing
       (target.GetSize() == 1 ||       // If string is exactly "."
        (target.GetSize() > 1 &&       //   or if it ".." or "./" or ".\"
         (target[1] == '.' || target[1] == '/' || target[1] == '\\')
        )
       )
      ) || target[0] == '/'           // If it is an absolute path, ignore prefix as well
     )
  {
    // Treat path as current working directory relative
    dir_prefix = "";
  }
  
  // Create directory structure, as necessary
  int i = 0;
  while (i < target.GetSize()) {
    int d = target.Find('/', i);
    int b = target.Find('\\', i);
    
    // If a backslash is found, and earlier then next forward slash, use it instead
    if (b != -1 && b < d) d = b;
    
    // Exit if neither slash is found
    if (d == -1) break;
    
    // If directory name is not null
    if (d - i > 0) {
      cString dir = target.Substring(i, d - i);
      // Create if  that this directory is not a relative path component
      if (dir.GetSize() > 2 || (dir != "." && dir != ".."))
        Apto::FileSystem::MkDir(Apto::String(dir_prefix + target.Substring(0, d)));
    }
    
    // Adjust next directory name starting point
    i = d + 1;
  }

  target = dir_prefix + target;
  found_file = new cDataFile(target);
  m_datafiles.Set(name, found_file);

  return *found_file;
}

void cDataFileManager::FlushAll()
{
  tList<cString> names;
  tList<cDataFile*> files;
  m_datafiles.AsLists(names, files);
  tListIterator<cDataFile*> list_it(files);
  while (list_it.Next() != NULL) (*list_it.Get())->Flush();
}

bool cDataFileManager::Remove(const cString& name)
{
  cDataFile* found_file = NULL;
  m_datafiles.Remove(name, found_file);
  if (found_file == NULL) return false;

  delete found_file;
  return true;
}
