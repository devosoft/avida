# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/svn/avida2/trunk/source/bindings/Boost.Python/AvidaGui2/pyTest.ui'
#
# Created: Thu Feb 17 15:14:31 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
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
