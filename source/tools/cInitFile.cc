/*
 *  cInitFile.cc
 *  Avida
 *
 *  Created by David on 12/7/05.
 *  Copyright 2005-2006 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology
 *
 */

#include "cInitFile.h"

#include "cStringIterator.h"

using namespace std;


void cInitFile::Load()
{
  if (!IsOpen()) return;   // The file must be opened!
  cStringList line_list;   // Create a list to load all of the lines into.

  cString buf;
  ReadLine(buf);

  // If this file doesn't work properly, return!
  if ( Eof() && !buf.GetSize() ) return;

  line_list.PushRear(buf);

  ReadLine(buf);
  while( !Eof() || buf.GetSize() ){
    line_list.PushRear(buf);
    ReadLine(buf);
  }

  // Copy all of the lines into the line array.
  const int file_size = line_list.GetSize();
  line_array.Resize(file_size);

  for (int i = 0; i < file_size; i++) {
    line_array[i].line = line_list.Pop();
    line_array[i].line_num = i;
    line_array[i].used = false;
  }
}

void cInitFile::LoadStream(istream & in_stream)
{
  if (in_stream.good() == false) {
    cerr << "Bad stream sent to cInitFile::LoadStream()" << endl;
    return;
  }

  cStringList line_list;   // Create a list to load all of the lines into.

  char cur_line[MAX_STRING_LENGTH];
  in_stream.getline(cur_line, MAX_STRING_LENGTH);

  // If this file doesn't work properly, return.
  if( !in_stream && !strlen(cur_line) )  return;

  in_stream.getline(cur_line, MAX_STRING_LENGTH);
  while ( in_stream ) {
    line_list.PushRear(cur_line);
    in_stream.getline(cur_line, MAX_STRING_LENGTH);
  }

  // Copy all of the lines into the line array.
  const int file_size = line_list.GetSize();
  line_array.Resize(file_size);

  for (int i = 0; i < file_size; i++) {
    line_array[i].line = line_list.Pop();
    line_array[i].line_num = i;
    line_array[i].used = false;
  }
}


void cInitFile::Save(const cString & in_filename)
{
  cString save_filename(GetFilename());
  if (in_filename != "") save_filename = in_filename;
  
  ofstream fp_save(save_filename);

  // Go through the lines saving them...
  for (int i = 0; i < line_array.GetSize(); i++) {
    fp_save << line_array[i].line << endl;
  }

  fp_save.close();
}


void cInitFile::ReadHeader()
{
  cString type_line = GetLine(0);
  cString format_line = GetLine(1);

  if (type_line.PopWord() == "#filetype") filetype = type_line.PopWord();
  if (format_line.PopWord() == "#format") file_format.Load(format_line);
}


void cInitFile::Compress()
{
  // We're going to handle this compression in multiple passes to make it
  // clean and easy.

  const int num_lines = line_array.GetSize();

  // PASS 1: Remove all comments -- everything after a '#' sign -- and
  // compress all whitespace into a single space.
  for (int i = 0; i < num_lines; i++) {
    cString & cur_line = line_array[i].line;

    // Remove all characters past a comment mark and reduce whitespace.
    int comment_pos = cur_line.Find('#');
    if (comment_pos >= 0) cur_line.Clip(comment_pos);
    cur_line.CompressWhitespace();
  }

  // PASS 2: Merge each line ending with a continue marker '\' with the
  // next line.

  int prev_line_id = -1;
  bool continued = false;
  for (int i = 0; i < num_lines; i++) {
    // If the current line is a continuation, append it to the previous line.
    if (continued == true) {
      line_array[prev_line_id].line += line_array[i].line;
      line_array[i].line = "";
    }
    else prev_line_id = i;

    // See if the prev_line is continued, and if it is, take care of it.
    cString & prev_line = line_array[prev_line_id].line;
    if (prev_line.GetSize() > 0 &&
	prev_line[prev_line.GetSize() - 1] == '\\') {
      prev_line.ClipEnd(1);  // Remove continuation mark.
      continued = true;
    }
    else continued = false;
  }

  // PASS 3: Remove now-empty lines.

  int next_id = 0;
  for (int i = 0; i < num_lines; i++) {
    // If we should keep this line, compact it.
    if (line_array[i].line.GetSize() > 0) {
      if (next_id != i) line_array[next_id] = line_array[i];
      next_id++;
    }
  }

  // Clip any extra lines at the end of the array.

  line_array.Resize(next_id);

  // Move the active line back to the beginning to avoid confusion.
  active_line = 0;
}


void cInitFile::AddLine(cString & in_string)
{
  extra_lines.Push(in_string);
}

cString cInitFile::GetLine(int line_num)
{
  if (line_num < 0 || line_num >= line_array.GetSize()) return "";
  return line_array[line_num].line;
}


bool cInitFile::Find(cString & in_string, const cString & keyword,
		     int col) const
{
  bool found = false;

  // Loop through all of the lines looking for this keyword.  Start with
  // the actual file...
  for (int line_id = 0; line_id < line_array.GetSize(); line_id++) {
    cString cur_string = line_array[line_id].line;

    // If we found the keyword, return it and stop.    
    if (cur_string.GetWord(col) == keyword) {
      line_array[line_id].used = true;
      in_string = cur_string;
      found = true;
    }
  }

  // Next, look through any extra lines appended to the file.
  cStringIterator list_it(extra_lines);
  while ( list_it.AtEnd() == false ) {
    list_it.Next();
    cString cur_string = list_it.Get();

    // If we found the keyword, return it and stop.
    if (cur_string.GetWord(col) == keyword) {
      in_string = cur_string;
      found = true;
    }
  }

  return found;    // Not Found...
}


cString cInitFile::ReadString(const cString & name, cString def) const
{
  // See if we definately can't find the keyword.
  if (name == "") return def;

  // Search for the keyword.
  cString cur_line;
  if (Find(cur_line, name, 0) == false) {
    if (verbose == true) {
      cerr << "Warning: " << name << " not in \"" << GetFilename()
	   << "\", defaulting to: " << def <<endl;
    }
    return def;
  }

  // Pop off the keyword, and return the remainder of the line.
  cur_line.PopWord();
  return cur_line;
}


bool cInitFile::WarnUnused() const
{
  bool found = false;

  for (int i = 0; i < line_array.GetSize(); i++) {
    if (line_array[i].used == false) {
      if (found == false) {
        found = true;
        cerr << "Warning: unknown lines in input file '" << filename << "':" << endl;
      }
      cerr << " " << line_array[i].line_num + 1 << ": " << line_array[i].line << endl;
    }
  }
  
  if (found == true) cerr << endl;

  return found;
}


#ifdef ENABLE_UNIT_TESTS

/*
Unit tests
*/
#include "cXMLArchive.h"

#include <boost/detail/lightweight_test.hpp>

#include <cstdio>    // for std::remove() to remove temporary files.
#include <iomanip>
#include <iostream>
#include <fstream> 
#include <string>

namespace nInitFileTests {
  /*
  Test-helpers.
  */
  template <class T>
  void save_stuff(const T &s, const char * filename){
    std::ofstream ofs(filename);
    cXMLOArchive oa(ofs);
    oa.ArkvObj("cInitFile_Archive", s);
  }
  
  template <class T>
  void restore_stuff(T &s, const char * filename) {
    std::ifstream ifs(filename);
    cXMLIArchive ia(ifs);
    ia.ArkvObj("cInitFile_Archive", s);
  }
  

  namespace utInitFile_hello_world {
    void test(){
      BOOST_TEST(true);
      BOOST_TEST(false);
    }
  }

  namespace utInitFile_archiving {
    void test(){
      int linecount = 3;
      std::string data_file_name("./cInitFile_data.txt");
      { 
        std::ofstream data_file(data_file_name.c_str());
        for(int i = 0; i < linecount; i++){
          data_file << 2 * i << std::endl;
        }
      }

      std::string filename("./cInitFile_basic_serialization.xml");

      // Open cInitFile_data.txt for reading.
      cInitFile f(data_file_name.c_str());

      cInitFile f1, f2, f3;
      cString s1, s2, s3;
      cString l1, l2, l3;

      // Save initial cInitFile state.
      save_stuff<>(f, filename.c_str());
      // Reload initial state into new cInitFile.
      restore_stuff<>(f1, filename.c_str());
  
      // Save cInitFile state after reading first line.
      f.ReadLine(s1);
      save_stuff<>(f, filename.c_str());
      // Reload second state into new cInitFile.
      restore_stuff<>(f2, filename.c_str());

      // Save cInitFile state after reading second line.
      f.ReadLine(s2);
      save_stuff<>(f, filename.c_str());
      // Reload third state into new cInitFile.
      restore_stuff<>(f3, filename.c_str());

      f.ReadLine(s3);

      // Sanity checks...
      //BOOST_TEST(false);
      BOOST_TEST(cString("0") == s1);
      BOOST_TEST(cString("2") == s2);
      BOOST_TEST(cString("4") == s3);

      // Verify reading expected lines from various reloaded states.
      f3.ReadLine(l3);
      f2.ReadLine(l2);
      f1.ReadLine(l1);
      BOOST_TEST(l1 == s1);
      BOOST_TEST(l2 == s2);
      BOOST_TEST(l3 == s3);

      std::remove(filename.c_str());
      std::remove(data_file_name.c_str());
    }
  } // utInitFile_archiving

  namespace utInitFile_archiving_closed_file {
    void test(){
      std::string data_file_name("./cInitFile_data.txt");
      {
        std::ofstream data_file(data_file_name.c_str());
        for(int i = 0; i < 3; i++){
          data_file << 2 * i << std::endl;
        }
      }

      std::string filename("./cInitFile_serialize_closed_file.xml");

      // Open cInitFile_data.txt for reading.
      cInitFile f(data_file_name.c_str());
      f.Load();
      f.Compress();
      f.Close();
      BOOST_TEST(!f.IsOpen());

      cInitFile f1;

      // Save cInitFile state.
      save_stuff<>(f, filename.c_str());
      // Reload state into new cInitFile.
      restore_stuff<>(f1, filename.c_str());
      // Verify new cInitFile has matching filename.
      BOOST_TEST(f.GetFilename() == f1.GetFilename());
      // Verify new cInitFile is closed.
      BOOST_TEST(!f1.IsOpen());

      std::remove(filename.c_str());
      std::remove(data_file_name.c_str());
    }
  } // utInitFile_archiving_closed_file

  /*
  Bugfix: cInitFile was returning default value instead of value read
  from a closed init file, even though file had been open and read.
  @kgn
  */
  namespace utInitFile_ReadString_after_open_and_close {
    void test(){
      std::string data_file_name("./utInitFile_ReadString_after_open_and_close.cfg");
      {
        std::ofstream data_file(data_file_name.c_str());
        data_file << "TEST_VALUE BLAH" << std::endl;
        data_file << std::endl;
      }

      std::string filename("./cInitFile_serialize_closed_file.xml");

      // Open cInitFile_data.txt for reading.
      cInitFile f(data_file_name.c_str());
      f.Load();
      f.Compress();
      f.Close();

      const cString keyword("TEST_VALUE");
      const cString default_val("ICK");
      const cString loaded_val(f.ReadString(keyword, default_val));
      const cString expected_val("BLAH");
      BOOST_TEST(!f.IsOpen());
      BOOST_TEST(loaded_val == expected_val);
      std::cout << "utInitFile_ReadString_after_open_and_close:" << std::endl;
      std::cout << "  keyword: " << keyword << std::endl;
      std::cout << "  default_val: " << default_val << std::endl;
      std::cout << "  loaded_val: " << loaded_val << std::endl;
      std::cout << "  expected_val: " << expected_val << std::endl;

      std::remove(data_file_name.c_str());
    }
  } // utInitFile_ReadString_after_open_and_close
} // nInitFileTests

void cInitFile::UnitTests(bool full)
{
  if(full) {
    std::cout << "nInitFileTests::utInitFile_hello_world" << std::endl;
    nInitFileTests::utInitFile_hello_world::test();
  }
  if(full) {
    std::cout << "nInitFileTests::utInitFile_archiving" << std::endl;
    nInitFileTests::utInitFile_archiving::test();
  }
  if(full) {
    std::cout << "nInitFileTests::utInitFile_archiving_closed_file" << std::endl;
    nInitFileTests::utInitFile_archiving_closed_file::test();
  }
  if(1) {
    std::cout << "nInitFileTests::utInitFile_ReadString_after_open_and_close" << std::endl;
    nInitFileTests::utInitFile_ReadString_after_open_and_close::test();
  }
}

#endif // ENABLE_UNIT_TESTS
