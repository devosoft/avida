/*
 *  cDataFileManager.h
 *  Avida
 *
 *  Created by David on 10/18/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2005 California Institute of Technology
 *
 */

#ifndef cDataFileManager_h
#define cDataFileManager_h

#include <fstream>

#ifndef cTools_h
#include "cTools.h"
#endif
#ifndef cDataFile_h
#include "cDataFile.h"
#endif
#ifndef tDictionary_h
#include "tDictionary.h"
#endif

/**
 * This class helps to manage a collection of data files. It is possible
 * to add files, to remove files, and to access existing files by name.
 **/

class cDataFile;
class cString;
template <class T> class tList; // aggregate

class cDataFileManager {
private:
  cString m_target_dir;
  tDictionary<cDataFile*> m_datafiles;

  cDataFileManager(const cDataFileManager&); // @not_implemented
  cDataFileManager& operator=(const cDataFileManager&); // @not_implemented
  
public:
  inline cDataFileManager(cString target_dir = "");
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
};


#ifdef ENABLE_UNIT_TESTS
namespace nDataFileManager {
  /**
   * Run unit tests
   *
   * @param full Run full test suite; if false, just the fast tests.
   **/
  void UnitTests(bool full = false);
}
#endif  

inline cDataFileManager::cDataFileManager(cString target_dir) : m_target_dir(target_dir)
{
  if (m_target_dir.GetSize() > 0) {
    char dir_tail = m_target_dir[m_target_dir.GetSize() - 1];
    if (dir_tail != '\\' && dir_tail != '/') m_target_dir += "/";
    cTools::MkDir(m_target_dir, true);
  }
}

inline bool cDataFileManager::IsOpen(const cString & name)
{
  cDataFile* found;
  if (m_datafiles.Find(name, found)) return false;
  return true;
}


#endif
