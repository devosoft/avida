# -*- coding: utf-8 -*-

import os
from qt import *
from pyFreezerView import pyFreezerView
from pyReadFreezer import pyReadFreezer


class pyFreezerCtrl(pyFreezerView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyFreezerView.__init__(self,parent,name,fl)
    self.m_list_view.setSelectionMode(QListView.Extended)
    # self.connect(self.m_list_view, 
    #   SIGNAL("doubleClicked(QListViewItem, QPoint, int)"),
    #   self.clicked_item)
    self.connect(self.m_list_view, 
      SIGNAL("clicked(QListViewItem*, const QPoint &, int )"),
      self.clicked_item)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    empty_item = self.m_list_view.firstChild()
    full_item = empty_item.nextSibling()
    organism_item = full_item.nextSibling()
    if os.path.exists("freezer") == False:
      os.mkdir("freezer")
    freezer_dir =  os.listdir("freezer")
    for file in freezer_dir:
      if file.endswith(".empty"):
        dish_name = file[:-6]
        tmp_item = QListViewItem(empty_item)
        tmp_item.setText(0,dish_name)
      if file.endswith(".full"):
        dish_name = file[:-5]
        tmp_item = QListViewItem(full_item)
        tmp_item.setText(0,dish_name)
      if file.endswith(".organism"):
        organism_name = file[:-9]
        tmp_item = QListViewItem(organism_item)
        tmp_item.setText(0,organism_name)

  # if freezer item is clicked read file/directory assocatied with item

  def clicked_item(self, item):
   
    # check that the item is not at the top level 

    if item.depth() > 0:
      top_level = item
      while top_level.parent():
        top_level = top_level.parent()

      # Rebuild the file name

      if str(top_level.text(0)).startswith(" Empty Petri"):
        file_name = str(item.text(0)) + ".empty"
      elif str(top_level.text(0)).startswith(" Full Petri"):
        file_name = str(item.text(0)) + ".full/petri_dish"
      elif str(top_level.text(0)).startswith(" Organism"):
        file_name = str(item.text(0)) + ".organism"
      file_name = "freezer/" + file_name
      thawed_item = pyReadFreezer(file_name)
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doDefrostDishSig"),
        (thawed_item,))
