
import qt

class pyAvidaDriverController(qt.QObject):

  def __init__(self, main_controller_data):
    self.avida_threaded_driver = main_controller_data.avida_threaded_driver
    self.avida_state_mediator = main_controller_data.avida_state_mediator
    self.should_update = False

    self.connect(self.avida_state_mediator, qt.PYSIGNAL("doPauseAvidaSig"), self.doPauseAvidaSlot)
    self.connect(self.avida_state_mediator, qt.PYSIGNAL("doStartAvidaSig"), self.doStartAvidaSlot)
    self.connect(self.avida_state_mediator, qt.PYSIGNAL("doUpdateAvidaSig"), self.doUpdateAvidaSlot)

    self.update_ck_timer = qt.QTimer()
    self.connect(self.update_ck_timer, qt.SIGNAL("timeout()"), self.updateCheckSlot)
    self.update_ck_timer.start(100)

  def updateCheckSlot(self):
    if self.avida_threaded_driver.updated_semaphore.acquire(False):
      self.avida_state_mediator.emit(qt.PYSIGNAL("AvidaUpdatedSig"),())
      if True == self.should_update:
        self.doUpdateAvidaSlot()

  def doPauseAvidaSlot(self):
    self.should_update = False;

  def doStartAvidaSlot(self):
    self.should_update = True;
    self.doUpdateAvidaSlot()

  def doUpdateAvidaSlot(self):
    self.avida_threaded_driver.doUpdate()

  def doStepAvidaSlot(self, cell_id):
    print("doStepAvidaSlot")
    pass

  def doCloseAvidaSlot(self):
    self.avida_threaded_driver.doExit()
    self.avida_threaded_driver.thread.join()
