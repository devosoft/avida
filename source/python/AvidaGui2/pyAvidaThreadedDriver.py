
from AvidaCore import pyAvidaDriver
import atexit, threading

class pyAvidaThreadedDriver(pyAvidaDriver):

  def construct(self):
    self.doSetProcessBitesize(1)
    self.m_thread.start()
  def __init__(self, environment):
    self.m_environment = environment
    pyAvidaDriver.__init__(self, self.m_environment)
    self.m_thread = threading.Thread(target=self.___threadloop___)
    print("""
    FIXME : class pyAvidaThreadedDriver : AvidaGui/py_avida_threaded_driver.py :
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    ...
    (so, think of a good way to associate a library mutex with the Avida libraries.)
    """)
    self.m_lock = threading.Lock()
    self.m_do_update_semaphore = threading.Semaphore(0)
    self.m_updated_semaphore = threading.Semaphore(0)

    # make sure that driver is deleted when python exits -- otherwise
    # its processing thread loop may continue to run when python tries
    # to exit, preventing the python process from exiting.
    atexit.register(self.__del__)

    self.m_work_functors = []

    pass

  def __del__(self):
#    print("pyAvidaThreadedDriver.__del__()...")
    self.doExit()
#    print("pyAvidaThreadedDriver.__del__() done.")

  def ___threadloop___(self):
    while True:
      # wait for notification by another thread before starting update.
      self.m_do_update_semaphore.acquire()
      # begin update.
      self.m_lock.acquire()
      self.m_updating = self.ProcessSome(self.m_process_bitesize)
      while self.m_updating:
        # yield lock to other threads.
        self.m_lock.release()
        self.m_lock.acquire()
        # continue update.
        self.m_updating = self.ProcessSome(self.m_process_bitesize)
      # do work on behalf of gui.
      for work_functor in self.m_work_functors:
        functor_still_working = True
        while functor_still_working:
          self.m_lock.release()
          self.m_lock.acquire()
          functor_still_working = work_functor.doSomeWork(self)
          
      self.m_lock.release()
      # tell other threads that an update has completed (possibly
      # unsuccessfully).
      self.m_updated_semaphore.release()
      if self.getDoneFlag():
        return

  def addGuiWorkFunctor(self, thread_work_functor):
    self.m_lock.acquire()
    self.m_work_functors.append(thread_work_functor)
    self.m_lock.release()
    
  def removeGuiWorkFunctor(self, thread_work_functor):
    self.m_lock.acquire()
    self.m_work_functors.remove(thread_work_functor)
    self.m_lock.release()

  def doExit(self):
#    print("pyAvidaThreadedDriver.doExit()...")
    self.m_lock.acquire()
    self.setDoneFlag()
    self.m_lock.release()
    self.doUpdate()
#    print("pyAvidaThreadedDriver.doExit() done.")

  def doUpdate(self):
#    print("pyAvidaThreadedDriver.doUpdate()...")
    self.m_do_update_semaphore.release()
#    print("pyAvidaThreadedDriver.doUpdate() done.")

  def doSetProcessBitesize(self, bitesize):
    self.m_lock.acquire()
    self.m_process_bitesize = bitesize
    self.m_lock.release()


# Unit tests.

from pyUnitTestSuiteRecurser import *
from pyUnitTestSuite import *
from pyTestCase import *

from pmock import *

class pyUnitTestSuite_pyAvidaThreadedDriver(pyUnitTestSuite):
  def adoptUnitTests(self):
    print """
    -------------
    %s
    """ % self.__class__.__name__
    print """
    FIXME: pyAvidaThreadedDriver
    pyAvidaThreadedDriver has no unit tests.
    """
    self.adoptUnitTestSuite("pyMdtr")

    # Dummy test case.
    class crashDummy(pyTestCase):
      def test(self):
        self.test_is_true(True)
    self.adoptUnitTestCase(crashDummy())
