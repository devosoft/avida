# -*- coding: utf-8 -*-

import os
from qt import *
from pyFreezerView import pyFreezerView
from pyReadFreezer import pyReadFreezer
from pyWriteToFreezer import pyWriteToFreezer
from pyFreezeOrganismCtrl import pyFreezeOrganismCtrl
import os.path

class pyFreezerCtrl(pyFreezerView):

  def __init__(self,parent = None,name = None,fl = 0):
    pyFreezerView.__init__(self,parent,name,fl)
    self.m_list_view.setSelectionMode(QListView.Extended)
    self.connect(self.m_list_view, 
      SIGNAL("doubleClicked(QListViewItem*, const QPoint &, int)"),
      self.clicked_itemSlot)
    self.connect(self.m_list_view, 
      SIGNAL("pressed(QListViewItem*, const QPoint &, int )"),
      self.pressed_itemSlot)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.connect(self.m_session_mdl.m_session_mdtr,
      PYSIGNAL("doRefreshFreezerInventorySig"),
      self.createFreezerIndexSlot)
    self.createFreezerIndexSlot()


  def createFreezerIndexSlot(self):
    empty_item = self.m_list_view.firstChild()
    while empty_item.firstChild():
      tmp_child = empty_item.firstChild()
      empty_item.takeItem(tmp_child)
      del (tmp_child)
    full_item = empty_item.nextSibling()
    while full_item.firstChild():
      tmp_child = full_item.firstChild()
      full_item.takeItem(tmp_child)
      del (tmp_child)
    organism_item = full_item.nextSibling()
    while organism_item.firstChild():
      tmp_child = organism_item.firstChild()
      organism_item.takeItem(tmp_child)
      del (tmp_child)
    if os.path.exists(self.m_session_mdl.m_current_freezer) == False:
      os.mkdir(self.m_session_mdl.m_current_freezer)
    freezer_dir =  os.listdir(self.m_session_mdl.m_current_freezer)
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

  # if mouse is pressed on list item prepare its info to be dragged        

  def pressed_itemSlot(self, item):

    if item != None and item.depth() > 0:
      top_level = item
      while top_level.parent():
        top_level = top_level.parent()

      # Rebuild the file name

      if str(top_level.text(0)).startswith(" Empty Petri"):
        file_name = str(item.text(0)) + ".empty"
      elif str(top_level.text(0)).startswith(" Full Petri"):
        file_name = str(item.text(0)) + ".full"
      elif str(top_level.text(0)).startswith(" Organism"):
        file_name = str(item.text(0)) + ".organism"
      file_name = os.path.join(self.m_session_mdl.m_current_freezer, file_name)

      dragHolder = self.itemDrag( file_name, self )
#     maybe play with iconView
#      dragHolder.dragEnabled()
      dragHolder.dragCopy()



  # if freezer item is clicked read file/directory assocatied with item

  def clicked_itemSlot(self, item):
   
    # check that the item is not at the top level 
    
    if item != None and item.depth() > 0:
      top_level = item
      while top_level.parent():
        top_level = top_level.parent()

      # Rebuild the file name

      if str(top_level.text(0)).startswith(" Empty Petri"):
        file_name = str(item.text(0)) + ".empty"
      elif str(top_level.text(0)).startswith(" Full Petri"):
        file_name = str(item.text(0)) + ".full"
        file_name = os.path.join(file_name, "petri_dish")
      elif str(top_level.text(0)).startswith(" Organism"):
        file_name = str(item.text(0)) + ".organism"
      file_name = os.path.join(self.m_session_mdl.m_current_freezer, file_name)
      thawed_item = pyReadFreezer(file_name)
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("doDefrostDishSig"),
        (item.text(0), thawed_item,))
      self.m_session_mdl.m_session_mdtr.emit(PYSIGNAL("freezerItemDoubleClicked"),
        (file_name,))


  class itemDrag(QTextDrag):
    def __init__(self, item_name, parent=None, name=None):
        QStoredDrag.__init__(self, 'item name (QString)', parent, name)
        self.setText(item_name)

  def dropEvent( self, e):
    freezer_item_name = QString()
    if e.source() is self:
      return
    if ( QTextDrag.decode( e, freezer_item_name ) ) : #freezer_item_name is a string...the file name 
      if freezer_item_name[:9] == 'organism.':
        freezer_item_name = freezer_item_name[9:] 
        self.FreezeOrganismSlot(freezer_item_name)
      else:
        print "that was not an organism"      
    
  def FreezeOrganismSlot(self, freezer_item_name, 
      send_reset_signal = False, send_quit_signal = False):
    tmp_dict = {1:freezer_item_name}
    m_pop_up_organism_file_name = pyFreezeOrganismCtrl()
    file_name = m_pop_up_organism_file_name.showDialog(self.m_session_mdl.m_current_freezer)

    file_name_len = len(file_name.rstrip())
    if (file_name_len > 0):
      freezer_file = pyWriteToFreezer(tmp_dict, file_name)
    
    self.m_session_mdl.m_session_mdtr.emit(
      PYSIGNAL("doRefreshFreezerInventorySig"), ())
