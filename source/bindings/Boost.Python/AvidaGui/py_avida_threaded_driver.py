
from AvidaCore import pyAvidaDriver
import atexit, threading

class pyAvidaThreadedDriver(pyAvidaDriver):

  def __init__(self, environment):

    pyAvidaDriver.__init__(self, environment)
    #
    self.thread = threading.Thread(target=self.___threadloop___)
    print("""
    FIXME : class pyAvidaThreadedDriver : AvidaGui/py_avida_threaded_driver.py :
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    """)
    self.lock = threading.Lock()
    self.do_update_semaphore = threading.Semaphore(0)
    self.updated_semaphore = threading.Semaphore(0)
    #
    self.doSetProcessBitesize(30)
    self.thread.start()

    # make sure that driver is deleted when python exits -- otherwise
    # its processing thread loop may continue to run when python tries
    # to exit, preventing the python process from exiting.
    atexit.register(self.__del__)

  def __del__(self):
    # ask the processing thread loop to exit.
    self.doExit()
    self.thread.join()

  def ___threadloop___(self):
    while True:
      # wait for notification by another thread before starting update.
      self.do_update_semaphore.acquire()
      # begin update.
      self.lock.acquire()
      self.updating = self.ProcessSome(self.process_bitesize)
      while self.updating:
        # yield lock to other threads.
        self.lock.release()
        self.lock.acquire()
        # continue update.
        self.updating = self.ProcessSome(self.process_bitesize)
      self.lock.release()
      # tell other threads that an update has completed (possibly
      # unsuccessfully).
      self.updated_semaphore.release()
      if self.getDoneFlag():
        return

  def doExit(self):
    self.lock.acquire()
    self.setDoneFlag();
    self.lock.release()
    self.doUpdate()

  def doUpdate(self):
    self.do_update_semaphore.release()

  def doSetProcessBitesize(self, bitesize):
    self.lock.acquire()
    self.process_bitesize = bitesize
    self.lock.release()
