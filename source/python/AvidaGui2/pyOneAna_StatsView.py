# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyOneAna_StatsView.ui'
#
# Created: Fri Dec 3 10:26:51 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyOneAna_StatsView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyOneAna_StatsView")


    pyOneAna_StatsViewLayout = QVBoxLayout(self,0,6,"pyOneAna_StatsViewLayout")

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

    self.textLabel14_3_3_3 = QLabel(self.groupBox4,"textLabel14_3_3_3")
    self.textLabel14_3_3_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_3_font = QFont(self.textLabel14_3_3_3.font())
    textLabel14_3_3_3_font.setPointSize(10)
    textLabel14_3_3_3_font.setBold(1)
    textLabel14_3_3_3_font.setUnderline(1)
    self.textLabel14_3_3_3.setFont(textLabel14_3_3_3_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_3)

    self.textLabel14_3_3_3_2_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_2_2")
    self.textLabel14_3_3_3_2_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_2_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_2_2_font = QFont(self.textLabel14_3_3_3_2_2.font())
    textLabel14_3_3_3_2_2_font.setPointSize(10)
    self.textLabel14_3_3_3_2_2.setFont(textLabel14_3_3_3_2_2_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_3_2_2)

    self.textLabel14_3_3_2_3 = QLabel(self.groupBox4,"textLabel14_3_3_2_3")
    self.textLabel14_3_3_2_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_3_font = QFont(self.textLabel14_3_3_2_3.font())
    textLabel14_3_3_2_3_font.setPointSize(10)
    textLabel14_3_3_2_3_font.setBold(1)
    textLabel14_3_3_2_3_font.setUnderline(1)
    self.textLabel14_3_3_2_3.setFont(textLabel14_3_3_2_3_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_2_3)

    self.textLabel14_3_3_3_3_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_3_2")
    self.textLabel14_3_3_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_2_font = QFont(self.textLabel14_3_3_3_3_2.font())
    textLabel14_3_3_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_3_3_2.setFont(textLabel14_3_3_3_3_2_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_3_3_2)

    self.textLabel14_3_3_2_2_3 = QLabel(self.groupBox4,"textLabel14_3_3_2_2_3")
    self.textLabel14_3_3_2_2_3.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_2_2_3.sizePolicy().hasHeightForWidth()))
    self.textLabel14_3_3_2_2_3.setMaximumSize(QSize(32767,40))
    textLabel14_3_3_2_2_3_font = QFont(self.textLabel14_3_3_2_2_3.font())
    textLabel14_3_3_2_2_3_font.setPointSize(10)
    textLabel14_3_3_2_2_3_font.setBold(1)
    textLabel14_3_3_2_2_3_font.setUnderline(1)
    self.textLabel14_3_3_2_2_3.setFont(textLabel14_3_3_2_2_3_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_2_2_3)

    self.textLabel14_3_3_3_3_3_2 = QLabel(self.groupBox4,"textLabel14_3_3_3_3_3_2")
    self.textLabel14_3_3_3_3_3_2.setSizePolicy(QSizePolicy(5,5,0,0,self.textLabel14_3_3_3_3_3_2.sizePolicy().hasHeightForWidth()))
    textLabel14_3_3_3_3_3_2_font = QFont(self.textLabel14_3_3_3_3_3_2.font())
    textLabel14_3_3_3_3_3_2_font.setPointSize(10)
    self.textLabel14_3_3_3_3_3_2.setFont(textLabel14_3_3_3_3_3_2_font)
    groupBox4Layout.addWidget(self.textLabel14_3_3_3_3_3_2)
    spacer1_2_2 = QSpacerItem(16,16,QSizePolicy.Minimum,QSizePolicy.Minimum)
    groupBox4Layout.addItem(spacer1_2_2)
    pyOneAna_StatsViewLayout.addWidget(self.groupBox4)

    self.languageChange()

    self.resize(QSize(124,167).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyOneAna_StatsView"))
    self.groupBox4.setTitle(self.__tr("Summary"))
    self.textLabel14_3_3_3.setText(self.__tr("Run on:"))
    self.textLabel14_3_3_3_2_2.setText(self.__tr("8/9/04"))
    self.textLabel14_3_3_2_3.setText(self.__tr("Tasks:"))
    self.textLabel14_3_3_3_3_2.setText(self.__tr("not, not, not"))
    self.textLabel14_3_3_2_2_3.setText(self.__tr("Mutation Rate:"))
    self.textLabel14_3_3_3_3_3_2.setText(self.__tr("0.4"))


  def __tr(self,s,c = None):
    return qApp.translate("pyOneAna_StatsView",s,c)
