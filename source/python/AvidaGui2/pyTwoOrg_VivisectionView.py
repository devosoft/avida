# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyTwoOrg_VivisectionView.ui'
#
# Created: Thu Mar 31 04:19:25 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyVivisectionCtrl import pyVivisectionCtrl
from pyAnalyzeControlsCtrl import pyAnalyzeControlsCtrl


class pyTwoOrg_VivisectionView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyTwoOrg_VivisectionView")


        pyTwoOrg_VivisectionViewLayout = QVBoxLayout(self,11,6,"pyTwoOrg_VivisectionViewLayout")

        self.pyVivisectionCtrl1 = pyVivisectionCtrl(self,"pyVivisectionCtrl1")
        self.pyVivisectionCtrl1.setSizePolicy(QSizePolicy(5,3,0,5,self.pyVivisectionCtrl1.sizePolicy().hasHeightForWidth()))
        pyTwoOrg_VivisectionViewLayout.addWidget(self.pyVivisectionCtrl1)

        self.textLabel5 = QLabel(self,"textLabel5")
        textLabel5_font = QFont(self.textLabel5.font())
        textLabel5_font.setPointSize(11)
        self.textLabel5.setFont(textLabel5_font)
        pyTwoOrg_VivisectionViewLayout.addWidget(self.textLabel5)

        layout63 = QHBoxLayout(None,0,6,"layout63")
        spacer35_2 = QSpacerItem(91,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout63.addItem(spacer35_2)

        self.pyAnalyzeControlsCtrl1 = pyAnalyzeControlsCtrl(self,"pyAnalyzeControlsCtrl1")
        self.pyAnalyzeControlsCtrl1.setSizePolicy(QSizePolicy(3,5,0,0,self.pyAnalyzeControlsCtrl1.sizePolicy().hasHeightForWidth()))
        layout63.addWidget(self.pyAnalyzeControlsCtrl1)
        spacer35 = QSpacerItem(91,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        layout63.addItem(spacer35)
        pyTwoOrg_VivisectionViewLayout.addLayout(layout63)

        self.languageChange()

        self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyTwoOrg_VivisectionView"))
        self.textLabel5.setText(self.__tr("IO: will take a number from the input machine and put it into the C register."))


    def __tr(self,s,c = None):
        return qApp.translate("pyTwoOrg_VivisectionView",s,c)
