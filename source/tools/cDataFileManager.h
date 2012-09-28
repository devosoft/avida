/*
 *  cDataFileManager.h
 *  Avida
 *
 *  Called "data_file_manager.hh" prior to 10/18/05.
 *  Copyright 1999-2011 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology
 *
 */

#ifndef cDataFileManager_h
#define cDataFileManager_h

#include <fstream>

#include "cDataFile.h"


/**
 * This class helps to manage a collection of data files. It is possible
 * to add files, to remove files, and to access existing files by name.
 **/

class cString;
template <class T> class tList; // aggregate

class cDataFileManager
{
private:
  cString m_target_dir;
  Apto::Map<Apto::String, cDataFile*> m_datafiles;

  cDataFileManager(const cDataFileManager&); // @not_implemented
  cDataFileManager& operator=(const cDataFileManager&); // @not_implemented
  
public:
  cDataFileManager(const cString& target_dir = "", bool verbose = false);
  ~cDataFileManager();

  /**
   * Looks up the @ref cDataFile corresponding to the given name. If that
   * file hasn't been created previously, it is created now.
   *
   * @return The @ref cDataFile.
   * @param name The name of the file to look up/create.
   **/
  cDataFile& Get(const cString & name);
  std::ofstream& GetOFStream(const cString& name) { return Get(name).GetOFStream(); }

  inline bool IsOpen(const cString& name);

  void FlushAll();

  /** Removes the given file, thereby closing it.
   *
   * @return true if file existed, otherwise false.
   **/
  bool Remove(const cString& name);
  
  const cString& GetTargetDir() const { return m_target_dir; }
};


inline bool cDataFileManager::IsOpen(const cString & name)
{
  cDataFile* found;
  if (m_datafiles.Get((const char*)name, found)) return false;
  return true;
}


#endif
