//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef DATA_FILE_MANAGER_HH
#include "cDataFileManager.h"
#endif

#ifndef DATA_FILE_HH
#include "cDataFile.h"
#endif

using namespace std;

//////////////////////
//  cDataFileManager
//////////////////////


cDataFileManager::~cDataFileManager()
{
  while (data_file_list.GetSize()) {
    delete data_file_list.Pop();
  }
}


cDataFile * cDataFileManager::InternalFind(const cString & name)
{
  tListIterator<cDataFile> list_it(data_file_list);
  while (list_it.Next() != NULL) {
    if (list_it.Get()->GetName() == name) return list_it.Get();
  }
  return NULL;
}


cDataFile & cDataFileManager::Get(const cString & name)
{
  // Find the file by this name...
  cDataFile * found_file = InternalFind(name);

  // If it hasn't been found, create it...
  if (found_file == NULL) {
    found_file = new cDataFile(name);
    data_file_list.Push(found_file);
  }

  // Make sure we got the name right...
  assert( found_file->GetName() == name );

  // and return it.
  return *found_file;
}


ofstream & cDataFileManager::GetOFStream(const cString & name)
{
  // Find the file by this name...
  cDataFile * found_file = InternalFind(name);

  // If it hasn't been found, create it...
  if (found_file == NULL) {
    found_file = new cDataFile(name);
    data_file_list.Push(found_file);
  }

  // Make sure we got the name right...
  assert( found_file->GetName() == name );

  // And return the releven stream...
  return found_file->GetOFStream();
}

bool cDataFileManager::IsOpen(const cString & name)
{
  if (InternalFind(name) == NULL) return false;
  return true;
}


void cDataFileManager::FlushAll()
{
  tListIterator<cDataFile> list_it(data_file_list);
  while (list_it.Next() != NULL) list_it.Get()->Flush();
}


bool cDataFileManager::Remove(const cString & name)
{
  cDataFile * found_file = InternalFind(name);
  if (found_file == NULL) return false;

  delete found_file;
  return true;
}
