# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyOnePop_StatsView.ui'
#
# Created: Fri Dec 3 10:26:52 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyOnePop_StatsView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyOnePop_StatsView")

    self.setSizePolicy(QSizePolicy(3,3,0,0,self.sizePolicy().hasHeightForWidth()))

    pyOnePop_StatsViewLayout = QHBoxLayout(self,0,6,"pyOnePop_StatsViewLayout")

    layout13 = QVBoxLayout(None,0,6,"layout13")

    self.groupBox3 = QGroupBox(self,"groupBox3")
    self.groupBox3.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox3.sizePolicy().hasHeightForWidth()))
    groupBox3_font = QFont(self.groupBox3.font())
    groupBox3_font.setPointSize(11)
    self.groupBox3.setFont(groupBox3_font)
    self.groupBox3.setColumnLayout(0,Qt.Vertical)
    self.groupBox3.layout().setSpacing(6)
    self.groupBox3.layout().setMargin(11)
    groupBox3Layout = QGridLayout(self.groupBox3.layout())
    groupBox3Layout.setAlignment(Qt.AlignTop)

    self.textLabel14 = QLabel(self.groupBox3,"textLabel14")
    textLabel14_font = QFont(self.textLabel14.font())
    textLabel14_font.setPointSize(10)
    textLabel14_font.setBold(1)
    textLabel14_font.setUnderline(1)
    self.textLabel14.setFont(textLabel14_font)

    groupBox3Layout.addWidget(self.textLabel14,0,0)

    self.textLabel14_2 = QLabel(self.groupBox3,"textLabel14_2")
    textLabel14_2_font = QFont(self.textLabel14_2.font())
    textLabel14_2_font.setPointSize(10)
    textLabel14_2_font.setBold(1)
    textLabel14_2_font.setUnderline(1)
    self.textLabel14_2.setFont(textLabel14_2_font)

    groupBox3Layout.addWidget(self.textLabel14_2,0,1)

    self.textLabel14_3 = QLabel(self.groupBox3,"textLabel14_3")
    self.textLabel14_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3.sizePolicy().hasHeightForWidth()))
    textLabel14_3_font = QFont(self.textLabel14_3.font())
    textLabel14_3_font.setPointSize(10)
    self.textLabel14_3.setFont(textLabel14_3_font)

    groupBox3Layout.addWidget(self.textLabel14_3,1,0)

    self.textLabel14_3_2 = QLabel(self.groupBox3,"textLabel14_3_2")
    self.textLabel14_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_2_font = QFont(self.textLabel14_3_2.font())
    textLabel14_3_2_font.setPointSize(10)
    self.textLabel14_3_2.setFont(textLabel14_3_2_font)

    groupBox3Layout.addWidget(self.textLabel14_3_2,1,1)
    spacer1 = QSpacerItem(10,10,QSizePolicy.Minimum,QSizePolicy.Minimum)
    groupBox3Layout.addMultiCell(spacer1,2,2,0,1)
    layout13.addWidget(self.groupBox3)

    self.groupBox5 = QGroupBox(self,"groupBox5")
    self.groupBox5.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox5.sizePolicy().hasHeightForWidth()))
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
    self.textLabel14_3_3_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_2.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_2_font = QFont(self.textLabel14_3_3_2_2.font())
    textLabel14_3_3_2_2_font.setPointSize(10)
    self.textLabel14_3_3_2_2.setFont(textLabel14_3_3_2_2_font)

    layout11.addWidget(self.textLabel14_3_3_2_2,2,0)

    self.textLabel14_3_3_3_3 = QLabel(self.groupBox5,"textLabel14_3_3_3_3")
    self.textLabel14_3_3_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_font = QFont(self.textLabel14_3_3_3_3.font())
    textLabel14_3_3_3_3_font.setPointSize(10)
    self.textLabel14_3_3_3_3.setFont(textLabel14_3_3_3_3_font)

    layout11.addWidget(self.textLabel14_3_3_3_3,1,1)

    self.textLabel14_3_3_3_2 = QLabel(self.groupBox5,"textLabel14_3_3_3_2")
    self.textLabel14_3_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_2_font = QFont(self.textLabel14_3_3_3_2.font())
    textLabel14_3_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_3_2.setFont(textLabel14_3_3_3_2_font)

    layout11.addWidget(self.textLabel14_3_3_3_2,0,1)

    self.textLabel14_3_3_2 = QLabel(self.groupBox5,"textLabel14_3_3_2")
    self.textLabel14_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_font = QFont(self.textLabel14_3_3_2.font())
    textLabel14_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_2.setFont(textLabel14_3_3_2_font)

    layout11.addWidget(self.textLabel14_3_3_2,1,0)

    self.textLabel14_3_3_3_3_3 = QLabel(self.groupBox5,"textLabel14_3_3_3_3_3")
    self.textLabel14_3_3_3_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_3.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_3_font = QFont(self.textLabel14_3_3_3_3_3.font())
    textLabel14_3_3_3_3_3_font.setPointSize(10)
    self.textLabel14_3_3_3_3_3.setFont(textLabel14_3_3_3_3_3_font)

    layout11.addWidget(self.textLabel14_3_3_3_3_3,2,1)

    self.textLabel14_3_3 = QLabel(self.groupBox5,"textLabel14_3_3")
    self.textLabel14_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_font = QFont(self.textLabel14_3_3.font())
    textLabel14_3_3_font.setPointSize(10)
    self.textLabel14_3_3.setFont(textLabel14_3_3_font)

    layout11.addWidget(self.textLabel14_3_3,0,0)

    self.textLabel14_3_3_2_2_2 = QLabel(self.groupBox5,"textLabel14_3_3_2_2_2")
    self.textLabel14_3_3_2_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_2.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_2_2.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_2_2_font = QFont(self.textLabel14_3_3_2_2_2.font())
    textLabel14_3_3_2_2_2_font.setPointSize(10)
    self.textLabel14_3_3_2_2_2.setFont(textLabel14_3_3_2_2_2_font)

    layout11.addWidget(self.textLabel14_3_3_2_2_2,3,0)

    self.textLabel14_3_3_3_3_4 = QLabel(self.groupBox5,"textLabel14_3_3_3_3_4")
    self.textLabel14_3_3_3_3_4.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_4.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_4_font = QFont(self.textLabel14_3_3_3_3_4.font())
    textLabel14_3_3_3_3_4_font.setPointSize(10)
    self.textLabel14_3_3_3_3_4.setFont(textLabel14_3_3_3_3_4_font)

    layout11.addWidget(self.textLabel14_3_3_3_3_4,3,1)
    groupBox5Layout.addLayout(layout11)
    spacer1_2 = QSpacerItem(10,10,QSizePolicy.Minimum,QSizePolicy.Minimum)
    groupBox5Layout.addItem(spacer1_2)
    layout13.addWidget(self.groupBox5)
    pyOnePop_StatsViewLayout.addLayout(layout13)

    self.groupBox4 = QGroupBox(self,"groupBox4")
    self.groupBox4.setSizePolicy(QSizePolicy(5,5,0,0,self.groupBox4.sizePolicy().hasHeightForWidth()))
    groupBox4_font = QFont(self.groupBox4.font())
    groupBox4_font.setPointSize(11)
    self.groupBox4.setFont(groupBox4_font)
    self.groupBox4.setColumnLayout(0,Qt.Vertical)
    self.groupBox4.layout().setSpacing(6)
    self.groupBox4.layout().setMargin(11)
    groupBox4Layout = QVBoxLayout(self.groupBox4.layout())
    groupBox4Layout.setAlignment(Qt.AlignTop)

    layout11_2 = QGridLayout(None,1,1,0,6,"layout11_2")

    self.textLabel14_3_3_2_2_3 = QLabel(self.groupBox4,"textLabel14_3_3_2_2_3")
    self.textLabel14_3_3_2_2_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_2_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_2_3_font = QFont(self.textLabel14_3_3_2_2_3.font())
    textLabel14_3_3_2_2_3_font.setPointSize(10)
    self.textLabel14_3_3_2_2_3.setFont(textLabel14_3_3_2_2_3_font)

    layout11_2.addWidget(self.textLabel14_3_3_2_2_3,2,0)

    self.textLabel14_3_3_3_3_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_3_2")
    self.textLabel14_3_3_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_2_font = QFont(self.textLabel14_3_3_3_3_2.font())
    textLabel14_3_3_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_3_3_2.setFont(textLabel14_3_3_3_3_2_font)

    layout11_2.addWidget(self.textLabel14_3_3_3_3_2,1,1)

    self.textLabel14_3_3_3_2_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_2_2")
    self.textLabel14_3_3_3_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_2_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_2_2_font = QFont(self.textLabel14_3_3_3_2_2.font())
    textLabel14_3_3_3_2_2_font.setPointSize(10)
    self.textLabel14_3_3_3_2_2.setFont(textLabel14_3_3_3_2_2_font)

    layout11_2.addWidget(self.textLabel14_3_3_3_2_2,0,1)

    self.textLabel14_3_3_2_3 = QLabel(self.groupBox4,"textLabel14_3_3_2_3")
    self.textLabel14_3_3_2_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_3_font = QFont(self.textLabel14_3_3_2_3.font())
    textLabel14_3_3_2_3_font.setPointSize(10)
    self.textLabel14_3_3_2_3.setFont(textLabel14_3_3_2_3_font)

    layout11_2.addWidget(self.textLabel14_3_3_2_3,1,0)

    self.textLabel14_3_3_3_3_3_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_3_3_2")
    self.textLabel14_3_3_3_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_3_2_font = QFont(self.textLabel14_3_3_3_3_3_2.font())
    textLabel14_3_3_3_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_3_3_3_2.setFont(textLabel14_3_3_3_3_3_2_font)

    layout11_2.addWidget(self.textLabel14_3_3_3_3_3_2,2,1)

    self.textLabel14_3_3_3 = QLabel(self.groupBox4,"textLabel14_3_3_3")
    self.textLabel14_3_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_3_font = QFont(self.textLabel14_3_3_3.font())
    textLabel14_3_3_3_font.setPointSize(10)
    self.textLabel14_3_3_3.setFont(textLabel14_3_3_3_font)

    layout11_2.addWidget(self.textLabel14_3_3_3,0,0)

    self.textLabel14_3_3_2_2_2_2 = QLabel(self.groupBox4,"textLabel14_3_3_2_2_2_2")
    self.textLabel14_3_3_2_2_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_2_2.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_2_2_2.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_2_2_2_font = QFont(self.textLabel14_3_3_2_2_2_2.font())
    textLabel14_3_3_2_2_2_2_font.setPointSize(10)
    self.textLabel14_3_3_2_2_2_2.setFont(textLabel14_3_3_2_2_2_2_font)

    layout11_2.addWidget(self.textLabel14_3_3_2_2_2_2,3,0)

    self.textLabel14_3_3_3_3_4_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_3_4_2")
    self.textLabel14_3_3_3_3_4_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_4_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_4_2_font = QFont(self.textLabel14_3_3_3_3_4_2.font())
    textLabel14_3_3_3_3_4_2_font.setPointSize(10)
    self.textLabel14_3_3_3_3_4_2.setFont(textLabel14_3_3_3_3_4_2_font)

    layout11_2.addWidget(self.textLabel14_3_3_3_3_4_2,3,1)
    groupBox4Layout.addLayout(layout11_2)
    spacer1_2_2 = QSpacerItem(10,10,QSizePolicy.Minimum,QSizePolicy.Minimum)
    groupBox4Layout.addItem(spacer1_2_2)
    pyOnePop_StatsViewLayout.addWidget(self.groupBox4)

    self.languageChange()

    self.resize(QSize(253,216).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyOnePop_StatsView"))
    self.groupBox3.setTitle(self.__tr("Task Outlook"))
    self.textLabel14.setText(self.__tr("Task"))
    self.textLabel14_2.setText(self.__tr("# Orgs"))
    self.textLabel14_3.setText(self.__tr("blah"))
    self.textLabel14_3_2.setText(self.__tr("blah blah"))
    self.groupBox5.setTitle(self.__tr("Statistics"))
    self.textLabel14_3_3_2_2.setText(self.__tr("Num. Orgs:"))
    self.textLabel14_3_3_3_3.setText(self.__tr("0000"))
    self.textLabel14_3_3_3_2.setText(self.__tr("0000"))
    self.textLabel14_3_3_2.setText(self.__tr("Dom. Merit:"))
    self.textLabel14_3_3_3_3_3.setText(self.__tr("0000"))
    self.textLabel14_3_3.setText(self.__tr("Avg. Merit:"))
    self.textLabel14_3_3_2_2_2.setText(self.__tr("Avg. Gest:"))
    self.textLabel14_3_3_3_3_4.setText(self.__tr("0000"))
    self.groupBox4.setTitle(self.__tr("Org Report"))
    self.textLabel14_3_3_2_2_3.setText(self.__tr("Age:"))
    self.textLabel14_3_3_3_3_2.setText(self.__tr("not, not, not"))
    self.textLabel14_3_3_3_2_2.setText(self.__tr("abcdef"))
    self.textLabel14_3_3_2_3.setText(self.__tr("Tasks:"))
    self.textLabel14_3_3_3_3_3_2.setText(self.__tr("20 gen."))
    self.textLabel14_3_3_3.setText(self.__tr("Name:"))
    self.textLabel14_3_3_2_2_2_2.setText(self.__tr("Clones:"))
    self.textLabel14_3_3_3_3_4_2.setText(self.__tr("13 in pop."))


  def __tr(self,s,c = None):
    return qApp.translate("pyOnePop_StatsView",s,c)
