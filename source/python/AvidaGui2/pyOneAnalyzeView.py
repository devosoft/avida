# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyOneAnalyzeView.ui'
#
# Created: Fri Dec 3 10:26:51 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyOneAna_GraphCtrl import pyOneAna_GraphCtrl
from pyOneAna_PetriDishCtrl import pyOneAna_PetriDishCtrl
from pyOneAna_StatsCtrl import pyOneAna_StatsCtrl
from pyOneAna_TimelineCtrl import pyOneAna_TimelineCtrl


class pyOneAnalyzeView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyOneAnalyzeView")


    pyOneAnalyzeViewLayout = QVBoxLayout(self,0,6,"pyOneAnalyzeViewLayout")

    self.splitter3 = QSplitter(self,"splitter3")
    self.splitter3.setOrientation(QSplitter.Vertical)

    LayoutWidget = QWidget(self.splitter3,"layout55")
    layout55 = QHBoxLayout(LayoutWidget,11,6,"layout55")

    self.pyOneAna_StatsCtrl1 = pyOneAna_StatsCtrl(LayoutWidget,"pyOneAna_StatsCtrl1")
    layout55.addWidget(self.pyOneAna_StatsCtrl1)

    self.pyOneAna_PetriDishCtrl1 = pyOneAna_PetriDishCtrl(LayoutWidget,"pyOneAna_PetriDishCtrl1")
    self.pyOneAna_PetriDishCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyOneAna_PetriDishCtrl1.sizePolicy().hasHeightForWidth()))
    layout55.addWidget(self.pyOneAna_PetriDishCtrl1)

    LayoutWidget_2 = QWidget(self.splitter3,"layout54")
    layout54 = QVBoxLayout(LayoutWidget_2,11,6,"layout54")

    self.pyOneAna_GraphCtrl1 = pyOneAna_GraphCtrl(LayoutWidget_2,"pyOneAna_GraphCtrl1")
    self.pyOneAna_GraphCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyOneAna_GraphCtrl1.sizePolicy().hasHeightForWidth()))
    layout54.addWidget(self.pyOneAna_GraphCtrl1)

    self.pyOneAna_TimelineCtrl1 = pyOneAna_TimelineCtrl(LayoutWidget_2,"pyOneAna_TimelineCtrl1")
    layout54.addWidget(self.pyOneAna_TimelineCtrl1)
    pyOneAnalyzeViewLayout.addWidget(self.splitter3)

    self.languageChange()

    self.resize(QSize(465,495).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyOneAnalyzeView"))


  def __tr(self,s,c = None):
    return qApp.translate("pyOneAnalyzeView",s,c)
