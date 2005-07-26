# -*- coding: utf-8 -*-

from qt import *
from pyFreezeOrganismView import pyFreezeOrganismView
import shutil, os, os.path

# class to pop up a dialog box to ask for what to freeze and to
# return the name of a file to save information to be frozen

class pyFreezeOrganismCtrl (pyFreezeOrganismView):
  def __init__(self):
    pyFreezeOrganismView.__init__(self)
    
  def showDialog(self, freezer_dir = None):
    found_valid_name = False
    dialog_result = 1
    while (found_valid_name == False and dialog_result > 0):
      while (self.exec_loop() and self.result() == 0):
        pass
      dialog_result = self.result()
      tmp_name = str(self.FileNameLineEdit.text())

      if dialog_result == 0:
        return ''
      elif (tmp_name == ''):
        found_valid_name = False
        self.MainMessageTextLabel.setText("Enter a Non-Blank Name of Organism to Freeze")
      else:
          if (tmp_name.endswith(".organism") == False):
            tmp_name = tmp_name + ".organism"
          tmp_name = os.path.join(freezer_dir, tmp_name)
          if os.path.exists(tmp_name):
            found_valid_name = False
            self.MainMessageTextLabel.setText("Organism Name Exists, Please Enter a Different Name")
          else:
            found_valid_name = True
            return tmp_name
      
