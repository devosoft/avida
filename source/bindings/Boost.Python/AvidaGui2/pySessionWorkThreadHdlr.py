import pyAvidaThreadedDriver; reload(pyAvidaThreadedDriver)
from pyAvidaThreadedDriver import *

import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import qt

class pyDeleteCheck:
  def __del__(self):
    print "pyDeleteCheck.__del__()"

class pySessionWorkThreadHdlr(qt.QObject):

  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)

  def construct(self, session_mdl):
    self.m_session_mdl = session_mdl
    self.m_session_mdl.m_avida_threaded_driver = pyAvidaThreadedDriver(self.m_session_mdl.m_avida_core_data.m_environment)
    self.m_avida_threaded_driver = pyAvidaThreadedDriver(self.m_session_mdl.m_avida_core_data.m_environment)
    self.m_updated_semaphore = self.m_session_mdl.m_avida_threaded_driver.m_updated_semaphore
    self.m_updated_semaphore = self.m_avida_threaded_driver.m_updated_semaphore
    self.m_should_update = False
    self.m_session_mdl.m_avida_threaded_driver.construct()
    self.m_avida_threaded_driver.construct()
    self.m_session_mdl.m_population = self.m_avida_threaded_driver.GetPopulation()

    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr = pyMdtr()
    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doPauseAvidaSig"), self.doPauseAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doStartAvidaSig"), self.doStartAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doUpdateAvidaSig"), self.doUpdateAvidaSlot)
    self.connect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doCloseAvidaSig"), self.doCloseAvidaSlot)

    self.m_update_ck_timer = qt.QTimer()
    self.connect(self.m_update_ck_timer, qt.SIGNAL("timeout()"), self.updateCheckSlot)
    self.m_update_ck_timer.start(100)


  def destruct(self):
    print("pySessionWorkThreadHdlr.destruct()...")
    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("doCloseAvidaSig"),())
    if hasattr(self, "m_update_ck_timer"):
      self.m_update_ck_timer.stop()
      del self.m_update_ck_timer
    else:
      print("pySessionWorkThreadHdlr.destruct() self.m_update_ck_timer missing.")

    self.disconnect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doPauseAvidaSig"), self.doPauseAvidaSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doStartAvidaSig"), self.doStartAvidaSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doUpdateAvidaSig"), self.doUpdateAvidaSlot)
    self.disconnect(self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr, qt.PYSIGNAL("doCloseAvidaSig"), self.doCloseAvidaSlot)

    if hasattr(self.m_session_mdl.m_session_mdtr, "m_avida_threaded_driver_mdtr"):
      del self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr
    else:
      print("pySessionWorkThreadHdlr.destruct() self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr missing.")

    if hasattr(self, "updated_semaphore"):
      del self.m_updated_semaphore
    else:
      print("pySessionWorkThreadHdlr.destruct() self.m_updated_semaphore missing.")

    #if hasattr(self.m_session_mdl, "m_avida_threaded_driver"):
    if hasattr(self, "m_avida_threaded_driver"):
      self.m_avida_threaded_driver.doExit()
      self.m_avida_threaded_driver.m_thread.join()
#      del self.m_avida_threaded_driver.m_thread
      #del self.m_session_mdl.m_avida_threaded_driver
      del self.m_avida_threaded_driver
    else:
      #print("pySessionWorkThreadHdlr.destruct() self.m_session_mdl.m_avida_threaded_driver missing.")
      print("pySessionWorkThreadHdlr.destruct() self.m_avida_threaded_driver missing.")

    if hasattr(self, "m_session_mdl"):
      del self.m_session_mdl
    else:
      print("pySessionWorkThreadHdlr.destruct() self.m_session_mdl missing.")

    print("pySessionWorkThreadHdlr.destruct() done.")

  def __del__(self):
    print("pySessionWorkThreadHdlr.__del__()...")
    self.destruct()
    print("pySessionWorkThreadHdlr.__del__() done.")

  def updateCheckSlot(self):
    #if hasattr(self.m_session_mdl, "m_avida_threaded_driver"):
    #  if self.m_session_mdl.m_avida_threaded_driver.m_updated_semaphore.acquire(False):
    #    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("AvidaUpdatedSig"),())
    #    if True == self.m_should_update:
    #      self.doUpdateAvidaSlot()

    #try:
    #  if self.m_updated_semaphore.acquire(False):
    #    self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("AvidaUpdatedSig"),())
    #    if True == self.m_should_update:
    #      self.doUpdateAvidaSlot()
    #except AttributeError:
    #  pass

    if self.m_updated_semaphore.acquire(False):
      self.m_session_mdl.m_session_mdtr.m_avida_threaded_driver_mdtr.emit(qt.PYSIGNAL("AvidaUpdatedSig"),())
      if True == self.m_should_update:
        self.doUpdateAvidaSlot()

    pass
  def doPauseAvidaSlot(self):
    self.m_should_update = False;

  def doStartAvidaSlot(self):
    self.m_should_update = True;
    self.doUpdateAvidaSlot()

  def doUpdateAvidaSlot(self):
    try:
      #self.session_mdl.avida_threaded_driver.doUpdate()
      self.m_avida_threaded_driver.doUpdate()
    except AttributeError:
      pass

  def doStepAvidaSlot(self, cell_id):
    print("doStepAvidaSlot")

  def doCloseAvidaSlot(self):
    #self.m_session_mdl.m_avida_threaded_driver.doExit()
    self.m_avida_threaded_driver.doExit()
    #self.m_session_mdl.m_avida_threaded_driver.m_thread.join()
    if self.m_avida_threaded_driver.m_thread.isAlive():
      self.m_avida_threaded_driver.m_thread.join()
    else:
      print("pySessionWorkThreadHdlr.doCloseAvidaSlot() thread is dead!")


# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pySessionWorkThreadHdlr(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    print """
    FIXME: pySessionWorkThreadHdlr
    pySessionWorkThreadHdlr has no unit tests.
    """
    self.adoptUnitTestSuite("pyAvidaThreadedDriver")
    self.adoptUnitTestSuite("pyMdtr")

    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)
    self.adoptUnitTestCase(crashDummy())
