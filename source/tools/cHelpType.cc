/*
 *  cHelpType.cc
 *  Avida
 *
 *  Called "help_type.cc" prior to 12/7/05.
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

#include "cHelpType.h"

#include "cHelpAlias.h"
#include "cHelpEntry.h"
#include "cHelpFullEntry.h"
#include "cHelpManager.h"

#include <fstream>

using namespace std;


cHelpAlias * cHelpType::AddAlias(const cString & alias_name, cHelpFullEntry * entry) {
  cHelpAlias * new_alias = new cHelpAlias(alias_name, entry);
  entry_list.Push(new_alias);
  return new_alias;
}

cHelpFullEntry * cHelpType::AddEntry(const cString & _name, const cString & _desc) {
  cHelpFullEntry * new_entry = new cHelpFullEntry(_name, this, _desc);
  entry_list.Push(new_entry);
  num_entries++;
  return new_entry;
}

cHelpEntry * cHelpType::FindEntry(const cString & entry_name) {
  tListIterator<cHelpEntry> entry_it(entry_list);
  while (entry_it.Next() != NULL) {
    if (entry_it.Get()->GetName() == entry_name) {
      return entry_it.Get(); // Found!
    }
  }
  return NULL;  // Not found...
}

cHelpType::~cHelpType() { while (entry_list.GetSize() > 0) delete entry_list.Pop(); }

void cHelpType::PrintHTML()
{
  if (manager->GetVerbose()) cout << "  Category: " << GetName() << endl;

  cHelpEntry * cur_entry = NULL;
  tListIterator<cHelpEntry> entry_it(entry_list);

  while ( (cur_entry = entry_it.Next()) != NULL) {
    // Only print non-alias entries...
    if (cur_entry->IsAlias() == true) continue;
    
    ofstream fp(cur_entry->GetHTMLFilename());
    
    fp << "<html>" << endl
       << "<title>" << GetName() << " : "
       << cur_entry->GetName() << "</title>" << endl << endl
       << "<h1>" << cur_entry->GetName() << "</h1>" << endl << endl;
    
    cString out_desc( cur_entry->GetDesc() );
    int pos = 0;
    while ((pos = out_desc.Find('!', pos)) != -1) {
      // Grab the word we need to replace
      cString found_word(out_desc.GetWordAt(pos));
      cString new_word;
      
      // If we have a double '!' reduce it to a single one and continue...
      if (found_word[1] == '!') {
	out_desc.Replace("!!", "!", pos);
	pos++;
      }
      
      // Otherwise, do a proper replacement...
      else {
	// Find the root keyword...
	cString keyword(found_word);
	keyword.ClipFront(1); // Clip off the '!' on the string.
	
	// Clip end punctuation and save it if there is any.
	char end_char = ' ';
	const int last_pos = keyword.GetSize() - 1;
	if (keyword.IsNumeric(last_pos) == false &&
	    keyword.IsLetter(last_pos) == false) {
	  end_char = keyword[keyword.GetSize() - 1];
	  keyword.ClipEnd(1);
	}
	// User can end a word in a '!' to preserve other punctuation...
	if (end_char == '!') end_char = ' ';

	// Determine what filename contains the new word...
	cHelpEntry * found_entry = manager->FindEntry(keyword);
	if (found_entry == NULL) {
	  if (manager->GetVerbose() == true) {
	    cerr << "  Warning: unknown help keyword \""
		 << keyword << "\"." << endl;
	  }
	  new_word.Set("<a href=\"help.%s.html\">%s</a>%c", static_cast<const char*>(keyword), static_cast<const char*>(keyword), end_char);
	}
	else {
	  new_word.Set("<a href=\"%s\">%s</a>%c", static_cast<const char*>(found_entry->GetHTMLFilename()), static_cast<const char*>(keyword), end_char);
	}

	// Rebuild the description with the new word...
	out_desc.Replace(found_word, new_word, pos);
	pos += new_word.GetSize();
      }
    }

    fp << out_desc << endl;
  }
}

