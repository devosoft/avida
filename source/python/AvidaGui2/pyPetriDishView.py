# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyPetriDishView.ui'
#
# Created: Thu Mar 31 04:19:24 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyPetriDishView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyPetriDishView")


        pyPetriDishViewLayout = QVBoxLayout(self,0,0,"pyPetriDishViewLayout")
        pyPetriDishViewLayout.setResizeMode(QLayout.Minimum)

        self.textLabel4 = QLabel(self,"textLabel4")
        self.textLabel4.setMinimumSize(QSize(200,200))
        self.textLabel4.setBackgroundMode(QLabel.PaletteShadow)
        textLabel4_font = QFont(self.textLabel4.font())
        textLabel4_font.setPointSize(24)
        self.textLabel4.setFont(textLabel4_font)
        self.textLabel4.setScaledContents(0)
        self.textLabel4.setAlignment(QLabel.AlignCenter)
        pyPetriDishViewLayout.addWidget(self.textLabel4)

        self.languageChange()

        self.resize(QSize(202,202).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyPetriDishView"))
        self.textLabel4.setText(self.__tr("mock petri dish"))


    def __tr(self,s,c = None):
        return qApp.translate("pyPetriDishView",s,c)
