# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyTwoPopulationView.ui'
#
# Created: Fri Dec 3 10:26:53 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyTwoPop_PetriDishesCtrl import pyTwoPop_PetriDishesCtrl
from pyOnePop_TimelineCtrl import pyOnePop_TimelineCtrl
from pyTwoPop_GraphCtrl import pyTwoPop_GraphCtrl
from pyTwoPop_StatsCtrl import pyTwoPop_StatsCtrl


class pyTwoPopulationView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyTwoPopulationView")


    pyTwoPopulationViewLayout = QHBoxLayout(self,11,6,"pyTwoPopulationViewLayout")

    layout40 = QVBoxLayout(None,0,6,"layout40")

    self.pyTwoPop_PetriDishesCtrl1 = pyTwoPop_PetriDishesCtrl(self,"pyTwoPop_PetriDishesCtrl1")
    self.pyTwoPop_PetriDishesCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyTwoPop_PetriDishesCtrl1.sizePolicy().hasHeightForWidth()))
    layout40.addWidget(self.pyTwoPop_PetriDishesCtrl1)

    self.pyOnePop_TimelineCtrl1 = pyOnePop_TimelineCtrl(self,"pyOnePop_TimelineCtrl1")
    self.pyOnePop_TimelineCtrl1.setSizePolicy(QSizePolicy(3,0,0,0,self.pyOnePop_TimelineCtrl1.sizePolicy().hasHeightForWidth()))
    layout40.addWidget(self.pyOnePop_TimelineCtrl1)
    pyTwoPopulationViewLayout.addLayout(layout40)

    layout2 = QVBoxLayout(None,0,6,"layout2")

    self.pyTwoPop_GraphCtrl2 = pyTwoPop_GraphCtrl(self,"pyTwoPop_GraphCtrl2")
    self.pyTwoPop_GraphCtrl2.setSizePolicy(QSizePolicy(3,3,0,0,self.pyTwoPop_GraphCtrl2.sizePolicy().hasHeightForWidth()))
    layout2.addWidget(self.pyTwoPop_GraphCtrl2)

    self.pyTwoPop_StatsCtrl1 = pyTwoPop_StatsCtrl(self,"pyTwoPop_StatsCtrl1")
    self.pyTwoPop_StatsCtrl1.setSizePolicy(QSizePolicy(0,0,0,0,self.pyTwoPop_StatsCtrl1.sizePolicy().hasHeightForWidth()))
    layout2.addWidget(self.pyTwoPop_StatsCtrl1)
    pyTwoPopulationViewLayout.addLayout(layout2)

    self.languageChange()

    self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyTwoPopulationView"))


  def __tr(self,s,c = None):
    return qApp.translate("pyTwoPopulationView",s,c)
