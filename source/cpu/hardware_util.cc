//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2003 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef HARDWARE_UTIL_HH
#include "hardware_util.hh"
#endif

#ifndef CONFIG_HH
#include "config.hh"
#endif
#ifndef HARDWARE_UTIL_HH
#include "hardware_base.hh"
#endif
#ifndef HARDWARE_CPU_HH
#include "hardware_cpu.hh"
#endif
#ifndef HARDWARE_4STACK_HH
#include "hardware_4stack.hh"
#endif
#ifndef INIT_FILE_HH
#include "init_file.hh"
#endif
#ifndef INST_LIB_CPU_HH
#include "inst_lib_cpu.hh"
#endif
#ifndef INST_SET_HH
#include "inst_set.hh"
#endif

using namespace std;


void cHardwareUtil::LoadInstSet(cString filename, cInstSet & inst_set, 
				tDictionary<int> & nop_dict,
				tDictionary<int> & inst_dict
){
  // If there is no filename, use the default for the appropriate hardware.
  cString default_filename = "unknown";
  if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_ORIGINAL) {
    default_filename = cHardwareCPU::GetDefaultInstFilename();
  }
  else if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_4STACK) {
    default_filename = cHardware4Stack::GetDefaultInstFilename();
  }

  if (filename == "") {
    cerr << "Warning: No instruction set specified; using default '"
	 << filename << "'." << endl;
    filename = default_filename;
  }

  cInitFile file(filename);

  // If we could not open the instruction set what to do?
  if (file.IsOpen() == false) {

    // If this is the default filename, write the file and try again.
    if (filename == default_filename) {
      if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_ORIGINAL) {
	cHardwareCPU::WriteDefaultInstSet();
      }
      else if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_4STACK) {
	cHardware4Stack::WriteDefaultInstSet();
      }      
    }

    // If this is not the default filename, give and error and stop.
    else {
      cerr << "Error: Could not open instruction set '" << filename
	   << "'.  Halting." << endl;
      exit(1);
    }
  }

  file.Load();
  file.Compress();

  for (int line_id = 0; line_id < file.GetNumLines(); line_id++) {
    cString cur_line = file.GetLine(line_id);
    cString inst_name = cur_line.PopWord();
    int redundancy = cur_line.PopWord().AsInt();
    int cost = cur_line.PopWord().AsInt();
    int ft_cost = cur_line.PopWord().AsInt();
    double prob_fail = cur_line.PopWord().AsDouble();

    // If this instruction has 0 redundancy, we don't want it!
    if (redundancy < 0) continue;
    if (redundancy > 256) {
      cerr << "Error: Max redundancy is 256.  Resetting redundancy of \""
	   << inst_name << "\" from " << redundancy << " to 256." << endl;
      redundancy = 256;
    }
    
    // Otherwise, this instruction will be in the set.
    // First, determine if it is a nop...
    int nop_mod = -1;
    if(nop_dict.Find(inst_name, nop_mod) == true) {
      inst_set.AddNop2(nop_mod, redundancy, ft_cost, cost, prob_fail);
      continue;
    }

    // Otherwise, it had better be in the main dictionary...
    int fun_id = -1;
    if(inst_dict.Find(inst_name, fun_id) == true){
      inst_set.Add2(fun_id, redundancy, ft_cost, cost, prob_fail);
      continue;
    }

    // Oh oh!  Didn't find an instruction!
    cerr << endl
	 << "Error: Could not find instruction '" << inst_name << "'" << endl
	 << "       (Best match = '"
	 << inst_dict.NearMatch(inst_name) << "')" << endl;
    exit(1);
  }

  cerr << "Loaded Instruction Library \"" << filename
       << "\" with " << inst_set.GetSize() << " instructions." << endl;
}

void cHardwareUtil::LoadInstSet_CPUOriginal(const cString & filename, cInstSet & inst_set)
{
  // Nops (or other modifying-instructions...)  Note: Nops must be the
  //  first instructions added to the set.

  // Build a dictionary of instructions and their corresponding methods...
  
  tDictionary<int> nop_dict;
  for(int i=0; i<inst_set.GetInstLib()->GetNumNops(); i++)
    nop_dict.Add(inst_set.GetInstLib()->GetNopName(i), i);

  tDictionary<int> inst_dict;
  for(int i=0; i<inst_set.GetInstLib()->GetSize(); i++)
    inst_dict.Add(inst_set.GetInstLib()->GetName(i), i);

  // And load it on up!
  LoadInstSet(filename, inst_set, nop_dict, inst_dict);
}

void cHardwareUtil::LoadInstSet_CPU4Stack(const cString & filename, cInstSet & inst_set)
{
  // Nops (or other modifying-instructions...)  Note: Nops must be the
  //  first instructions added to the set.

  // Build a dictionary of instructions and their corresponding methods...
  
  tDictionary<int> nop_dict;
  for(int i=0; i<inst_set.GetInstLib()->GetNumNops(); i++)
    nop_dict.Add(inst_set.GetInstLib()->GetNopName(i), i);

  tDictionary<int> inst_dict;
  for(int i=0; i<inst_set.GetInstLib()->GetSize(); i++)
    inst_dict.Add(inst_set.GetInstLib()->GetName(i), i);

  cout << "Instruction Library in util has " << inst_dict.GetSize()
       << " instructions and " << nop_dict.GetSize() <<  " nops." << endl;


  // And load it on up!
  LoadInstSet(filename, inst_set, nop_dict, inst_dict);
}

cInstSet & cHardwareUtil::DefaultInstSet(const cString & inst_filename)
{
  static cInstSet inst_set;

  // If we don't have an instruction set yet, and we've been given a filename,
  // set it up.
  if (inst_filename != "" && inst_set.GetSize() == 0) {
    if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_ORIGINAL) {
      inst_set.SetInstLib(cHardwareCPU::GetInstLib());
      LoadInstSet_CPUOriginal(inst_filename, inst_set);
    }
    else if (cConfig::GetHardwareType() == HARDWARE_TYPE_CPU_4STACK) {
      inst_set.SetInstLib(cHardware4Stack::GetInstLib());
      LoadInstSet_CPU4Stack(inst_filename, inst_set);
    }
  }

  return inst_set;
}

