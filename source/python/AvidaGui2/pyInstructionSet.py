# -*- coding: utf-8 -*-

import string

class pyInstructionSet:

  def __init__(self, inst_file_name = None):
  
    self.cmd_letter_dict = {}
    self.letter_cmd_dict = {}
    letters = ['z','y','x','w','v','u','t','s','r','q','p','o','n','m','l',
               'k','j','i','h','g','f','e','d','c','b','a']
    inst_file = open(inst_file_name)
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

  def OrgFile2LetterString(self, org_file_name = None):
  
    print "****** " + org_file_name + " **********"
    org_file = open(org_file_name)
    lines = org_file.readlines()
    org_file.close
    out_string = ""
    for line in lines:
      comment_start = line.find("#")
      if comment_start > -1:
        if comment_start == 0:
          line = ""
        else:
          line = line[:comment_start]
      line = line.strip()
      if len(line) > 0:
        cmd_name = line.upper()
        letter =  self.cmd_letter_dict[cmd_name]
        out_string = out_string + letter
    return out_string