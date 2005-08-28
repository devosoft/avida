# -*- coding: utf-8 -*-

import shutil, string, pyInstructionSet, os.path

# Class to write the working genesis and event files

class pyWriteGenesisEvent:

  def __init__(self, in_dict = None, workspace_dir = None, freeze_dir = None,
    tmp_in_dir = None, tmp_out_dir = None):
  
    settings_dict = in_dict["SETTINGS"]
	
    # Copies default event and environment files and adds names to the 
    # temporary dictionary where the input files will live

    shutil.copyfile(os.path.join(workspace_dir, "events.default"), os.path.join(tmp_in_dir, "events.cfg"))
    
    # If this is a full petri dish inject all the organisms, otherwise
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
        self.start_cell_location = int(((world_y/2) * world_x) + (world_x/2))
        cells_dict[str(self.start_cell_location)] = str(1)

        # Read the genome from the organism file 

        org_file = open(os.path.join(freeze_dir, settings_dict["START_CREATURE"]))
        org_string = org_file.readline()
        org_string = org_string.rstrip()
        org_string = org_string.lstrip()
        org_file.close
        organisms_dict[str(1)] = org_string
    self.modifyEventFile(cells_dict, organisms_dict, 
      os.path.join(tmp_in_dir, "events.cfg"), tmp_out_dir)
    
    shutil.copyfile(os.path.join(workspace_dir, "environment.default"), os.path.join(tmp_in_dir, "environment.cfg"))
    shutil.copyfile(os.path.join(workspace_dir, "inst_set.default"), os.path.join(tmp_in_dir, "inst_set.default"))

    settings_dict["EVENT_FILE"] = os.path.join(tmp_in_dir, "events.cfg")
    settings_dict["ENVIRONMENT_FILE"] = os.path.join(tmp_in_dir, "environment.cfg")
    settings_dict["INST_SET"] = os.path.join(tmp_in_dir, "inst_set.default")
    # settings_dict["START_CREATURE"] = os.path.join(tmp_in_dir, settings_dict["START_CREATURE"])
    
    # Read the default genesis file, if there is a equivilent line in the 
    # dictionary replace it the new values, otherwise just copy the line
  
    orig_genesis_file = open(os.path.join(workspace_dir, "genesis.default"))
    lines = orig_genesis_file.readlines()
    orig_genesis_file.close()
    out_genesis_file = open(os.path.join(tmp_in_dir, "genesis.avida"), "w")
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
    
  def modifyEventFile(self, cells_dict, organisms_dict, event_file_name, 
    tmp_out_dir = None):

    # Routine to add to the event.cfg file by inject creatures into the population
    # and adding print statements into the correct directory

    event_out_file = open(event_file_name, 'a')
    for cell in cells_dict.keys():
      part1 = "u begin inject_sequence " +  organisms_dict[cells_dict[cell]] + " " 
      part2 = cell + " " + str(int(cell)+1) + " -1 "
      part3 = cells_dict[cell] + "\n"
      event_out_file.write(part1 +  part2 + part3)
    
    # write the .dat files to the correct directory

    event_out_file.write("\nu 0:1:end print_average_data " + 
                         os.path.join(tmp_out_dir, "average.dat") +"\n")
    event_out_file.write("u 0:1:end print_count_data " + 
                         os.path.join(tmp_out_dir, "count.dat") +"\n")
    event_out_file.close()
    
