# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyTwoOrganismView.ui'
#
# Created: Thu Mar 31 04:19:25 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyTwoOrganismView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyTwoOrganismView")


        pyTwoOrganismViewLayout = QVBoxLayout(self,11,6,"pyTwoOrganismViewLayout")

        self.textLabel1 = QLabel(self,"textLabel1")
        self.textLabel1.setAlignment(QLabel.AlignCenter)
        pyTwoOrganismViewLayout.addWidget(self.textLabel1)

        self.languageChange()

        self.resize(QSize(604,650).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyTwoOrganismView"))
        self.textLabel1.setText(self.__tr("Contents of this space are temporarily removed."))


    def __tr(self,s,c = None):
        return qApp.translate("pyTwoOrganismView",s,c)
