# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '/Users/kaben/Projects/Software/Avida/branch.kaben/avida--current--2.0--base-0/source/bindings/Boost.Python/AvidaGui2/pyTwoPop_GraphView.ui'
#
# Created: Fri Dec 3 10:26:53 2004
#      by: The PyQt User Interface Compiler (pyuic) 3.12
#
# WARNING! All changes made in this file will be lost!


from qt import *
from pyGraphCtrl import pyGraphCtrl

image0_data = \
  "\x89\x50\x4e\x47\x0d\x0a\x1a\x0a\x00\x00\x00\x0d" \
  "\x49\x48\x44\x52\x00\x00\x00\x0f\x00\x00\x00\x0e" \
  "\x08\x06\x00\x00\x00\xf0\x8a\x46\xef\x00\x00\x01" \
  "\xd7\x49\x44\x41\x54\x78\x9c\x85\x92\xcd\x6a\x53" \
  "\x51\x14\x85\xbf\x9b\x5c\x09\x89\x69\xa3\xad\xb6" \
  "\x45\xc1\xe2\x4f\x1a\x9d\x28\x8a\x13\x91\x80\xc1" \
  "\xd0\xa9\xe0\x44\xa1\x82\x38\xa8\x93\x4e\xd4\xe2" \
  "\xd4\x17\x70\x20\x8a\x51\x21\xfa\x04\xfa\x06\x05" \
  "\x87\x22\xc4\x61\x41\x09\x0d\x04\x35\x85\x04\x93" \
  "\xe6\xaf\xf6\x26\xb9\xe7\x2c\x07\x6d\xd3\x34\x11" \
  "\xba\x60\x71\xe0\xec\xb3\xce\x62\xef\xbd\x90\xc4" \
  "\x20\x5b\xad\x96\x96\x9f\x3c\x55\xa5\x52\xd1\x70" \
  "\x6d\x98\x01\x06\xd0\x6e\xb7\x95\x79\x91\xe5\x88" \
  "\x3b\x85\x31\x96\x83\xe0\x48\x02\xa0\xd3\xe9\x28" \
  "\xf3\x32\xcb\x4c\x60\x8e\xdf\xb5\x3c\x53\x73\x51" \
  "\x42\xa1\xd0\xbe\xc7\xb1\x58\x8c\x54\x2a\x45\x38" \
  "\x1c\x76\xfa\x62\xdf\xf7\xf5\xe6\x55\x96\xe3\x9c" \
  "\xa3\x56\xde\xe2\xd8\xd4\x18\x96\x3d\x67\x21\xac" \
  "\x0c\x5e\x6f\x8b\x52\x23\xcf\xdd\x07\xb7\x48\x24" \
  "\x12\x8e\x23\x89\x77\xaf\xdf\x2b\xd6\x3d\xc5\x46" \
  "\xb9\x83\xac\xc0\xd9\x15\x81\x24\x8c\x84\x95\xc5" \
  "\xf3\x7d\xa6\xa7\xc7\x21\x5a\x63\xe1\xe1\x6d\x82" \
  "\x97\x2f\x5e\x55\x77\xfd\x30\xed\x3f\x86\xdd\x16" \
  "\xf6\x3b\x82\x95\xc5\x58\x8b\x91\xa5\xd6\xdc\xe4" \
  "\x68\x64\x92\x46\x6f\x9d\xc0\xcd\xf9\x1b\x14\x36" \
  "\x7e\xe0\x19\x7f\x48\xb8\x0d\xbb\xf3\xa1\x91\x30" \
  "\xb2\x08\x68\x79\x3d\xca\xbf\xea\x04\xa2\xd1\xa8" \
  "\xb3\xf4\x68\x81\x92\xf7\x9d\xae\x35\xfb\x5c\xa5" \
  "\xed\xd3\x58\xdb\x77\xb7\xb2\xf4\x7a\x06\x6f\xab" \
  "\xbb\x37\xed\xc2\x5a\x41\x99\xe7\x9f\x38\x3d\x76" \
  "\x9e\x4d\xbf\xc3\xdf\x6e\x13\x83\xb0\x56\x08\x70" \
  "\x02\x2e\xae\x13\xc2\xc8\x30\x39\x31\xce\x89\xb3" \
  "\x03\xab\x02\xc8\xe5\xbe\xe9\xe3\x87\x2f\x6c\x7a" \
  "\x75\xae\xa5\xcf\x10\x0c\x06\xfb\xb5\x66\xa3\x4d" \
  "\xee\xeb\x1a\x13\xe1\x0b\xb8\xee\x21\xae\xcf\x4f" \
  "\x32\x92\x9a\x95\x95\xcf\xba\x77\x67\x49\xd5\x6a" \
  "\x75\x24\x61\xf9\x7c\x5e\x8b\xf7\x9f\xe9\xf1\xe2" \
  "\x5b\x15\x8b\x45\xb9\xc3\xa9\x49\xa7\x53\x4e\x28" \
  "\xe4\x2a\x12\x89\x8c\x24\x2a\x1e\x8f\x3b\x33\x27" \
  "\xa3\xaa\x55\xea\xcc\xce\xce\x3a\x23\x62\x80\x64" \
  "\x32\xe9\xfc\xef\x1e\xe0\xd2\x95\x04\xeb\x3f\x6b" \
  "\x3b\x53\x3d\x20\xfc\xc3\x5c\x5d\x5d\x55\xa9\x54" \
  "\x92\x24\xfe\x01\x03\x3d\x5a\xfa\x32\x50\xe5\x56" \
  "\x00\x00\x00\x00\x49\x45\x4e\x44\xae\x42\x60\x82"

class pyTwoPop_GraphView(QWidget):
  def __init__(self,parent = None,name = None,fl = 0):
    QWidget.__init__(self,parent,name,fl)

    self.image0 = QPixmap()
    self.image0.loadFromData(image0_data,"PNG")
    if not name:
      self.setName("pyTwoPop_GraphView")


    pyTwoPop_GraphViewLayout = QVBoxLayout(self,0,6,"pyTwoPop_GraphViewLayout")

    layout210 = QHBoxLayout(None,0,6,"layout210")
    spacer152 = QSpacerItem(1,32,QSizePolicy.Minimum,QSizePolicy.Minimum)
    layout210.addItem(spacer152)

    self.textLabel9_2 = QLabel(self,"textLabel9_2")
    self.textLabel9_2.setSizePolicy(QSizePolicy(3,5,0,0,self.textLabel9_2.sizePolicy().hasHeightForWidth()))
    textLabel9_2_font = QFont(self.textLabel9_2.font())
    textLabel9_2_font.setPointSize(11)
    self.textLabel9_2.setFont(textLabel9_2_font)
    layout210.addWidget(self.textLabel9_2)

    layout178 = QVBoxLayout(None,0,6,"layout178")
    spacer126 = QSpacerItem(1,1,QSizePolicy.Minimum,QSizePolicy.Minimum)
    layout178.addItem(spacer126)

    self.toolButton31_2 = QToolButton(self,"toolButton31_2")
    toolButton31_2_font = QFont(self.toolButton31_2.font())
    toolButton31_2_font.setPointSize(10)
    self.toolButton31_2.setFont(toolButton31_2_font)
    self.toolButton31_2.setIconSet(QIconSet(self.image0))
    layout178.addWidget(self.toolButton31_2)
    spacer126_2 = QSpacerItem(1,1,QSizePolicy.Minimum,QSizePolicy.Minimum)
    layout178.addItem(spacer126_2)
    layout210.addLayout(layout178)
    pyTwoPop_GraphViewLayout.addLayout(layout210)

    self.pyGraphCtrl1 = pyGraphCtrl(self,"pyGraphCtrl1")
    self.pyGraphCtrl1.setSizePolicy(QSizePolicy(3,3,0,0,self.pyGraphCtrl1.sizePolicy().hasHeightForWidth()))
    self.pyGraphCtrl1.setMinimumSize(QSize(100,100))
    pyTwoPop_GraphViewLayout.addWidget(self.pyGraphCtrl1)

    layout262_2 = QHBoxLayout(None,0,6,"layout262_2")
    spacer200_2_2 = QSpacerItem(10,10,QSizePolicy.Expanding,QSizePolicy.Minimum)
    layout262_2.addItem(spacer200_2_2)

    self.toolButton31_2_2_2 = QToolButton(self,"toolButton31_2_2_2")
    self.toolButton31_2_2_2.setSizePolicy(QSizePolicy(0,0,0,0,self.toolButton31_2_2_2.sizePolicy().hasHeightForWidth()))
    toolButton31_2_2_2_font = QFont(self.toolButton31_2_2_2.font())
    toolButton31_2_2_2_font.setPointSize(10)
    self.toolButton31_2_2_2.setFont(toolButton31_2_2_2_font)
    layout262_2.addWidget(self.toolButton31_2_2_2)
    spacer200_3 = QSpacerItem(10,10,QSizePolicy.Expanding,QSizePolicy.Minimum)
    layout262_2.addItem(spacer200_3)
    pyTwoPop_GraphViewLayout.addLayout(layout262_2)

    layout262 = QHBoxLayout(None,0,6,"layout262")
    spacer200_2 = QSpacerItem(10,10,QSizePolicy.Expanding,QSizePolicy.Minimum)
    layout262.addItem(spacer200_2)

    self.toolButton31_2_2 = QToolButton(self,"toolButton31_2_2")
    self.toolButton31_2_2.setSizePolicy(QSizePolicy(0,0,0,0,self.toolButton31_2_2.sizePolicy().hasHeightForWidth()))
    toolButton31_2_2_font = QFont(self.toolButton31_2_2.font())
    toolButton31_2_2_font.setPointSize(10)
    self.toolButton31_2_2.setFont(toolButton31_2_2_font)
    layout262.addWidget(self.toolButton31_2_2)

    self.comboBox1 = QComboBox(0,self,"comboBox1")
    comboBox1_font = QFont(self.comboBox1.font())
    comboBox1_font.setPointSize(11)
    self.comboBox1.setFont(comboBox1_font)
    layout262.addWidget(self.comboBox1)
    spacer200 = QSpacerItem(10,10,QSizePolicy.Expanding,QSizePolicy.Minimum)
    layout262.addItem(spacer200)
    pyTwoPop_GraphViewLayout.addLayout(layout262)

    self.languageChange()

    self.resize(QSize(132,221).expandedTo(self.minimumSizeHint()))
    self.clearWState(Qt.WState_Polished)


  def languageChange(self):
    self.setCaption(self.__tr("pyTwoPop_GraphView"))
    self.textLabel9_2.setText(self.__tr("Graph"))
    self.toolButton31_2.setText(QString.null)
    self.toolButton31_2_2_2.setText(self.__tr("+"))
    self.toolButton31_2_2.setText(self.__tr("-"))
    self.comboBox1.clear()
    self.comboBox1.insertItem(self.__tr("Merit"))


  def __tr(self,s,c = None):
    return qApp.translate("pyTwoPop_GraphView",s,c)
