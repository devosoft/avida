/*
 *  cInitFile.cc
 *  Avida
 *
 *  Called "init_file.cc" prior to 12/7/05.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
 *  Copyright 1993-2003 California Institute of Technology.
 *
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "cInitFile.h"

#include "cFile.h"
#include "cStringIterator.h"


using namespace std;


cInitFile::cInitFile(const cString& filename) : m_filename(filename), m_ftype("unknown")
{
  tSmartArray<sLine*> lines;
  m_opened = LoadFile(filename, lines);
  PostProcess(lines);
}

cInitFile::cInitFile(const cString& filename, const tDictionary<cString>& mappings)
  : m_filename(filename), m_ftype("unknown")
{
  InitMappings(mappings);
  tSmartArray<sLine*> lines;
  m_opened = LoadFile(filename, lines);
  PostProcess(lines);
}


cInitFile::cInitFile(istream& in_stream) : m_filename("(stream)"), m_ftype("unknown")
{
  if (in_stream.good() == false) {
    cerr << "Bad stream sent to cInitFile::LoadStream()" << endl;
    m_opened = false;
    return;
  }
  
  tSmartArray<sLine*> lines;
  
  char cur_line[MAX_STRING_LENGTH];
  in_stream.getline(cur_line, MAX_STRING_LENGTH);
  
  // If this file doesn't work properly, return.
  if (!in_stream && !strlen(cur_line)) {
    m_opened = false;
    return;
  }
  
  int linenum = 1;
  while (in_stream) {
    if (cur_line[0] == '#' && cur_line[1] == '!') ProcessCommand(cur_line, lines);
    else lines.Push(new sLine(cur_line, "(stream)", linenum));
    
    in_stream.getline(cur_line, MAX_STRING_LENGTH);
    linenum++;
  }
  
  PostProcess(lines);
}


void cInitFile::InitMappings(const tDictionary<cString>& mappings)
{
  tList<cString> names;
  mappings.GetKeys(names);
  
  tListIterator<cString> names_it(names);
  while (names_it.Next() != NULL) {
    cString* name = names_it.Get();
    cString value;
    if (mappings.Find(*name, value)) m_mappings.Add(*name, value);
  }
}


bool cInitFile::LoadFile(const cString& filename, tSmartArray<sLine*>& lines)
{
  cFile file(filename);
  if (!file.IsOpen()) return false;   // The file must be opened!
  
  cStringList line_list;   // Create a list to load all of the lines into.

  int linenum = 0;
  cString buf;
  while (!file.Eof() && file.ReadLine(buf)) {
    linenum++;

    if (buf.GetSize() > 1 && buf[0] == '#' && buf[1] == '!') ProcessCommand(buf, lines);
    else lines.Push(new sLine(buf, filename, linenum));
  }
  file.Close();
  
  if (linenum == 0) return false; // @DMB - should this be the case?
  
  return true;
}


void cInitFile::ProcessCommand(cString cmdstr, tSmartArray<sLine*>& lines)
{
  cString cmd = cmdstr.PopWord();
  
  if (cmd == "#!include") {
    cString dir = cmdstr.PopWord();
    if (dir.GetSize() && dir[0] == '$') {
      dir.ClipFront(1);
      if (!m_mappings.Find(dir, dir)) {
        // @TODO - report error
        return;
      }
    }
    LoadFile(dir, lines); // @TODO - report error
  } else if (cmd == "#!default") {
    cString name = cmdstr.PopWord();
    cmdstr.Trim();
    if (!m_mappings.HasEntry(name)) m_mappings.Add(name, cmdstr);
  }
}


void cInitFile::PostProcess(tSmartArray<sLine*>& lines)
{
  if (lines.GetSize() >= 2) {
    cString type_line = lines[0]->line;
    cString format_line = lines[1]->line;
    
    if (type_line.PopWord() == "#filetype") m_ftype = type_line.PopWord();
    if (format_line.PopWord() == "#format") m_format.Load(format_line);
  }

  
  // We're going to handle this compression in multiple passes to make it
  // clean and easy.

  const int num_lines = lines.GetSize();

  // PASS 1: Remove all comments -- everything after a '#' sign -- and
  // compress all whitespace into a single space.
  for (int i = 0; i < num_lines; i++) {
    cString& cur_line = lines[i]->line;

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
      lines[prev_line_id]->line += lines[i]->line;
      lines[i]->line = "";
    }
    else prev_line_id = i;

    // See if the prev_line is continued, and if it is, take care of it.
    cString& prev_line = lines[prev_line_id]->line;
    if (prev_line.GetSize() > 0 && prev_line[prev_line.GetSize() - 1] == '\\') {
      prev_line.ClipEnd(1);  // Remove continuation mark.
      continued = true;
    }
    else continued = false;
  }

  // PASS 3: Remove now-empty lines.

  int next_id = 0;
  for (int i = 0; i < num_lines; i++) {
    // If we should keep this line, compact it.
    if (lines[i]->line.GetSize() > 0) {
      if (next_id != i) {
        delete lines[next_id];
        lines[next_id] = lines[i];
        lines[i] = NULL;
      }
      next_id++;
    }
  }

  // Resize the internal line structure and move the line structs to it
  m_lines.Resize(next_id);
  for (int i = 0; i < next_id; i++) m_lines[i] = lines[i];
}


void cInitFile::Save(const cString& in_filename)
{
  cString save_filename(in_filename);
  if (save_filename != "") save_filename = m_filename;
  
  ofstream fp_save(save_filename);
  
  // Go through the lines saving them...
  for (int i = 0; i < m_lines.GetSize(); i++) {
    fp_save << m_lines[i]->line << endl;
  }
  
  fp_save.close();
}



cString cInitFile::GetLine(int line_num)
{
  if (line_num < 0 || line_num >= m_lines.GetSize()) return "";
  return m_lines[line_num]->line;
}


bool cInitFile::Find(cString& in_string, const cString& keyword, int col) const
{
  bool found = false;

  // Loop through all of the lines looking for this keyword.  Start with
  // the actual file...
  for (int line_id = 0; line_id < m_lines.GetSize(); line_id++) {
    cString cur_string = m_lines[line_id]->line;

    // If we found the keyword, return it and stop.    
    if (cur_string.GetWord(col) == keyword) {
      m_lines[line_id]->used = true;
      in_string = cur_string;
      found = true;
    }
  }

  return found;    // Not Found...
}


cString cInitFile::ReadString(const cString& name, cString def) const
{
  // See if we definately can't find the keyword.
  if (name == "") return def;

  // Search for the keyword.
  cString cur_line;
  if (Find(cur_line, name, 0) == false) {
    cerr << "Warning: " << name << " not in '" << m_filename << "', defaulting to: " << def << endl;
    return def;
  }

  // Pop off the keyword, and return the remainder of the line.
  cur_line.PopWord();
  return cur_line;
}


bool cInitFile::WarnUnused() const
{
  bool found = false;

  for (int i = 0; i < m_lines.GetSize(); i++) {
    if (m_lines[i]->used == false) {
      if (found == false) {
        found = true;
        cerr << "Warning: unknown lines in input file '" << m_filename << "'" << endl;
      }
      cerr << "    " << m_lines[i]->file << ":" << m_lines[i]->line_num << ": " << m_lines[i]->line << endl;
    }
  }
  
  if (found == true) cerr << endl;

  return found;
}
