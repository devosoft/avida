# -*- coding: utf-8 -*-

import string
from qt import *
import os

class pyWriteToFreezer:

  def __init__(self, in_dict = None, out_file_name = None):

    is_org = False
    is_dish = False
    is_empty_dish = False

    if out_file_name.endswith("petri_dish"):
      out_file_name = (os.path.split(out_file_name)[0])

    if out_file_name.endswith(".organism"):
      print "that was an org"
      is_org = True

    elif out_file_name.endswith(".full"):
      print "that was a full dish man"
      is_dish = True
      self.simplifyPopulation(in_dict)
      out_file_name = (out_file_name + '/petri_dish')

    elif out_file_name.endswith(".empty"):
      print "that was an empty dish man"
      is_dish = True
      is_empty_dish = True

    else:
      print "that was junk man"

    out_freezer_file = open((out_file_name), "w")

  
    if is_org == True:
      print "writing org"
      genome = str(in_dict[1])
      out_freezer_file.write(genome + "\n")
    elif is_dish == True:

      for section in in_dict.keys():
    
        # Always print out the petri dishes settings and if it is a full dish
        # print out the cell and organism information
      
        if ((section == "SETTINGS") or (is_empty_dish == False)):
          out_freezer_file.write("*" + section + "\n")
          info_dict = in_dict[section]
          for info_key in info_dict.keys():
            out_freezer_file.write(str(info_key) + " " + str(info_dict[info_key]) + "\n")
    else: 
      print "something is wrong, that was not an org or a dish"

    out_freezer_file.close()
    
  def simplifyPopulation(self, in_dict = None):
    if in_dict.has_key("POPULATION"):
      pop_dict = in_dict["POPULATION"]
      del in_dict["POPULATION"]
      
      # Create a unique number ID for each genome.  Make a dictionary with 
      # that ID as a key and the genome string as the value.  Make a second
      # (temporary) directory that has reverse key/values.  Create a third
      # dictionary that has the cell location as the key and the genome ID
      # as the value
      
      organism_dict = {}
      in_dict["ORGANISMS"] = {}
      in_dict["CELLS"] = {}
      new_genome_num = 0
      for cell in pop_dict.keys():
        genome = pop_dict[cell]
        if (organism_dict.has_key(genome) == False):
          new_genome_num = new_genome_num + 1
          curr_genome_num = new_genome_num
          in_dict["ORGANISMS"][new_genome_num] = genome
          organism_dict[genome] = new_genome_num
        else:
          curr_genome_num = organism_dict[genome]
        in_dict["CELLS"][cell] = curr_genome_num
      
      
