/*
 *  cAvidaConfig.cc
 *  Avida2
 *
 *  Created by David on 10/16/05.
 *  Copyright 2005 Michigan State University. All rights reserved.
 *
 */

#include "cAvidaConfig.h"

#include <fstream>
#include "defs.hh"
#include "cInitFile.h"

tList<cAvidaConfig::cBaseConfigGroup> cAvidaConfig::global_group_list;

cAvidaConfig::cBaseConfigEntry::cBaseConfigEntry(const cString & _name,
  const cString & _type, const cString & _def, const cString & _desc)
: config_name(_name)
, type(_type)
, default_value(_def)
, description(_desc)
, use_overide(true)
{
  // If the default value was originally a string, it will begin and end with
  // quotes.  We should make sure to remove those.
  if (default_value[0] == '"') {
    cout << "Removing quotes from " << default_value << endl;
    default_value = default_value.Substring(1, default_value.GetSize() - 2);
    cout << "...now "  << default_value << endl;
  }
}


cAvidaConfig::cAvidaConfig()
{
  group_list.Transfer(global_group_list);
}

cAvidaConfig::~cAvidaConfig()
{
}

void cAvidaConfig::Load(const cString & filename)
{
  // Load the contents from the file.
  cInitFile init_file(filename);
  init_file.Load();
  init_file.Compress();
  init_file.Close();
  
  // Loop through all groups, then all entrys, and try to load each one.
  tListIterator<cBaseConfigGroup> group_it(group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    
    // Loop through entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry * cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      const cString keyword = cur_entry->GetName();
      const cString default_val = cur_entry->GetDefault();
      cur_entry->LoadString( init_file.ReadString(keyword, default_val) );
    }
  }
}

void cAvidaConfig::Print(const cString & filename)
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
  
  tListIterator<cBaseConfigGroup> group_it(group_list);
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
  cout << "Config contains " << group_list.GetSize() << " groups." << endl;
  
  tListIterator<cBaseConfigGroup> group_it(group_list);
  cBaseConfigGroup * cur_group;
  while ((cur_group = group_it.Next()) != NULL) {
    cout << "Group \"" << cur_group->GetName()
    << "\" has " << cur_group->GetEntryList().GetSize()
    << " settings." << endl;
  }
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
  tListIterator<cBaseConfigGroup> group_it(group_list);
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
      const cString & cur_name = cur_entry->GetName();
      const cString & cur_default = cur_entry->GetDefault();
      const cString & cur_type = cur_entry->GetType();
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
      fp << "  void LoadString(const cString & str_value) {" << endl;
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
