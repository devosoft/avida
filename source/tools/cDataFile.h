/*
 *  cDataFile.h
 *  Avida
 *
 *  Created by David on 12/2/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 */

#ifndef cDataFile_h
#define cDataFile_h

#include <fstream>

#ifndef cString_h
#include "cString.h"
#endif

/**
 * This class encapsulates file handling for the class @ref cDataFileManager.
 * The files it creates are write only. Reading of files is handled by
 * @ref cFile.
 */

class cDataFile {

private:
  cDataFile(const cDataFile &); // not implemented.
private:
  cString m_name;
  cString m_data;
  cString m_descr;
  int num_cols;
  
  bool m_descr_written;
  
  std::ofstream m_fp;

  void Init();
public:
  /**
   * The empty constructor does nothing.
   **/
  cDataFile();

  /**
   * This constructor opens a file of the given name, and makes sure
   * the file is usable.
   *
   * @param _name The name of the file to open.
   **/
  cDataFile(cString name);

  /**
   * The desctructor automatically closes the file.
   **/
  ~cDataFile(){ m_fp.close(); }

  /**
   * @return The file name used
   **/
  const cString & GetName() const { return m_name; }

  /**
   * @return A bool that indicates whether the file is actually usable.
   **/
  bool Good() const { return m_fp.good(); }

  /**
   * This function allows low-level write access to the file. Although
   * sometimes usefull, it provides the possibility to circumvent the
   * automatic documentation. Use with care.
   *
   * @return The output stream corresponding to the file.
   **/
  std::ofstream& GetOFStream() { return m_fp; }

  /**
   * Outputs a value into the data file.
   *
   * @param x The value to write (as double, int, or char *)
   *
   * @param descr A string that describes the meaning of the value. The string
   * will be written only once, before the first data line has been finished.
   **/

  void Write( double x,              const char * descr );
  void Write( int i,                 const char * descr );
  void Write( const char * data_str, const char * descr );
  void WriteBlockElement (double x, int element, int x_size );
  void WriteBlockElement (int i, int element, int x_size );

  /**
   * Writes a descriptive string into a data file. The string is only
   * written if the first data line hasn't been completed (Endl() hasn't
   * been called. This allows to output initial comments into a file.
   **/
  void WriteComment( const char * descr );

  /**
   * Same as WriteComment, but doesn't automatically include the # in the
   * front of the line.  This should only be used in special circumstances
   * where something outside of a typical comment needs to be at the top.
   **/
  void WriteRawComment( const char * descr );

  /**
   * Writes text string any where in the data file. This should only be used 
   * in special circumstances where something outside of a typical comment 
   * needs to be placed in the file.
   **/
  void WriteRaw( const char * descr );

  /**
   * Writes the description for a single column; keeps track of column numbers.
   **/
  void WriteColumnDesc(const char * descr );

  /**
   * Writes the current time into the data file. The time string is only
   * written if the first data line hasn't been completed (Endl() hasn't
   * been called.
   **/
  void WriteTimeStamp();

  /**
   * This function writes the comments that have accumulated. There should
   * normally be no reason to call this function. Endl() does the same thing
   * in a safer way.
   **/
  void FlushComments();
  
  /**
   * Write all data to disk and start a new line.
   **/
  void Endl();

  /**
   * Has the header been written to the file yet?
   **/
  bool HeaderDone() { return m_descr_written; }
  
  /**
   * This function makes sure that all cached data is written to the disk.
   **/
  void Flush(){ m_fp.flush(); }
};

#endif
