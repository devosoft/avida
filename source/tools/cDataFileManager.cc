/*
 *  cDataFileManager.cc
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology
 *
 */

#include "cDataFileManager.h"

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
    found_file = new cDataFile(m_target_dir + name);
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
