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
test_module_names = [
  "AvidaGui2.FunctionalTest_recursive_tests",
  "AvidaGui2.UnitTest_py_test_utils",
  "AvidaGui2.UnitTest_pyTestCase",
  "AvidaGui2.UnitTest_pyUnitTestSuiteRecurser",
  "AvidaGui2.UnitTest_pyEduMainCtrl",

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

nontest_module_names = [
  "AvidaGui2.pyAnalyzeControlsView",
  "AvidaGui2.pyAnalyzeControlsCtrl",
  "AvidaGui2.pyAvidaStatsInterface",
  "AvidaGui2.pyFreezeDialogView",
  "AvidaGui2.pyFreezeDialogCtrl",
  "AvidaGui2.pyFreezerView",
  "AvidaGui2.pyFreezerCtrl",
  "AvidaGui2.pyGradientScaleView",
  "AvidaGui2.pyGradientScaleCtrl",
  "AvidaGui2.pyGraphView",
  "AvidaGui2.pyGraphCtrl",
  "AvidaGui2.pyHardwareCPUTrace",
  "AvidaGui2.pyHardwareTracer",
  "AvidaGui2.pyInstructionSet",
  "AvidaGui2.pyLiveControlsView",
  "AvidaGui2.pyLiveControlsCtrl",
  "AvidaGui2.pyMapProfile",
  "AvidaGui2.pyNavBarView",
  "AvidaGui2.pyNavBarCtrl",
  "AvidaGui2.pyOrganismScopeView",
  "AvidaGui2.pyOrganismScopeCtrl",
  "AvidaGui2.pyPetriConfigureView",
  "AvidaGui2.pyPetriConfigureCtrl",
  "AvidaGui2.pyPetriCanvasView",
  "AvidaGui2.pyPetriDishView",
  "AvidaGui2.pyPetriDishCtrl",
  "AvidaGui2.pyPopulationCellItem",
  "AvidaGui2.pyQuitDialogView",
  "AvidaGui2.pyQuitDialogCtrl",
  "AvidaGui2.pyReadFreezer",
  "AvidaGui2.pyTimelineView",
  "AvidaGui2.pyTimelineCtrl",
  "AvidaGui2.pyWriteGenesisEvent",
  "AvidaGui2.pyWriteToFreezer",
  
  "AvidaGui2.pyOneAna_GraphView",
  "AvidaGui2.pyOneAna_GraphCtrl",
  "AvidaGui2.pyOneAna_PetriDishView",
  "AvidaGui2.pyOneAna_PetriDishCtrl",
  "AvidaGui2.pyOneAna_StatsView",
  "AvidaGui2.pyOneAna_StatsCtrl",
  "AvidaGui2.pyOneAna_TimelineView",
  "AvidaGui2.pyOneAna_TimelineCtrl",

  "AvidaGui2.pyOneOrg_ScopeView",
  "AvidaGui2.pyOneOrg_ScopeCtrl",

  "AvidaGui2.pyOnePop_GraphView",
  "AvidaGui2.pyOnePop_GraphCtrl",
  "AvidaGui2.pyOnePop_PetriDishView",
  "AvidaGui2.pyOnePop_PetriDishCtrl",
  "AvidaGui2.pyOnePop_StatsView",
  "AvidaGui2.pyOnePop_StatsCtrl",
  "AvidaGui2.pyOnePop_TimelineView",
  "AvidaGui2.pyOnePop_TimelineCtrl",

  "AvidaGui2.pyOrgSquareCtrl",

  "AvidaGui2.pyTwoPop_GraphView",
  "AvidaGui2.pyTwoPop_GraphCtrl",
  "AvidaGui2.pyTwoPop_PetriDishesView",
  "AvidaGui2.pyTwoPop_PetriDishesCtrl",
  "AvidaGui2.pyTwoPop_StatsView",
  "AvidaGui2.pyTwoPop_StatsCtrl",
  "AvidaGui2.pyTwoPop_TimelineView",
  "AvidaGui2.pyTwoPop_TimelineCtrl",

  "AvidaGui2.pyOneAnalyzeView",
  "AvidaGui2.pyOneAnalyzeCtrl",
  "AvidaGui2.pyOneOrganismView",
  "AvidaGui2.pyOneOrganismCtrl",
  "AvidaGui2.pyOnePopulationView",
  "AvidaGui2.pyOnePopulationCtrl",

  "AvidaGui2.pyTwoAnalyzeView",
  "AvidaGui2.pyTwoAnalyzeCtrl",
  "AvidaGui2.pyTwoOrganismView",
  "AvidaGui2.pyTwoOrganismCtrl",
  "AvidaGui2.pyTwoPopulationView",
  "AvidaGui2.pyTwoPopulationCtrl",

  "AvidaGui2.pyEduWorkspaceView",
  "AvidaGui2.pyEduWorkspaceCtrl",

  "AvidaGui2.pyEduSessionMenuBarHdlr",
  "AvidaGui2.pySessionDumbCtrl",
  
  "AvidaGui2.pyAvida",
#  "AvidaGui2.pyAvidaThreadHdlr",
]

module_names = nontest_module_names + test_module_names

for module_name in module_names:
  module = __import__(module_name)
  for component in module_name.split('.')[1:]:
    module = getattr(module, component)
    reload(module)
    del module

def UTs():
  for module_name in test_module_names:
    test_module = __import__(module_name)
    for component in module_name.split('.')[1:]:
      test_module = getattr(test_module, component)
      reload(test_module)
    unittest.TextTestRunner(verbosity=3).run(test_module.suite())

import AvidaGui2.FunctionalTest_recursive_tests
reload(AvidaGui2.FunctionalTest_recursive_tests)
