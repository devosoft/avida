# -*- coding: utf-8 -*-

import string

class pyReadFreezer:

  def __init__(self, in_file_name = None):
    self.file_name = in_file_name
    self.dictionary = {}
    freezefile = open(self.file_name)
    lines = freezefile.readlines()
    freezefile.close
    for line in lines:
      comment_start = line.find("#")
      if comment_start > -1:
        if comment_start == 0:
          line = ""
        else:
          line = line[:comment_start]
      line = line.strip()
      if len(line) > 0:
        if line[0] == "*":
          section_key = line[1:].upper()
          self.dictionary[section_key] = {}
        else:
          var_name, value = string.split(line)
          self.dictionary[section_key][var_name.upper()] = value
	
  def GetDictionary(self):
    return self.dictionary
