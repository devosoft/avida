# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyGradientScaleView.ui'
#
# Created: Fri Dec 3 10:26:50 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pyGradientScaleView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    if not name:
      self.setName("pyGradientScaleView")

    self.setSizePolicy(QSizePolicy(5,5,0,0,self.sizePolicy().hasHeightForWidth()))
    self.setMaximumSize(QSize(32767,32767))

    pyGradientScaleViewLayout = QVBoxLayout(self,0,0,"pyGradientScaleViewLayout")
    pyGradientScaleViewLayout.setResizeMode(QLayout.Minimum)

    self.textLabel4 = QLabel(self,"textLabel4")
    self.textLabel4.setSizePolicy(QSizePolicy(1,1,0,0,self.textLabel4.sizePolicy().hasHeightForWidth()))
    self.textLabel4.setBackgroundMode(QLabel.PaletteShadow)
    self.textLabel4.setPaletteBackgroundColor(QColor(220,220,220))
    textLabel4_font = QFont(self.textLabel4.font())
    textLabel4_font.setPointSize(10)
    self.textLabel4.setFont(textLabel4_font)
    self.textLabel4.setScaledContents(0)
    self.textLabel4.setAlignment(QLabel.AlignCenter)
    pyGradientScaleViewLayout.addWidget(self.textLabel4)

    self.languageChange()

    self.resize(QSize(124,188).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyGradientScaleView"))
    self.textLabel4.setText(self.__tr("mock\n"
"gradient"))


  def __tr(self,s,c = None):
    return qApp.translate("pyGradientScaleView",s,c)
