# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'pySessionDumbViewBase.ui'
#
# Created: Thu Mar 31 04:19:24 2005
#      by: The PyQt User Interface Compiler (pyuic) 3.13
#
# WARNING! All changes made in this file will be lost!


from qt import *


class pySessionDumbViewBase(QWidget):
    def __init__(self,parent = None,name = None,fl = 0):
        QWidget.__init__(self,parent,name,fl)

        if not name:
            self.setName("pySessionDumbViewBase")


        pySessionDumbViewBaseLayout = QVBoxLayout(self,11,6,"pySessionDumbViewBaseLayout")

        layout1 = QVBoxLayout(None,0,6,"layout1")

        self.m_startpause_avida_pb = QPushButton(self,"m_startpause_avida_pb")
        layout1.addWidget(self.m_startpause_avida_pb)

        self.m_update_avida_pb = QPushButton(self,"m_update_avida_pb")
        layout1.addWidget(self.m_update_avida_pb)

        self.m_log_te = QTextEdit(self,"m_log_te")
        layout1.addWidget(self.m_log_te)
        pySessionDumbViewBaseLayout.addLayout(layout1)

        self.languageChange()

        self.resize(QSize(589,471).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)


    def languageChange(self):
        self.setCaption(self.__tr("Form2"))
        self.m_startpause_avida_pb.setText(self.__tr("pushButton1"))
        self.m_update_avida_pb.setText(self.__tr("Next Update..."))


    def __tr(self,s,c = None):
        return qApp.translate("pySessionDumbViewBase",s,c)
