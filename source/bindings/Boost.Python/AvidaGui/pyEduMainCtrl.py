"""
Main controller for AvidaEdu.

Requirements:
- On initialization,
  - Opens spash screen
  - Instantiates a global mediator
  - Instantiates a controller factory
    - connects it to global mediator
  - Instantiates menu bar handler via controller factory
    - connects it to mediator
  - Instantiates thread handler via controller factory
    - connects it to mediator
  - Instantiates close handler via controller factory
    - connects it to mediator
  - Instantiates console controller via controller factory
    - connects it to mediator
  - Instantiates preferences controller via controller factory
    - connects it to mediator
  - Closes splash screen

Problems:
- multiple avida sessions means multiple mediators, one per session.
  - what if all widgets connected to that mediator are closed?
    - must provide access to list of all sessions and their mediators.
      - for now, lets do it with a global menu of sessions. Selecting a
        session can bring forward widgets pertaining to that session.

Current paradigm:
- per session, one set of
  - session model
  - session mediator
  - session thread handler
  - session menu bar controller
  - session console controller

Terminology:
- contollers control visible widgets (views)
- handlers stand between mediator and data (models)

To Do:
- pyEduMainCtrl
- pyMdl
- pyMdtr
- pySessionCtrl

- pyMainControllerFactory

- pyEduMainMenuBarHdlr
- pyEduDefaultMenuBarView
- pyEduMainConsoleCtrlCreator
- pyEduMainConsoleCtrl
- pyEduMainConsoleView
- pyEduMainPrefsCtrlCreator
- pyEduMainPrefsCtrl
- pyEduMainPrefsView
- pyMainQuitHdlrCreator
- pyMainQuitHdlr

- pySessionThreadHdlr
- pySessionThreadMdl
- pySessionMenuBarCtrl
- pySessionMenuBarView
- pySessionConsoleCtrl
- pySessionConsoleView
- pySessionDumbCtrl
- pySessionDumbView

- pyMainSplashView

- For main model (intially)?
  - global menu bar
  - global gui controller factory
    - gui controller factory is passed a session key and gui type
  - global console
  - global prefs
  - global quit handler
- For all sessions (globally)?
  - What operations?
    - Creating a new session.
    - Getting a list of all sessions.
  - What data?
    - pyMdl
      - pyMdtr
      - Sessions dictionary
- Per-session?
  - What operations?
    - Creating a new session model.
    - New session mediator.
    - New session thread handler.
    - New session menu bar handler.
    - New session console controller and console.
  - What data?
    - every session gets reference to main mediator
    - every session gets a key

- pyMdl
  - contains global mediator pyMdtr
  - contains sessions dictionary

- pySessionCtrl
  - contains session model pySessionMdl
  - contains session mediator pyMdtr

Python niftyness:
- Adding attributes on the fly:
  class PyMdl:
    pass
  self.main_mdl = pyMdl()
  self.main_mdl.main_mdtr = pyMdtr()
  self.main_mdl.main_controller_factory = pyMainControllerFactory(self.main_mdl)

- Classes are objects:
  class pySessionCtrl(...):

  blah = pySessionCtrl
  session_ctrl = blah()
"""

from pyEduMainMenuBarHdlr import pyEduMainMenuBarHdlr
from pyMainControllerFactory import pyMainControllerFactory
from pyMdl import pyMdl
from pyMdtr import pyMdtr
from pySessionCtrl import pySessionCtrl

import qt

class pyEduMainCtrl(qt.QObject):
  def __init__(self):
    self.main_mdl = pyMdl()
    self.main_mdl.main_mdtr = pyMdtr()
    self.main_mdl.sessions_dict = {}
    self.main_mdl.main_controller_factory = pyMainControllerFactory(self.main_mdl)
    self.main_mdl.main_controllers_list = []
  def construct(self):
    self.main_mdl.main_controller_factory.construct()
    self.main_mdl.main_controller_factory.addControllerCreator("pyEduMainMenuBarHdlr", pyEduMainMenuBarHdlr)
    #self.main_mdl.main_controller_factory.addControllerCreator("pyEduMainConsoleCtrl", pyEduMainConsoleCtrlCreator())
    #self.main_mdl.main_controller_factory.addControllerCreator("pyEduMainPrefsCtrl", pyEduMainPrefsCtrlCreator())
    #self.main_mdl.main_controller_factory.addControllerCreator("pyMainQuitHdlr", pyMainQuitHdlrCreator())
    self.main_mdl.main_controller_factory.addControllerCreator("pySessionCtrl", pySessionCtrl)

    self.main_mdl.main_mdtr.main_controller_factory_mdtr.emit( qt.PYSIGNAL("newMainControllerSig"), ("pyEduMainMenuBarHdlr",))
    #self.main_mdl.main_mdtr.main_controller_factory_mdtr.emit( qt.PYSIGNAL("newMainControllerSig"), ("pySessionCtrl",))
  def __del__(self):
    print "pyEduMainCtrl.__del__()..."
    while 0 < len(self.main_mdl.main_controllers_list):
      print "pyEduMainCtrl.destruct() deleting controller:", self.main_mdl.main_controllers_list[0]
      del self.main_mdl.main_controllers_list[0]
    del self.main_mdl.main_controllers_list
    del self.main_mdl.main_controller_factory
    del self.main_mdl.main_mdtr
    del self.main_mdl
    print "pyEduMainCtrl.__del__() done."
