# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pyTwoPop_StatsView.ui'
#
# Created: Thu Mar 31 04:19:26 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyTwoPop_StatsView(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pyTwoPop_StatsView")


        pyTwoPop_StatsViewLayout = QVBoxLayout(self,11,6,"pyTwoPop_StatsViewLayout")

        self.groupBox3 = QGroupBox(self,"groupBox3")
        self.groupBox3.setSizePolicy(QSizePolicy(3,3,0,0,self.groupBox3.sizePolicy().hasHeightForWidth()))
        groupBox3_font = QFont(self.groupBox3.font())
        groupBox3_font.setPointSize(11)
        self.groupBox3.setFont(groupBox3_font)
        self.groupBox3.setColumnLayout(0,Qt.Vertical)
        self.groupBox3.layout().setSpacing(6)
        self.groupBox3.layout().setMargin(11)
        groupBox3Layout = QGridLayout(self.groupBox3.layout())
        groupBox3Layout.setAlignment(Qt.AlignTop)

        self.textLabel3_3 = QLabel(self.groupBox3,"textLabel3_3")
        textLabel3_3_font = QFont(self.textLabel3_3.font())
        textLabel3_3_font.setPointSize(10)
        textLabel3_3_font.setBold(1)
        textLabel3_3_font.setUnderline(1)
        self.textLabel3_3.setFont(textLabel3_3_font)

        groupBox3Layout.addWidget(self.textLabel3_3,0,0)

        self.textLabel3 = QLabel(self.groupBox3,"textLabel3")
        textLabel3_font = QFont(self.textLabel3.font())
        textLabel3_font.setPointSize(10)
        textLabel3_font.setBold(1)
        textLabel3_font.setUnderline(1)
        self.textLabel3.setFont(textLabel3_font)

        groupBox3Layout.addWidget(self.textLabel3,0,1)

        self.textLabel3_2 = QLabel(self.groupBox3,"textLabel3_2")
        textLabel3_2_font = QFont(self.textLabel3_2.font())
        textLabel3_2_font.setPointSize(10)
        textLabel3_2_font.setBold(1)
        textLabel3_2_font.setUnderline(1)
        self.textLabel3_2.setFont(textLabel3_2_font)

        groupBox3Layout.addWidget(self.textLabel3_2,0,2)

        self.textLabel3_3_2 = QLabel(self.groupBox3,"textLabel3_3_2")
        textLabel3_3_2_font = QFont(self.textLabel3_3_2.font())
        textLabel3_3_2_font.setPointSize(10)
        self.textLabel3_3_2.setFont(textLabel3_3_2_font)

        groupBox3Layout.addWidget(self.textLabel3_3_2,1,0)

        self.textLabel3_4 = QLabel(self.groupBox3,"textLabel3_4")
        textLabel3_4_font = QFont(self.textLabel3_4.font())
        textLabel3_4_font.setPointSize(10)
        self.textLabel3_4.setFont(textLabel3_4_font)

        groupBox3Layout.addWidget(self.textLabel3_4,1,1)

        self.textLabel3_2_2 = QLabel(self.groupBox3,"textLabel3_2_2")
        textLabel3_2_2_font = QFont(self.textLabel3_2_2.font())
        textLabel3_2_2_font.setPointSize(10)
        self.textLabel3_2_2.setFont(textLabel3_2_2_font)

        groupBox3Layout.addWidget(self.textLabel3_2_2,1,2)
        spacer1 = QSpacerItem(270,31,QSizePolicy.Minimum,QSizePolicy.Expanding)
        groupBox3Layout.addMultiCell(spacer1,2,2,0,2)
        pyTwoPop_StatsViewLayout.addWidget(self.groupBox3)

        self.groupBox5 = QGroupBox(self,"groupBox5")
        self.groupBox5.setSizePolicy(QSizePolicy(3,3,0,0,self.groupBox5.sizePolicy().hasHeightForWidth()))
        groupBox5_font = QFont(self.groupBox5.font())
        groupBox5_font.setPointSize(11)
        self.groupBox5.setFont(groupBox5_font)
        self.groupBox5.setColumnLayout(0,Qt.Vertical)
        self.groupBox5.layout().setSpacing(6)
        self.groupBox5.layout().setMargin(11)
        groupBox5Layout = QVBoxLayout(self.groupBox5.layout())
        groupBox5Layout.setAlignment(Qt.AlignTop)

        layout11 = QGridLayout(None,1,1,0,6,"layout11")

        self.textLabel14_3_3_2_2 = QLabel(self.groupBox5,"textLabel14_3_3_2_2")
        self.textLabel14_3_3_2_2.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_2_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_font = QFont(self.textLabel14_3_3_2_2.font())
        textLabel14_3_3_2_2_font.setPointSize(10)
        self.textLabel14_3_3_2_2.setFont(textLabel14_3_3_2_2_font)
        self.textLabel14_3_3_2_2.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignRight)

        layout11.addWidget(self.textLabel14_3_3_2_2,2,0)

        self.textLabel14_3_3_3_3 = QLabel(self.groupBox5,"textLabel14_3_3_3_3")
        self.textLabel14_3_3_3_3.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_3_3.sizePolicy().hasHeightForWidth()))
        textLabel14_3_3_3_3_font = QFont(self.textLabel14_3_3_3_3.font())
        textLabel14_3_3_3_3_font.setPointSize(10)
        self.textLabel14_3_3_3_3.setFont(textLabel14_3_3_3_3_font)

        layout11.addWidget(self.textLabel14_3_3_3_3,1,1)

        self.textLabel14_3_3_3_2 = QLabel(self.groupBox5,"textLabel14_3_3_3_2")
        self.textLabel14_3_3_3_2.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_3_2.sizePolicy().hasHeightForWidth()))
        textLabel14_3_3_3_2_font = QFont(self.textLabel14_3_3_3_2.font())
        textLabel14_3_3_3_2_font.setPointSize(10)
        self.textLabel14_3_3_3_2.setFont(textLabel14_3_3_3_2_font)

        layout11.addWidget(self.textLabel14_3_3_3_2,0,1)

        self.textLabel14_3_3_2 = QLabel(self.groupBox5,"textLabel14_3_3_2")
        self.textLabel14_3_3_2.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_font = QFont(self.textLabel14_3_3_2.font())
        textLabel14_3_3_2_font.setPointSize(10)
        self.textLabel14_3_3_2.setFont(textLabel14_3_3_2_font)
        self.textLabel14_3_3_2.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignRight)

        layout11.addWidget(self.textLabel14_3_3_2,1,0)

        self.textLabel14_3_3_3_3_3 = QLabel(self.groupBox5,"textLabel14_3_3_3_3_3")
        self.textLabel14_3_3_3_3_3.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_3_3_3.sizePolicy().hasHeightForWidth()))
        textLabel14_3_3_3_3_3_font = QFont(self.textLabel14_3_3_3_3_3.font())
        textLabel14_3_3_3_3_3_font.setPointSize(10)
        self.textLabel14_3_3_3_3_3.setFont(textLabel14_3_3_3_3_3_font)

        layout11.addWidget(self.textLabel14_3_3_3_3_3,2,1)

        self.textLabel14_3_3 = QLabel(self.groupBox5,"textLabel14_3_3")
        self.textLabel14_3_3.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_font = QFont(self.textLabel14_3_3.font())
        textLabel14_3_3_font.setPointSize(10)
        self.textLabel14_3_3.setFont(textLabel14_3_3_font)
        self.textLabel14_3_3.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignRight)

        layout11.addWidget(self.textLabel14_3_3,0,0)

        self.textLabel14_3_3_2_2_2 = QLabel(self.groupBox5,"textLabel14_3_3_2_2_2")
        self.textLabel14_3_3_2_2_2.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_2_2_2.sizePolicy().hasHeightForWidth()))
        self.textLabel14_3_3_2_2_2.setMaximumSize(QSize(32767,40))
        textLabel14_3_3_2_2_2_font = QFont(self.textLabel14_3_3_2_2_2.font())
        textLabel14_3_3_2_2_2_font.setPointSize(10)
        self.textLabel14_3_3_2_2_2.setFont(textLabel14_3_3_2_2_2_font)
        self.textLabel14_3_3_2_2_2.setAlignment(QLabel.WordBreak | QLabel.AlignVCenter | QLabel.AlignRight)

        layout11.addWidget(self.textLabel14_3_3_2_2_2,3,0)

        self.textLabel14_3_3_3_3_4 = QLabel(self.groupBox5,"textLabel14_3_3_3_3_4")
        self.textLabel14_3_3_3_3_4.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel14_3_3_3_3_4.sizePolicy().hasHeightForWidth()))
        textLabel14_3_3_3_3_4_font = QFont(self.textLabel14_3_3_3_3_4.font())
        textLabel14_3_3_3_3_4_font.setPointSize(10)
        self.textLabel14_3_3_3_3_4.setFont(textLabel14_3_3_3_3_4_font)

        layout11.addWidget(self.textLabel14_3_3_3_3_4,3,1)
        groupBox5Layout.addLayout(layout11)
        spacer1_2 = QSpacerItem(110,30,QSizePolicy.Minimum,QSizePolicy.MinimumExpanding)
        groupBox5Layout.addItem(spacer1_2)
        pyTwoPop_StatsViewLayout.addWidget(self.groupBox5)

        self.languageChange()

        self.resize(QSize(314,275).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("pyTwoPop_StatsView"))
        self.groupBox3.setTitle(self.__tr("Task Outlook"))
        self.textLabel3_3.setText(self.__tr("Task"))
        self.textLabel3.setText(self.__tr("[left population]"))
        self.textLabel3_2.setText(self.__tr("[right population]"))
        self.textLabel3_3_2.setText(self.__tr("blah"))
        self.textLabel3_4.setText(self.__tr("blah blah"))
        self.textLabel3_2_2.setText(self.__tr("blah blah"))
        self.groupBox5.setTitle(self.__tr("Statistics"))
        self.textLabel14_3_3_2_2.setText(self.__tr("Num. Orgs:"))
        self.textLabel14_3_3_3_3.setText(self.__tr("0000"))
        self.textLabel14_3_3_3_2.setText(self.__tr("0000"))
        self.textLabel14_3_3_2.setText(self.__tr("Dom. Merit:"))
        self.textLabel14_3_3_3_3_3.setText(self.__tr("0000"))
        self.textLabel14_3_3.setText(self.__tr("Avg. Merit:"))
        self.textLabel14_3_3_2_2_2.setText(self.__tr("Avg. Gest:"))
        self.textLabel14_3_3_3_3_4.setText(self.__tr("0000"))


    def __tr(self,s,c = None):
        return qApp.translate("pyTwoPop_StatsView",s,c)
