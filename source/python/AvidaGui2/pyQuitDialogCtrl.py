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
    self.FreezeQuitFlag = 0
    self.QuitFlag = 1
    self.CancelFlag = 2
    
  def DownQuitSlot(self):
    self.QuitPushButton.setDown(True)
    
  def showDialog(self):
    dialog_result = 1
    while (dialog_result > 0):
      while (self.exec_loop() and self.result() == 0):
        pass
      dialog_result = self.result()
      if dialog_result == 0:
        return self.CancelFlag
      elif self.QuitPushButton.isDown():
        return self.QuitFlag
      else:
        return self.FreezeQuitFlag
