/*
 *  cHelpManager.cc
 *  Avida
 *
 *  Called "help_manager.cc" prior to 12/7/05.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cHelpManager.h"

#include "AvidaTools.h"
#include "cHelpType.h"
#include "cInitFile.h"
#include "cString.h"
#include "tList.h"

#include <iostream>

using namespace std;


cHelpType* cHelpManager::GetType(const cString type_name)
{
  // See if we can find this type in the already existant list...
  tListIterator<cHelpType> type_it(type_list);
  while (type_it.Next() != NULL) {
    if (type_it.Get()->GetName() == type_name) return type_it.Get();
  }

  // Otherwise, create it.
  if (verbose == true) {
    cout << "  Creating help type \"" << type_name << "\"." << endl;
  }
  cHelpType * type = new cHelpType(type_name, this);
  type_list.Push(type);
  return type;
}

void cHelpManager::LoadFile(const cString & filename)
{
  cInitFile help_file(filename, AvidaTools::FileSystem::GetCWD());

  cHelpType * type = NULL;
  cString keyword;

  for (int line_id = 0; line_id < help_file.GetNumLines(); line_id++) {
    cString cur_string = help_file.GetLine(line_id);
    cString command = cur_string.PopWord();
    command.ToLower();

    if (command == "type:") {
      type = GetType(cur_string);
    }
    else if (command == "keyword:") {
      keyword = cur_string;
    }
    else if (command == "desc:") {
      if (type == NULL) type = GetType("None"); // Make sure we have a type.
      if (keyword == "") {
	if (verbose == true) {
	  cerr << "  Help description set without keyword;"
	       << " setting keyword to \"None\"." << endl;
	}
	keyword = "None";
      }
      last_entry = type->AddEntry(keyword, cur_string);
    }
    else if (command == "alias:") {
      if (last_entry == NULL) {
	cerr << "  Warning: Setting aliases \"" << cur_string
	     << "\" to incomplete entry!" << endl;
      }
      while (cur_string.GetSize() > 0) {
	type->AddAlias(cur_string.PopWord(), last_entry);
      }
    }
    else if (verbose == true) {
      cerr << "Warning: Unknown command \"" << command
	   << "\" in file " << filename << endl;
    }
  }

}


void cHelpManager::PrintHTML()
{
  tListIterator<cHelpType> type_it(type_list);
  while (type_it.Next() != NULL) {
    type_it.Get()->PrintHTML();
  }
}
