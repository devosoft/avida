"""
Temporary convenience module to reload AvidaGui classes.

Temporary Usage::

To reload unit-test modules listed in module_names below:
>>> reload(AvidaGui.pyTemporaryReloads)

To run the unit-tests they contain:
>>> AvidaGui.pyTemporaryReloads.UTs()

Reloading a unit-test module also reloads the module it tests. (this is
so because, coincidentally, the unit-test modules are written that way.
this is fragile. beware.)
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"

import unittest

# These are imported by name so that they may be reloaded each time this
# script is run.
module_names = [
  "AvidaGui.FunctionalTest_recursive_tests",
  #"AvidaGui.UnitTest_pyAvidaCoreData",
  #"AvidaGui.UnitTest_pyAvidaThreadedDriver",
  #"AvidaGui.UnitTest_pyEduMainCtrl",
  #"AvidaGui.UnitTest_pyEduMainMenuBarHdlr",
  #"AvidaGui.UnitTest_pyEduSessionMenuBarHdlr",
  #"AvidaGui.UnitTest_pyMainControllerFactory",
  #"AvidaGui.UnitTest_pyMdl",
  #"AvidaGui.UnitTest_pyMdtr",
  #"AvidaGui.UnitTest_pySessionControllerFactory",
  #"AvidaGui.UnitTest_pySessionCtrl",
  #"AvidaGui.UnitTest_pySessionDumbCtrl",
  #"AvidaGui.UnitTest_pySessionDumbView",
  #"AvidaGui.UnitTest_pySessionWorkThreadHdlr",
]

for module_name in module_names:
  module = __import__(module_name)
  for component in module_name.split('.')[1:]:
    module = getattr(module, component)
    reload(module)

def UTs():
  for module_name in module_names:
    test_module = __import__(module_name)
    for component in module_name.split('.')[1:]:
      test_module = getattr(test_module, component)
      reload(test_module)
    unittest.TextTestRunner(verbosity=2).run(test_module.suite())

#import AvidaGui.pyEduMainCtrl
#reload(AvidaGui.pyEduMainCtrl)
