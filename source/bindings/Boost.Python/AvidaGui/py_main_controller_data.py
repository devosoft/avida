
from py_avida_core_data import *
from py_avida_state_mediator import *
from py_avida_threaded_driver import *
from AvidaCore import cString


class pyMainControllerData:

  def __init__(self):
    print("""
    FIXME : class pyMainControllerData : AvidaGui/py_main_controller_data.py :
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    """)
    self.genesis_filename = cString('genesis')
    self.avida_core_data = pyAvidaCoreData(self.genesis_filename)
    # Create threaded Avida driver.
    self.avida_threaded_driver = pyAvidaThreadedDriver(self.avida_core_data.environment)
    self.avida_state_mediator = pyAvidaStateMediator();
    
  def __del__(self):
    print("""
    FIXME : class pyMainControllerData : AvidaGui/py_main_controller_data.py :
    __del__() doesn't clean-up correctly (halt the processing thread,
    then delete it before exiting), probably because I'm locking
    per-thread rather than on the Avida library.
    """)
    # Explicit deletion, rather than reliance on refcounting, permits
    # the thread to delete itself in a thread-safe way.
    print("""
    FIXME : class pyMainControllerData : AvidaGui/py_main_controller_data.py :
    I'm using the wrong locking model in the driver threads...
    I need to lock on access to the Avida core library, rather than on
    per-thread locks (in order to protect static data in the library).
    """)
    del(self.avida_threaded_driver)
