/*
 *  cDataFileManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology
 *
 */

#include "cDataFileManager.h"

#include "cTools.h"

using namespace std;


cDataFileManager::~cDataFileManager()
{
  tList<cString> names;
  tList<cDataFile*> files;
  m_datafiles.AsLists(names, files);
  tListIterator<cDataFile*> list_it(files);
  while (list_it.Next() != NULL) delete *list_it.Get();
}

cDataFile & cDataFileManager::Get(const cString & name)
{
  // Find the file by this name...
  cDataFile* found_file;

  // If it hasn't been found, create it...
  if (!m_datafiles.Find(name, found_file)) {
    // Create directory structure, as necessary
    int i = 0;
    while (i < name.GetSize()) {
      int d = name.Find('/', i);
      int b = name.Find('\\', i);
      
      // If a backslash is found, and earlier then next forward slash, use it instead
      if (b != -1 && b < d) d = b;
      
      // Exit if neither slash is found
      if (d == -1) break;
      
      // If directory name is not null
      if (d - i > 0) cTools::MkDir(m_target_dir + name.Substring(0, d - i), false);
      
      // Adjust next directory name starting point
      i = d + 1;
    }
    cString filename = m_target_dir + name;
    found_file = new cDataFile(filename);
    m_datafiles.Add(name, found_file);
  }

  // and return it.
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
  cDataFile* found_file = m_datafiles.Remove(name);
  if (found_file == NULL) return false;

  delete found_file;
  return true;
}
