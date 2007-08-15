/*
 *  cAvidaConfig.cc
 *  Avida
 *
 *  Created by David on 10/16/05.
 *  Designed by Charles.
 *  Copyright 1999-2007 Michigan State University. All rights reserved.
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

#include <fstream>
#include "defs.h"
#include "cActionLibrary.h"
#include "cDriverManager.h"
#include "cInitFile.h"
#include "cStringIterator.h"
#include "tDictionary.h"

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
    if (default_value.GetSize() > 2)
      default_value = default_value.Substring(1, default_value.GetSize() - 2);
    else default_value = "";
  }
}

void cAvidaConfig::Load(const cString & filename, 
                        const bool & crash_if_not_found = false)
{
  // Load the contents from the file.
  cInitFile init_file(filename);
  
  if (!init_file.WasOpened()) {
    if (crash_if_not_found) {
      // exit the program if the requested configuration file is not found
      cerr << "Warning: Unable to find file '" << filename 
           << "'.  Ending the program." << endl;
      exit(-1);
    } else {
      // If we failed to open the config file, try creating it.
      cerr << "Warning: Unable to find file '" << filename 
           << "'.  Creating default." << endl;
      Print(filename);
    }
  }
  
  cString version_id = init_file.ReadString("VERSION_ID", "Unknown");
  if (version_id != VERSION) {
    cerr << "Warning: Configuration file version number mismatch." << endl;
    cerr << "         Avida Version: \"" << VERSION << "\".  Config Version: \"" << version_id << "\"" << endl;
  }
  

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
  
  init_file.WarnUnused();
}

/* Routine to create an avida configuration file from internal default values */

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

void cAvidaConfig::PrintReview()
{
  cout << endl << "Non-Default Settings: " << endl << endl;

  // Loop through all possible groups.
  tListIterator<cBaseConfigGroup> group_it(group_list);
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

cAvidaConfig* cAvidaConfig::LoadWithArgs(cStringList &argv)
{
  cString config_filename = "avida.cfg";
  bool crash_if_not_found = false;
  tDictionary<cString> sets;
  
  int arg_num = 1;              // Argument number being looked at.
  
  // Load all of the args into string objects for ease of access.
  int argc = argv.GetSize();
  cString* args = new cString[argc];
  cStringIterator list_it(argv);
  for (int i = 0; (i < argc) && (list_it.AtEnd() == false); i++) {
    list_it.Next();
    args[i] = list_it.Get();
  }
  
  // -config option
  if (argc > 1 && (args[1] == "-c" || args[1] == "-config")) {
    if (argc < 3) {
      cerr << "Error: Filename for configuration must be specified." << endl;
      exit(0);
    }
    config_filename = args[2];
    crash_if_not_found = true;
    arg_num += 2;
  } else if (argc > 1 && (args[1] == "-g" || args[1] == "-genesis")) {
    cerr << "Warning: Use of -g[enesis] deprecated in favor of -c[onfig]." << endl;
    if (argc < 3) {
      cerr << "Error: Filename for configuration must be specified." << endl;
      exit(0);
    }
    config_filename = args[2];
    crash_if_not_found = true;
    arg_num += 2;
  }
  
  // Create Config object, load with values from configuration file
  cAvidaConfig* cfg = new cAvidaConfig();
  cfg->Load(config_filename, crash_if_not_found);
  
  // Then scan through and process the rest of the args.
  while (arg_num < argc) {
    cString cur_arg = args[arg_num];
    
    // Test against the possible inputs.

    // Print out a list of all possibel actions (was events).
    if (cur_arg == "-e" || cur_arg == "-events" || cur_arg == "-actions") {
      cout << endl << "Supported Actions:" << endl;
      cout << cDriverManager::GetActionLibrary()->DescribeAll() << endl;
      exit(0);
    }

    // Review configuration options, listing those non-default.
    else if (cur_arg == "-review" || cur_arg == "-r") {
      cfg->PrintReview();
      exit(0);
    }
    
    else if (cur_arg == "--help" || cur_arg == "-help" || cur_arg == "-h") {
      cout << "Options:"<<endl
	   << "  -a[nalyze]            Process analyze.cfg instead of normal "
               << "run." << endl
	   << "  -c[onfig] <filename>  Set config file to be <filename>"<<endl
	   << "  -e; -actions          Print a list of all known actions"<< endl
	   << "  -h[elp]               Help on options (this listing)"<<endl
	   << "  -i[nteractive]        Run analyze mode interactively" << endl
	   << "  -l[oad] <filename>    Load a clone file" << endl
	   << "  -r[eview]             Review avida.cfg settings." << endl
	   << "  -s[eed] <value>       Set random seed to <value>" << endl
	   << "  -set <name> <value>   Overide values in avida.cfg" << endl
	   << "  -v[ersion]            Prints the version number" << endl
	   << "  -v0 -v1 -v2 -v3 -v4   Set output verbosity to 0..4" << endl
	   << endl;
      
      exit(0);
    }
    else if (cur_arg == "-seed" || cur_arg == "-s") {
      int in_seed = 0;
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
        cerr << "Error: Must include a number as the random seed!" << endl;
        exit(0);
      } else {
        arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
        in_seed = cur_arg.AsInt();
      }
      cfg->RANDOM_SEED.Set(in_seed);
    } else if (cur_arg == "-analyze" || cur_arg == "-a") {
      if (cfg->ANALYZE_MODE.Get() < 1) {
        cfg->ANALYZE_MODE.Set(1);
      }
    } else if (cur_arg == "-interactive" || cur_arg == "-i") {
      if (cfg->ANALYZE_MODE.Get() < 2) {
        cfg->ANALYZE_MODE.Set(2);
      }
    } else if (cur_arg == "-load" || cur_arg == "-l") {
      if (arg_num + 1 == argc || args[arg_num + 1][0] == '-') {
        cerr << "Error: Must include a filename to load from." << endl;
        exit(0);
      } else {
        arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
        cfg->CLONE_FILE.Set(cur_arg);
      }
    } else if (cur_arg == "-version" || cur_arg == "-v") {
      // We've already showed version info, so just quit.
      exit(0);
    } else if (cur_arg.Substring(0, 2) == "-v") {
      int level = cur_arg.Substring(2, cur_arg.GetSize() - 2).AsInt();
      cfg->VERBOSITY.Set(level);
    } else if (cur_arg == "-set") {
      if (arg_num + 1 == argc || arg_num + 2 == argc) {
        cerr << "'-set' option must be followed by name and value" << endl;
        exit(0);
      }
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString name(cur_arg);
      arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
      cString value(cur_arg);
      sets.Add(name, value);
    } else if (cur_arg == "-c" || cur_arg == "-config") {
      cerr << "Error: -c[onfig] option must be listed first." << endl;
      exit(0);
    } else if (cur_arg == "-g" || cur_arg == "-genesis") {
      cerr << "Warning: Use of '-g[enesis]' deprecated in favor or -c[onfig]." << endl;
      cerr << "Error: -c[onfig] option must be listed first." << endl;
      exit(0);
    } else {
      cerr << "Error: Unknown Option '" << args[arg_num] << "'" << endl
      << "Type: \"" << args[0] << " -h\" for a full option list." << endl;
      exit(0);
    }
    
    arg_num++;  if (arg_num < argc) cur_arg = args[arg_num];
  }
  
  // Loop through all groups, then all entries, and try to load each one.
  tListIterator<cBaseConfigGroup> group_it(cfg->group_list);
  cBaseConfigGroup* cur_group;
  cString val;
  while ((cur_group = group_it.Next()) != NULL) {
    // Loop through entries for this group...
    tListIterator<cBaseConfigEntry> entry_it(cur_group->GetEntryList());
    cBaseConfigEntry* cur_entry;
    while ((cur_entry = entry_it.Next()) != NULL) {
      if (sets.Find(cur_entry->GetName(), val)) {
        cur_entry->LoadString(val);
        if (cfg->VERBOSITY.Get() > VERBOSE_NORMAL)
          cout << "CmdLine Set: " << cur_entry->GetName() << " " << val << endl;
      }
    }
  }
  
  delete [] args;
  
  return cfg;
}
cAvidaConfig* cAvidaConfig::LoadWithCmdLineArgs(int argc, char * argv[])
{
  cStringList sl;
  for(int i=0; i<argc; i++){
    sl.PushRear(argv[i]);
  }
  return LoadWithArgs(sl);
}

bool cAvidaConfig::Get(const cString& entry, cString& ret) const
{
  // Loop through all groups, then all entries, searching for the specified entry.
  tConstListIterator<cBaseConfigGroup> group_it(group_list);
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

bool cAvidaConfig::Set(const cString& entry, const cString& val)
{
  // Loop through all groups, then all entries, searching for the specified entry.
  tListIterator<cBaseConfigGroup> group_it(group_list);
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
  return false;
}
