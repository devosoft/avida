import qt

class pySessionDumbView(qt.QMainWindow):
  def __init__(self, *args):
    apply(qt.QMainWindow.__init__, (self,) + args)
    self.m_central_vbox = qt.QVBox(self)
    self.m_startpause_avida_pb = qt.QPushButton(self.m_central_vbox)
    self.m_update_avida_pb = qt.QPushButton(self.m_central_vbox)
    self.m_log_te = qt.QTextEdit(self.m_central_vbox)

    self.m_update_avida_pb.setText("Next Update...")

    self.m_log_te.setTextFormat(qt.Qt.LogText)
    self.m_log_te.setMaxLogLines(400)

    self.setCentralWidget(self.m_central_vbox)
  def __del__(self):
    print "pySessionDumbView.__del__()."
