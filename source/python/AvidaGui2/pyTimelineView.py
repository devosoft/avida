# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyTimelineView.ui'
#
# Created: Thu Mar 31 04:19:24 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyTimelineView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyTimelineView")

        self.setMaximumSize(QSize(32767,100))

        pyTimelineViewLayout = QHBoxLayout(self,0,0,"pyTimelineViewLayout")
        pyTimelineViewLayout.setResizeMode(QLayout.Minimum)

        self.textLabel4 = QLabel(self,"textLabel4")
        self.textLabel4.setSizePolicy(QSizePolicy(5,0,0,0,self.textLabel4.sizePolicy().hasHeightForWidth()))
        self.textLabel4.setMinimumSize(QSize(0,50))
        self.textLabel4.setBackgroundMode(QLabel.PaletteShadow)
        textLabel4_font = QFont(self.textLabel4.font())
        textLabel4_font.setPointSize(24)
        self.textLabel4.setFont(textLabel4_font)
        self.textLabel4.setScaledContents(0)
        self.textLabel4.setAlignment(QLabel.AlignCenter)
        pyTimelineViewLayout.addWidget(self.textLabel4)

        self.languageChange()

        self.resize(QSize(214,51).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyTimelineView"))
        self.textLabel4.setText(self.__tr("mock timeline"))


    def __tr(self,s,c = None):
        return qApp.translate("pyTimelineView",s,c)
