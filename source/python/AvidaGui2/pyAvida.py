#
#  pyAvida.py
#  AvidaEd
#
#  Created by Kaben Nanlohy on 05.3.3.
#  Copyright (c) 2005 __MyCompanyName__. All rights reserved.
#
import pyAvidaThreadedDriver; reload(pyAvidaThreadedDriver)
from pyAvidaThreadedDriver import *

import pyMdtr; reload(pyMdtr)
from pyMdtr import *

import qt
from AvidaCore import *

class pyAvida(qt.QObject):

  def __init__(self):
    qt.QObject.__init__(self, None, self.__class__.__name__)

  def construct(self, genesis):
    self.m_name = genesis.GetFilename()
    self.m_environment = cEnvironment()
    cConfig.Setup(genesis)
    if 0 == self.m_environment.Load(cConfig.GetEnvironmentFilename()):
      print "Unable to load environment... aborting."
      self.m_population = None
      return None

    self.m_environment.GetInstSet().SetInstLib(cHardwareCPU.GetInstLib())
    cHardwareUtil.LoadInstSet_CPUOriginal(
      cConfig.GetInstFilename(),
      self.m_environment.GetInstSet())
    cConfig.SetNumInstructions(self.m_environment.GetInstSet().GetSize())
    cConfig.SetNumTasks(self.m_environment.GetTaskLib().GetSize())
    cConfig.SetNumReactions(self.m_environment.GetReactionLib().GetSize())
    cConfig.SetNumResources(self.m_environment.GetResourceLib().GetSize())
    
    # Test-CPU creation.
    test_interface = cPopulationInterface()
    BuildTestPopInterface(test_interface)
    cTestCPU.Setup(
      self.m_environment.GetInstSet(),
      self.m_environment,
      self.m_environment.GetResourceLib().GetSize(),
      test_interface)

    self.m_avida_threaded_driver = pyAvidaThreadedDriver(self.m_environment)
    self.m_avida_threaded_driver.construct()
    self.m_population = self.m_avida_threaded_driver.GetPopulation()
    self.m_avida_thread_mdtr = pyMdtr()
    self.m_should_update = False

    self.connect(
      self.m_avida_thread_mdtr,
      qt.PYSIGNAL("doPauseAvidaSig"),
      self.doPauseAvidaSlot)
    self.connect(
      self.m_avida_thread_mdtr,
      qt.PYSIGNAL("doStartAvidaSig"),
      self.doStartAvidaSlot)
    self.connect(
      self.m_avida_thread_mdtr,
      qt.PYSIGNAL("doUpdateAvidaSig"),
      self.doUpdateAvidaSlot)
    self.connect(
      self.m_avida_thread_mdtr,
      qt.PYSIGNAL("doCloseAvidaSig"),
      self.doCloseAvidaSlot)

    self.m_update_ck_timer = qt.QTimer()
    self.connect(
      self.m_update_ck_timer,
      qt.SIGNAL("timeout()"),
      self.updateCheckSlot)
    self.m_update_ck_timer.start(0)

    return self

  def addGuiWorkFunctor(self, thread_work_functor):
    self.m_avida_threaded_driver.addGuiWorkFunctor(thread_work_functor)
    
  def removeGuiWorkFunctor(self, thread_work_functor):
    self.m_avida_threaded_driver.removeGuiWorkFunctor(thread_work_functor)

  def destruct(self):
    print("pyAvida.destruct() ...")
    self.m_avida_thread_mdtr.emit(qt.PYSIGNAL("doCloseAvidaSig"),())
    if hasattr(self, "m_update_ck_timer"):
      self.m_update_ck_timer.stop()
      del self.m_update_ck_timer
    else:
      print("pyAvida.destruct() self.m_update_ck_timer missing.")

    if hasattr(self, "m_avida_thread_mdtr"):
      del self.m_avida_thread_mdtr
    else:
      print("pyAvida.destruct() self.m_avida_thread_mdtr missing.")

    if hasattr(self, "m_avida_threaded_driver"):
      self.m_avida_threaded_driver.doExit()
      self.m_avida_threaded_driver.m_thread.join()
      del self.m_avida_threaded_driver
    else:
      print("pyAvida.destruct() self.m_avida_threaded_driver missing.")

    print("pyAvida.destruct() done.")

  def __del__(self):
    print("pyAvida.__del__()...")
    self.destruct()
    print("pyAvida.__del__() done.")

  def updateCheckSlot(self):
    if self.m_avida_threaded_driver.m_updated_semaphore.acquire(False):
      self.m_avida_thread_mdtr.emit(qt.PYSIGNAL("AvidaUpdatedSig"),())
      if True == self.m_should_update:
        self.doUpdateAvidaSlot()

  def doPauseAvidaSlot(self):
    self.m_should_update = False;

  def doStartAvidaSlot(self):
    self.m_should_update = True;
    self.doUpdateAvidaSlot()

  def doUpdateAvidaSlot(self):
    try:
      self.m_avida_threaded_driver.doUpdate()
    except AttributeError:
      pass

  def doStepAvidaSlot(self, cell_id):
    print("pyAvida.doStepAvidaSlot() ...")

  def doCloseAvidaSlot(self):
    print("pyAvida.doCloseAvidaSlot() ...")
    self.m_avida_threaded_driver.doExit()
    if self.m_avida_threaded_driver.m_thread.isAlive():
      self.m_avida_threaded_driver.m_thread.join()
    else:
      print("pyAvida.doCloseAvidaSlot() thread is dead!")
