# -*- coding: utf-8 -*-

import shutil, string, pyInstructionSet

class pyWriteGenesis:

  def __init__(self, in_dict = None, freeze_dir = None, out_dir = None):
  
    settings_dict = in_dict["SETTINGS"]
	
    # Copies default event and environment files and adds names to the 
    # incoming dictionary

    shutil.copyfile("events.default", out_dir + "events.cfg")
    shutil.copyfile("environment.default", out_dir + "environment.cfg")
    shutil.copyfile("inst_set.default", out_dir + "inst_set.default")
    shutil.copyfile(freeze_dir + settings_dict["START_CREATURE"],
                    out_dir + settings_dict["START_CREATURE"])
    settings_dict["EVENT_FILE"] = out_dir + "events.cfg"
    settings_dict["ENVIRONMENT_FILE"] = out_dir + "environment.cfg"
    settings_dict["INST_SET"] = out_dir + "inst_set.default"
    settings_dict["START_CREATURE"] = out_dir + settings_dict["START_CREATURE"]
    
    # Read the default genesis file, if there is a equivilent line in the 
    # dictionary replace it the new values, otherwise just copy the line
  
    orig_genesis_file = open("genesis.default")
    lines = orig_genesis_file.readlines()
    orig_genesis_file.close()
    out_genesis_file = open(out_dir + "genesis.avida", "w")
    for line in lines:
      comment_start = line.find("#")
      if comment_start > -1:
        if comment_start == 0:
          clean_line = ""
        else:
          clean_line = line[:comment_start]
      else:
        clean_line = line;
      clean_line = clean_line.strip()
      if len(clean_line) > 0:
        var_name, value = string.split(clean_line)
        var_name = var_name.upper()
        if settings_dict.has_key(var_name) == True:
          out_genesis_file.write(var_name + " " + str(settings_dict[var_name]) + "\n")
        else:
          out_genesis_file.write(line)
      else:
         out_genesis_file.write(line)
    out_genesis_file.close()