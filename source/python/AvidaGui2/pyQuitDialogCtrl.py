# -*- coding: utf-8 -*-

from qt import *
from pyQuitDialogView import pyQuitDialogView
import shutil, os, os.path

# class to pop up a dialog box when the user quits
# to ask for what to freeze and to
# return the name of a file to save information to be frozen

class pyQuitDialogCtrl (pyQuitDialogView):
  def __init__(self):
    pyQuitDialogView.__init__(self)
    self.connect(self.QuitPushButton, SIGNAL("clicked()"), self.DownQuitSlot)
    
  def DownQuitSlot(self):
    self.QuitPushButton.setDown()
    
  def showDialog(self):
    found_valid_name = False
    dialog_result = 1
    while (found_valid_name == False and dialog_result > 0):
      while (self.exec_loop() and self.result() == 0):
        pass
      dialog_result = self.result()
      tmp_name = str(self.FileNameLineEdit.text())
      print "dialog_result = " + str(dialog_result) + " QuitPushButton.isDown() = " + str(QuitPushButton.isDown())
      if dialog_result == 0:
        return 0
      elif QuitPushButton.isDown():
        return 1
      else:
        return 2
