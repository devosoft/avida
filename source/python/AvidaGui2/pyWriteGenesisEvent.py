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
      if settings_dict.has_key("START_CREATURE0"):
        world_x = settings_dict["WORLD-X"]
        world_y = settings_dict["WORLD-Y"]

        # Count all ancestors with the name of the form START_CREATUREx

        num_ancestors = 0
        while(settings_dict.has_key("START_CREATURE" + str(num_ancestors))):
          num_ancestors = num_ancestors + 1

        # Process all the ancestors

        for i in range(num_ancestors):
          start_creature = settings_dict["START_CREATURE" + str(i)]

          self.start_cell_location = self.find_location(world_x, world_y, 
             num_ancestors, i)
          cells_dict[str(self.start_cell_location)] = str(i)

          # Read the genome from the organism file 

          org_file = open(os.path.join(freeze_dir, start_creature+".organism"))
          org_string = org_file.readline()
          org_string = org_string.rstrip()
          org_string = org_string.lstrip()
          org_file.close
          organisms_dict[str(i)] = org_string

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

    # Routine to add to the event.cfg file by inject creatures into the
    # population and adding print statements into the correct directory

    event_out_file = open(event_file_name, 'a')
    for cell in cells_dict.keys():
      part1 = "u begin inject_sequence " +  organisms_dict[cells_dict[cell]]
      part2 = " " + cell + " " + str(int(cell)+1) + " -1 "
      part3 = cells_dict[cell] + "\n"
      event_out_file.write(part1 +  part2 + part3)
    
    # write the .dat files to the correct directory

    event_out_file.write("\nu 0:1:end print_average_data " + 
                         os.path.join(tmp_out_dir, "average.dat") +"\n")
    event_out_file.write("u 0:1:end print_count_data " + 
                         os.path.join(tmp_out_dir, "count.dat") +"\n")
    event_out_file.close()
    
  def find_location(self, world_x, world_y, num_ancestors=1, org_num=0):

    # Routine to evenly place a given ancestor into the petri dish

    # If there are more than 9 creatures place them evenly in the population
    # array (ignoring the edges)

    if (num_ancestors > 9):
      return int(float(world_x * world_y) * (float(org_num + 1)/float(num_ancestors + 1))) 

    spots = {};
    if (num_ancestors == 1):
      spots = [0.5,0.5]
    elif (num_ancestors == 2):
      spots = [0.5,0.33, 0.5,0.67]
    elif (num_ancestors == 3):
      spots = [0.25,0.25, 0.5,0.5, 0.75,0.75]
    elif (num_ancestors == 4):
      spots = [0.33,0.33, 0.33,0.67, 0.67,0.33, 0.67,0.67]
    elif (num_ancestors == 5):
      spots = [0.25,0.25, 0.75,0.25, 0.50,0.50, 0.25,0.75, 0.75,0.75]
    elif (num_ancestors == 6):
      spots = [0.25,0.25, 0.75,0.25, 
               0.25,0.50, 0.75,0.50,
               0.25,0.75, 0.75,0.75]
    elif (num_ancestors == 7):
      spots = [0.25,0.25, 0.75,0.25, 
               0.25,0.50, 0.50,0.50, 0.75,0.50,
               0.25,0.75, 0.75,0.75]
    elif (num_ancestors == 8):
      spots = [0.25,0.25, 0.50,0.375, 0.75,0.25, 
               0.25,0.50, 0.75,0.50,
               0.25,0.75, 0.50,0.625, 0.75,0.75]
    elif (num_ancestors == 9):
      spots = [0.25,0.25, 0.50,0.25, 0.75,0.25, 
               0.25,0.50, 0.50,0.50, 0.75,0.50,
               0.25,0.75, 0.50,0.75, 0.75,0.75]

    x = spots[org_num * 2]
    y = spots[(org_num * 2) + 1]
    
    return int((round(world_y * y) * world_x) + round(world_x * x))
