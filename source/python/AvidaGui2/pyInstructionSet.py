# -*- coding: utf-8 -*-

import string

class pyInstructionSet:

  def __init__(self, in_file = None):
  
    self.cmd_letter_dict = {}
    self.letter_cmd_dict = {}
    letters = ['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l',
               'k','j','i','h','g','f','e','d','c','b','a']
    inst_file = open(in_file)
    lines = inst_file.readlines()
    inst_file.close
    for line in lines:
      comment_start = line.find("#")
      if comment_start > -1:
        if comment_start == 0:
          line = ""
        else:
          line = line[:comment_start]
      line = line.strip()
      if len(line) > 0:
        cmd_name, junk = string.split(line)
        cmd_name = cmd_name.upper()
        letter =  letters.pop()
        self.cmd_letter_dict[cmd_name] = letter
        self.letter_cmd_dict[letter] = cmd_name
