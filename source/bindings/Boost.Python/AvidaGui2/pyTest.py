# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyTest.ui'
#
# Created: Fri Dec 3 10:26:52 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyTest(QDialog):
  def __init__(self,parent = None,name = None,modal = 0,fl = 0):
    QDialog.__init__(self,parent,name,modal,fl)

    if not name:
      self.setName("pyTest")



    self.pySessionDumbView1 = pySessionDumbView(self,"pySessionDumbView1")
    self.pySessionDumbView1.setGeometry(QRect(60,50,431,351))

    self.languageChange()

    self.resize(QSize(600,480).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("Form1"))


  def __tr(self,s,c = None):
    return qApp.translate("pyTest",s,c)
