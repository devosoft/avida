
from AvidaCore import *
import qt


class pyAvidaDumbGuiView(qt.QMainWindow):
  def __init__(self, *args):
    apply(qt.QMainWindow.__init__, (self,) + args)
    self.central_vbox = qt.QVBox(self)
    self.startpause_avida_pb = qt.QPushButton(self.central_vbox)
    self.update_avida_pb = qt.QPushButton(self.central_vbox)
    self.log_te = qt.QTextEdit(self.central_vbox)

    self.update_avida_pb.setText("Update...")

    self.log_te.setTextFormat(qt.Qt.LogText)
    self.log_te.setMaxLogLines(400)

    self.setCentralWidget(self.central_vbox)


class pyAvidaDumbGuiController(qt.QObject):

  def __init__(self, main_controller_data):
    self.main_controller_data = main_controller_data
    self.avida_dumb_gui_view = pyAvidaDumbGuiView()

    self.start_pb_text = "Start..."
    self.pause_pb_text = "Pause..."
    self.should_update = False

    self.connect(
      self.avida_dumb_gui_view.startpause_avida_pb,
      qt.SIGNAL("clicked()"),
      self.startPausePBClickedSlot)
    self.connect(
      self.avida_dumb_gui_view.update_avida_pb,
      qt.SIGNAL("clicked()"),
      self.updatePBClickedSlot)
    self.connect(
      self.main_controller_data.avida_state_mediator,
      qt.PYSIGNAL("AvidaUpdatedSig"),
      self.avidaUpdatedSlot)

    self.doPause()

    self.avida_dumb_gui_view.show()

  def doPause(self):
    self.should_update = False
    self.avida_dumb_gui_view.startpause_avida_pb.setText(self.start_pb_text)
    self.avida_dumb_gui_view.update_avida_pb.setEnabled(True)
    self.main_controller_data.avida_state_mediator.emit(qt.PYSIGNAL("doPauseAvidaSig"), ())

  def doStart(self):
    self.should_update = True
    self.avida_dumb_gui_view.startpause_avida_pb.setText(self.pause_pb_text)
    self.avida_dumb_gui_view.update_avida_pb.setEnabled(False)
    self.main_controller_data.avida_state_mediator.emit(qt.PYSIGNAL("doStartAvidaSig"), ())

  def startPausePBClickedSlot(self):
    if True == self.should_update:
      self.doPause()
    else:
      self.doStart()
    
  def updatePBClickedSlot(self):
    self.main_controller_data.avida_state_mediator.emit(qt.PYSIGNAL("doUpdateAvidaSig"), ())

  def avidaUpdatedSlot(self):
    stats = self.main_controller_data.avida_threaded_driver.GetPopulation().GetStats()
    self.avida_dumb_gui_view.log_te.append(
      "UD: " + repr(stats.GetUpdate()) +
      "\t Gen: " + repr(stats.SumGeneration().Average()) +
      "\t Fit: " + repr(stats.GetAveFitness()) +
      "\t Size: " + repr(self.main_controller_data.avida_threaded_driver.GetPopulation().GetNumOrganisms())
    )
    #self.avida_dumb_gui_view.log_te.append(
    #  "UD: %d\t Gen: %g\t \t Fit: %g\t Size: %d"
    #  + stats.GetUpdate() +
    #  "\t Gen: "
    #  + stats.SumGeneration().Average() +
    #  "\t Fit: "
    #  + stats.GetAveFitness() +
    #  "\t Size: "
    #  + self.main_controller_data.avida_threaded_driver.GetPopulation().GetNumOrganisms()
    #)

