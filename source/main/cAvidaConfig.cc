/*
 *  cAvidaConfig.cc
 *  Avida
 *
 *  Created by David on 10/16/05.
 *  Designed by Charles.
 *  Copyright 1999-2010 Michigan State University. All rights reserved.
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

#include "cAvidaConfig.h"

#include "Avida.h"
#include "AvidaTools.h"

#include "cActionLibrary.h"
#include "cDriverManager.h"
#include "cDriverStatusConduit.h"
#include "cInitFile.h"
#include "cStringIterator.h"
#include "cUserFeedback.h"
#include "tDictionary.h"

#include <fstream>

using namespace AvidaTools;


cMutex cAvidaConfig::global_list_mutex;
tList<cAvidaConfig::cBaseConfigGroup> cAvidaConfig::global_group_list;
tList<cAvidaConfig::cBaseConfigCustomFormat> cAvidaConfig::global_format_list;

cAvidaConfig::cBaseConfigEntry::cBaseConfigEntry(const cString& _name,
  const cString& _type, const cString& _def, const cString& _desc)
: config_name(1)
, type(_type)
, default_value(_def)
, description(_desc)
, use_overide(true)
{
  config_name[0] = _name;

  // If the default value was originally a string, it will begin and end with
  // quotes.  We should make sure to remove those.
  if (default_value[0] == '"') {
    if (default_value.GetSize() > 2)
      default_value = default_value.Substring(1, default_value.GetSize() - 2);
    else default_value = "";
  }
}

bool cAvidaConfig::Load(const cString& filename, const cString& working_dir, cUserFeedback* feedback,
                        const tDictionary<cString>* mappings, bool warn_default)
{
  tDictionary<cString> lmap;
  
  // Load the contents from the file.
  cInitFile init_file(filename, (mappings) ? *mappings : lmap, working_dir);
  
  if (!init_file.WasOpened()) {
    if (feedback) {
      feedback->Append(init_file.GetFeedback());
      if (init_file.WasFound()) {
        feedback->Error("unable to open configuration file '%s'", (const char*)filename);
      } else {
        feedback->Error("configuration file '%s' not found", (const char*)filename); 
      }
    }
    
    return false;
  }
  
  cString version_id = init_file.ReadString("VERSION_ID", "Unknown");
  if (version_id != VERSION) {
    if (feedback)
      feedback->Warning("config file version number mismatch -- Avida: '%s'  File: '%s'", VERSION, (const char*)version_id);
  }
  

  // Loop through all groups, then all entrys, and try to load each one.
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup* cur_group;
  while ((cur_group = group_it.Next()) != NULL) {    
    // Loop through entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      const tArray<cString> & keywords = cur_entry->GetNames();
      const cString default_val = cur_entry->GetDefault();
      cur_entry->LoadString( init_file.ReadString(keywords, default_val, warn_default) );
    }
  }
  
  
  // Build dictionary of custom format entries
  tDictionary<cBaseConfigFormatEntry*> entry_dict;
  
  tListIterator<cBaseConfigCustomFormat> format_it(m_format_list);
  cBaseConfigCustomFormat* cur_format;
  while ((cur_format = format_it.Next())) {
    tListIterator<cBaseConfigFormatEntry> entry_it(cur_format->GetEntryList());
    cBaseConfigFormatEntry* cur_entry;
    while ((cur_entry = entry_it.Next())) entry_dict.Set(cur_entry->GetName(), cur_entry);
  }
  
  
  // Pass over the file again, checking for custom format entries
  for (int line_id = 0; line_id < init_file.GetNumLines(); line_id++) {
    cString cur_line = init_file.GetLine(line_id);
    cString keyword = cur_line.PopWord();
    
    cBaseConfigFormatEntry* cur_entry;
    if (entry_dict.Find(keyword, cur_entry)) {
      cur_entry->LoadString(cur_line);
      init_file.MarkLineUsed(line_id);
    }
  }
  
  if (feedback) {
    init_file.WarnUnused();
    feedback->Append(init_file.GetFeedback());
  }
  return true;
}

/* Routine to create an avida configuration file from internal default values */

void cAvidaConfig::Print(const cString& filename)
{
  ofstream fp(filename);
  
  // Print out the generic header, including the version ID.
  fp << "#############################################################################" << endl
    << "# This file includes all the basic run-time defines for Avida." << endl
    << "# For more information, see doc/config.html" << endl
    << "#############################################################################" << endl
    << endl
    << "VERSION_ID " << VERSION << "   # Do not change this value."
    << endl;
  
  // Loop through the groups, and print out all of the variables.
  
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    // Print out the group name...
    fp << endl;
    fp << "### " << cur_group->GetName() << " ###" << endl;
    
    // If we have a comment about the current group, include it.  Make sure
    // to allow multi-line comments.
    cStringList group_desc(cur_group->GetDesc(), '\n');
    while (group_desc.GetSize() > 0) {
      fp << "# " << group_desc.Pop() << endl;
    }
    
    // Print out everything for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    
    // First, figure out the widest entry so we know where to put comments.
    int max_width = 0;
    cBaseConfigEntry * cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      int cur_width = cur_entry->GetName().GetSize() +
      cur_entry->GetDefault().GetSize() + 1;
      if (cur_width > max_width) max_width = cur_width;
    }
    
    // Now, make a second pass printing everything.
    entry_it.Reset();
    while ((cur_entry = entry_it.Next()) != NULL) {
      int cur_width = cur_entry->GetName().GetSize() +
      cur_entry->GetDefault().GetSize() + 1;
      // Print the variable and its setting...
      fp << cur_entry->GetName() << " " << cur_entry->GetDefault();
      
      // Print some spaces before the description.
      for (int i = cur_width; i < max_width; i++) fp << " ";
      
      // Print the first line of the description.
      // @CAO Again, we should allow multi-line comments....
      if (cur_entry->GetDesc().GetSize() == 0) {
        fp << "  # " << endl;
      } else {
        cStringList cur_desc(cur_entry->GetDesc(), '\n');
        fp << "  # " << cur_desc.Pop() << endl;
        while (cur_desc.GetSize() > 0) {
          for (int i = 0; i < max_width; i++) fp << " ";
          fp << "  # " << cur_desc.Pop() << endl;
        }
      }
    }
  }
}

void cAvidaConfig::Status()
{
  cout << "Config contains " << m_group_list.GetSize() << " groups." << endl;
  
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    cout << "Group \"" << cur_group->GetName()
    << "\" has " << cur_group->GetEntryList().GetSize()
    << " settings." << endl;
  }
}

void cAvidaConfig::PrintReview()
{
  cout << endl << "Non-Default Settings: " << endl << endl;

  // Loop through all possible groups.
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    // Loop through entries for this group...
    tConstListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    const cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      if (cur_entry->EqualsString( cur_entry->GetDefault() ) == false) {
	cout << " " << cur_entry->GetName() << " ";
	if (cur_entry->GetType() == "double") {
	  cout << cur_entry->AsString().AsDouble() << " ";
	} else if (cur_entry->GetType() == "int") {
	  cout << cur_entry->AsString().AsInt() << " ";
	} else {
	  cout << cur_entry->AsString() << " ";
	}
	cout << "(default=" << cur_entry->GetDefault() << ")"
	     << endl;
      }
    }
  }
  cout << endl;
  
}


void cAvidaConfig::GenerateOverides()
{
  ofstream fp("config_overrides.h");  
  
  // Print out a header for the top of the file.
  fp << "/****************************************************************************" << endl;
  fp << " * This is an automatically generated file.  This file will overide any     *" << endl;
  fp << " * values set by the user at runtime.  When Avida is compiled using these   *" << endl;
  fp << " * overides, it will reduce the flexibility of the user, but will increase  *" << endl;
  fp << " * the speed at which Avida runs.                                           *" << endl;
  fp << " *                                                                          *" << endl;
  fp << " * To use this file, include the compiler option -DOVERRIDE_CONFIGS         *" << endl;
  fp << " ***************************************************************************/" << endl;
  fp << endl;

  // Loop through all of the groups, printing the classes representing that
  // group followed by the classes representing the individual settings.
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    fp << "/**********************************************" << endl;
    fp << " * Group: " << cur_group->GetName() << endl;
    fp << " * Description: " << cur_group->GetDesc() << endl;
    fp << " **********************************************/" << endl;
    fp << endl;
    
    fp << "class cGroup_" << cur_group->GetName()
      << " : public cBaseConfigGroup {" << endl;
    fp << "public:" << endl;
    fp << "  cGroup_" << cur_group->GetName() << "()" << endl;
    
    // Pay special attention to multi-line group descriptions.
    fp << "    : cBaseConfigGroup(\""
      << cur_group->GetName() << "\", \"";
    cStringList group_desc(cur_group->GetDesc(), '\n');
    fp << group_desc.Pop();
    while (group_desc.GetSize() > 0) {
      fp << "\\n";
      fp << group_desc.Pop();
    }
    fp << "\") { ; }" << endl;
    fp << "} " << cur_group->GetName() << ";" << endl;
    fp << endl;
    
    // Print out the entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry * cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      const cString& cur_name = cur_entry->GetName();
      const cString& cur_default = cur_entry->GetDefault();
      const cString& cur_type = cur_entry->GetType();
      cStringList cur_desc(cur_entry->GetDesc(), '\n');
      fp << "// Entry: " << cur_name << endl;
      fp << "// Description: ";
      if (cur_desc.GetSize() > 0) fp << cur_desc.GetLine(0);
      fp << endl;
      for (int i = 1; i < cur_desc.GetSize(); i++) {
        fp << "//              " << cur_desc.GetLine(i) << endl;
      }
      fp << "class cEntry_" << cur_name
        << " : public cBaseConfigEntry {" << endl;
      fp << "private:" << endl;
      fp << "  " << cur_type << " value;" << endl;
      fp << "public:" << endl;
      fp << "  void LoadString(const cString& str_value) {" << endl;
      fp << "    value = cStringUtil::Convert(str_value, value);" << endl;
      fp << "  }" << endl;
      fp << "  cEntry_" << cur_name << "() : cBaseConfigEntry(\""
        << cur_name << "\", \"" << cur_type << "\", \""
        << cur_default << "\", \""; 
      if (cur_desc.GetSize() > 0) { fp << cur_desc.Pop(); }
      while (cur_desc.GetSize() > 0) {
        fp << "\\n" << cur_desc.Pop();
      }
      fp << "\") {" << endl;
      fp << "    LoadString(GetDefault());" << endl;
      fp << "    global_group_list.GetLast()->AddEntry(this);" << endl;
      fp << "  }" << endl;
      
      // This section will vary depending on if we are making this variable
      // a constant.
      if (cur_entry->GetUseOveride() == true) {
        // For string types we must enclose the value in quotes.
        if (cur_type == "cString") {
          fp << "  " << cur_type << " Get() const { return \""
          << cur_entry->AsString() << "\"; }" << endl;
        } else {
          fp << "  " << cur_type << " Get() const { return "
          << cur_entry->AsString() << "; }" << endl;
        }
        fp << "  void Set(" << cur_type
        << " in_value) { cerr << \"ERROR: resetting value for constant setting '" << cur_name << "'\" << endl; }" << endl;	
      } else {
        fp << "  " << cur_type << " Get() const { return value; }" << endl;
        fp << "  void Set(" << cur_type
          << " in_value) { value = in_value; }" << endl;
      }
      
      fp << "  cString AsString() { return cStringUtil::Convert(value); }" << endl;
      fp << "} " << cur_name << ";" << endl;
      fp << endl;
    }
    
    // Skip an extra line between groups
    fp << endl;
  }  
}


bool cAvidaConfig::Get(const cString& entry, cString& ret) const
{
  // Loop through all groups, then all entries, searching for the specified entry.
  tConstListIterator<cBaseConfigGroup> group_it(m_group_list);
  const cBaseConfigGroup* cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    // Loop through entries for this group...
    tConstListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    const cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      if (cur_entry->GetName() == entry) {
        ret = cur_entry->AsString();
        return true;
      }
    }
  }
  return false;
}

cString cAvidaConfig::GetAsString(const cString& entry) const
{
  // Default to empty string on lookup failure
  cString rtn("");
  if (!Get(entry, rtn))
    cDriverManager::Status().SignalError(cString("config entry '") + entry + "' not found");
  return rtn;
}

bool cAvidaConfig::Set(const cString& entry, const cString& val)
{
  // Loop through all groups, then all entries, searching for the specified entry.
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup* cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    // Loop through entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      if (cur_entry->GetName() == entry) {
        cur_entry->LoadString(val);
        return true;
      }
    }
  }
  tListIterator<cBaseConfigCustomFormat> format_it(m_format_list);
  cBaseConfigCustomFormat* cur_format;
  while ((cur_format = format_it.Next())) {
    tListIterator<cBaseConfigFormatEntry> entry_it(cur_format->GetEntryList());
    cBaseConfigFormatEntry* cur_entry;
    while ((cur_entry = entry_it.Next())) {
      if (cur_entry->GetName() == entry) {
        cur_entry->LoadString(val);
        return true;
      }
    }
  }
  
  return false;
}


void cAvidaConfig::Set(tDictionary<cString>& sets)
{
  // Loop through all groups, then all entries, and try to load each one.
  tListIterator<cBaseConfigGroup> group_it(m_group_list);
  cBaseConfigGroup* cur_group;
  cString val;
  while ((cur_group = group_it.Next()) != NULL) {
    // Loop through entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      for (int i = 0; i < cur_entry->GetNumNames(); i++) {
	if (sets.Find(cur_entry->GetName(i), val)) {
	  cur_entry->LoadString(val);
	  sets.Remove(cur_entry->GetName(i));
	  if (VERBOSITY.Get() > VERBOSE_NORMAL)
	    cout << "CmdLine Set: " << cur_entry->GetName() << " " << val << endl;
	  break;
	}
      }
    }
  }
}
