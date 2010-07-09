/*
 *  cDataManager_Base.cc
 *  Avida
 *
 *  Called "data_manager_base.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
 *  Copyright 1993-2001 California Institute of Technology
 *
 */

#include "cDataManager_Base.h"

#include "cDataFile.h"

using namespace std;


bool cDataManager_Base::PrintRow(cDataFile& data_file, cString row_entries, char sep)
{
  bool result = true;
  
  row_entries.CompressWhitespace();
  
  // If we haven't output the header for this file yet, do so.
  if ( data_file.HeaderDone() == false ) {
    // Setup the format string for the top...
    cString format(row_entries);
    cString out_filetype(filetype);
    
    // Make sure its space seperated...
    if (sep != ' ') {
      int pos = -1;
      while ( (pos = format.Find(sep)) != -1 ) format[pos] = ' ';
    }
    
    // Write out the filetype and format strip
    out_filetype.Insert("#filetype ");
    format.Insert("#format ");
    data_file.WriteRawComment(out_filetype);
    data_file.WriteRawComment(format);
    
    // Setup the human-readable description...
    cString header_entries(row_entries);
    cString cur_desc;
    data_file.WriteComment(" ");
    data_file.WriteComment("Legend:");
    while (header_entries.GetSize() > 0) {
      cString cur_entry( header_entries.Pop(sep) );
      if ( GetDesc(cur_entry, cur_desc) == false ) {
        result = false;
        continue;
      }
      data_file.WriteColumnDesc(cur_desc);
    }
    data_file.Endl();
  }
  
  
  ofstream& fp = data_file.GetOFStream();
  while (row_entries.GetSize() > 0) {
    cString cur_entry( row_entries.Pop(sep) );
    if ( Print(cur_entry, fp) == false ) {
      cerr << "Data manager unable to find entry '"
      << cur_entry << "'" << endl;
      result = false;
      continue;
    }
    fp << " ";
  }
  fp << endl;
  
  return result;
}

