# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyOnePopulationView.ui'
#
# Created: Fri Dec 3 10:26:52 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyOnePop_PetriDishCtrl import pyOnePop_PetriDishCtrl
from pyOnePop_TimelineCtrl import pyOnePop_TimelineCtrl
from pyOnePop_GraphCtrl import pyOnePop_GraphCtrl
from pyOnePop_StatsCtrl import pyOnePop_StatsCtrl


class pyOnePopulationView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyOnePopulationView")


    pyOnePopulationViewLayout = QHBoxLayout(self,11,6,"pyOnePopulationViewLayout")

    layout16 = QVBoxLayout(None,0,6,"layout16")

    self.m_one_pop_petri_dish_ctrl = pyOnePop_PetriDishCtrl(self,"m_one_pop_petri_dish_ctrl")
    self.m_one_pop_petri_dish_ctrl.setSizePolicy(QSizePolicy(3,7,0,0,self.m_one_pop_petri_dish_ctrl.sizePolicy().hasHeightForWidth()))
    layout16.addWidget(self.m_one_pop_petri_dish_ctrl)

    self.m_one_pop_timeline_ctrl = pyOnePop_TimelineCtrl(self,"m_one_pop_timeline_ctrl")
    self.m_one_pop_timeline_ctrl.setSizePolicy(QSizePolicy(1,0,0,0,self.m_one_pop_timeline_ctrl.sizePolicy().hasHeightForWidth()))
    layout16.addWidget(self.m_one_pop_timeline_ctrl)
    pyOnePopulationViewLayout.addLayout(layout16)

    layout44 = QVBoxLayout(None,0,6,"layout44")

    self.m_one_pop_graph_ctrl = pyOnePop_GraphCtrl(self,"m_one_pop_graph_ctrl")
    self.m_one_pop_graph_ctrl.setSizePolicy(QSizePolicy(3,3,0,0,self.m_one_pop_graph_ctrl.sizePolicy().hasHeightForWidth()))
    layout44.addWidget(self.m_one_pop_graph_ctrl)

    self.m_one_pop_stats_ctrl = pyOnePop_StatsCtrl(self,"m_one_pop_stats_ctrl")
    self.m_one_pop_stats_ctrl.setSizePolicy(QSizePolicy(5,5,0,0,self.m_one_pop_stats_ctrl.sizePolicy().hasHeightForWidth()))
    layout44.addWidget(self.m_one_pop_stats_ctrl)
    pyOnePopulationViewLayout.addLayout(layout44)

    self.languageChange()

    self.resize(QSize(690,607).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyOnePopulationView"))


  def __tr(self,s,c = None):
    return qApp.translate("pyOnePopulationView",s,c)
