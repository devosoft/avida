# -*- coding: utf-8 -*-

import shutil, string, pyInstructionSet

class pyWriteGenesis:

  def __init__(self, in_dict = None, workspace_dir = None, freeze_dir = None,
    out_dir = None):
  
    settings_dict = in_dict["SETTINGS"]
	
    # Copies default event and environment files and adds names to the 
    # incoming dictionary

    shutil.copyfile(workspace_dir + "events.default", out_dir + "events.cfg")
    
    # If this is a full petri dish inject all the organisms otherwise
    # inject the start creature in the center of the grid
    
    if in_dict.has_key("CELLS"):
      cells_dict = in_dict["CELLS"]
      organisms_dict = in_dict["ORGANISMS"]
    else:
      cells_dict = {}
      organisms_dict = {}
      if settings_dict.has_key("START_CREATURE"):
        world_x = settings_dict["WORLD-X"]
        world_y = settings_dict["WORLD-Y"]
        self.start_cell_location = int(((world_y/2) * world_x) +(world_x/2))
        cells_dict[str(self.start_cell_location)] = str(1)
        tmp_inst_set = pyInstructionSet.pyInstructionSet(workspace_dir + "inst_set.default")
        org_string = tmp_inst_set.OrgFile2LetterString(freeze_dir + settings_dict["START_CREATURE"])
        organisms_dict[str(1)] = org_string
    self.createInjects(cells_dict, organisms_dict, out_dir + "events.cfg")
    
    shutil.copyfile(workspace_dir + "environment.default", out_dir + "environment.cfg")
    shutil.copyfile(workspace_dir + "inst_set.default", out_dir + "inst_set.default")

    # shutil.copyfile(freeze_dir + settings_dict["START_CREATURE"],
    #                 out_dir + settings_dict["START_CREATURE"])
    settings_dict["EVENT_FILE"] = out_dir + "events.cfg"
    settings_dict["ENVIRONMENT_FILE"] = out_dir + "environment.cfg"
    settings_dict["INST_SET"] = out_dir + "inst_set.default"
    # settings_dict["START_CREATURE"] = out_dir + settings_dict["START_CREATURE"]
    
    # Read the default genesis file, if there is a equivilent line in the 
    # dictionary replace it the new values, otherwise just copy the line
  
    orig_genesis_file = open(workspace_dir + "genesis.default")
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
    
  def createInjects(self, cells_dict, organisms_dict, out_file):
    event_out_file = open(out_file, 'a')
    for cell in cells_dict.keys():
      part1 = "u 0 inject_sequence " +  organisms_dict[cells_dict[cell]] + " " 
      part2 = cell + " " + str(int(cell)+1) + " -1 "
      part3 = cells_dict[cell] + "\n"
      event_out_file.write(part1 +  part2 + part3)
    event_out_file.close()
    
    
