# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyTwoAnalyzeView.ui'
#
# Created: Thu Mar 31 04:19:24 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyOneAna_GraphCtrl import pyOneAna_GraphCtrl
from pyOneAna_PetriDishCtrl import pyOneAna_PetriDishCtrl
from pyOneAna_StatsCtrl import pyOneAna_StatsCtrl
from pyOneAna_TimelineCtrl import pyOneAna_TimelineCtrl


class pyTwoAnalyzeView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyTwoAnalyzeView")


        pyTwoAnalyzeViewLayout = QVBoxLayout(self,0,6,"pyTwoAnalyzeViewLayout")

        self.splitter6 = QSplitter(self,"splitter6")
        self.splitter6.setOrientation(QSplitter.Vertical)

        self.splitter5 = QSplitter(self.splitter6,"splitter5")
        self.splitter5.setOrientation(QSplitter.Horizontal)

        LayoutWidget = QWidget(self.splitter5,"layout73")
        layout73 = QHBoxLayout(LayoutWidget,11,6,"layout73")

        self.pyOneAna_StatsCtrl1 = pyOneAna_StatsCtrl(LayoutWidget,"pyOneAna_StatsCtrl1")
        layout73.addWidget(self.pyOneAna_StatsCtrl1)

        self.pyOneAna_PetriDishCtrl1_2 = pyOneAna_PetriDishCtrl(LayoutWidget,"pyOneAna_PetriDishCtrl1_2")
        self.pyOneAna_PetriDishCtrl1_2.setSizePolicy(QSizePolicy(3,3,0,0,self.pyOneAna_PetriDishCtrl1_2.sizePolicy().hasHeightForWidth()))
        layout73.addWidget(self.pyOneAna_PetriDishCtrl1_2)

        LayoutWidget_2 = QWidget(self.splitter5,"layout72")
        layout72 = QHBoxLayout(LayoutWidget_2,11,6,"layout72")

        self.pyOneAna_StatsCtrl1_2 = pyOneAna_StatsCtrl(LayoutWidget_2,"pyOneAna_StatsCtrl1_2")
        layout72.addWidget(self.pyOneAna_StatsCtrl1_2)

        self.pyOneAna_PetriDishCtrl1 = pyOneAna_PetriDishCtrl(LayoutWidget_2,"pyOneAna_PetriDishCtrl1")
        self.pyOneAna_PetriDishCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyOneAna_PetriDishCtrl1.sizePolicy().hasHeightForWidth()))
        layout72.addWidget(self.pyOneAna_PetriDishCtrl1)

        LayoutWidget_3 = QWidget(self.splitter6,"layout54")
        layout54 = QVBoxLayout(LayoutWidget_3,11,6,"layout54")

        self.pyOneAna_GraphCtrl1 = pyOneAna_GraphCtrl(LayoutWidget_3,"pyOneAna_GraphCtrl1")
        self.pyOneAna_GraphCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyOneAna_GraphCtrl1.sizePolicy().hasHeightForWidth()))
        layout54.addWidget(self.pyOneAna_GraphCtrl1)

        self.pyOneAna_TimelineCtrl1 = pyOneAna_TimelineCtrl(LayoutWidget_3,"pyOneAna_TimelineCtrl1")
        layout54.addWidget(self.pyOneAna_TimelineCtrl1)
        pyTwoAnalyzeViewLayout.addWidget(self.splitter6)

        self.languageChange()

        self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyTwoAnalyzeView"))


    def __tr(self,s,c = None):
        return qApp.translate("pyTwoAnalyzeView",s,c)
