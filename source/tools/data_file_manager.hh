//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

/* data_file_manager.hh ********************************************************
 cDataFileManager

 charles@krl.caltech.edu & travc@ugcs.caltech.edu
 Time-stamp: <1999-01-07 10:02:09 travc>

 cString: basic string class
******************************************************************************/

#ifndef DATA_FILE_MANAGER_HH
#define DATA_FILE_MANAGER_HH

#include <fstream>

#ifndef TLIST_HH
#include "tList.hh"
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
  tList<cDataFile> data_file_list;

  cDataFile * InternalFind(const cString & name);

public:
  cDataFileManager() { ; }
  ~cDataFileManager();

  /**
   * Looks up the @ref cDataFile corresponding to the given name. If that
   * file hasn't been created previously, it is created now.
   *
   * @return The @ref cDataFile.
   * @param name The name of the file to look up/create.
   **/
  cDataFile & Get(const cString & name);

  /**
   * Looks up the ofstream corresponding to the file of the given name.
   * If that file hasn't been created previously, it is created now.
   *
   * Read the cautionary remarks about the function with the same name in
   * @ref cDataFile.
   *
   * @return The ofstream.
   * @param name The name of the file to look up/create.
   **/
  std::ofstream & GetOFStream(const cString & name);

  bool IsOpen(const cString & name);

  void FlushAll();

  /** Removes the given file, thereby closing it.
   *
   * @return true if file existed, otherwise false.
   **/
  bool Remove(const cString & name);
};

#endif
