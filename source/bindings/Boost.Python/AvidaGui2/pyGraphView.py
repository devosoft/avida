# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyGraphView.ui'
#
# Created: Fri Dec 3 10:26:50 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyGraphView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyGraphView")


    pyGraphViewLayout = QVBoxLayout(self,0,0,"pyGraphViewLayout")
    pyGraphViewLayout.setResizeMode(QLayout.Minimum)

    self.languageChange()

    self.resize(QSize(161,51).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyGraphView"))


  def __tr(self,s,c = None):
    return qApp.translate("pyGraphView",s,c)
