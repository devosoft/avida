"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pyEduMainCtrl.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pyEduMainCtrl; reload(pyEduMainCtrl)
pyEduMainCtrl = pyEduMainCtrl.pyEduMainCtrl

from AvidaCore import cTextTestInterpreter

import unittest
import qt


class RealTestCase(unittest.TestCase):
  def crashDummy(self):
    test_result = pyEduMainCtrl().unitTest(True)
    print cTextTestInterpreter().interpretation(test_result)
    self.assert_(test_result.successCount() == test_result.totalTestCount())

def suite():
  test_suite = unittest.TestSuite()
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
  return test_suite
