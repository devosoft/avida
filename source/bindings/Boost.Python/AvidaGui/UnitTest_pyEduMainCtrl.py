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

import unittest
import qt


class TestBase(unittest.TestCase):
  def crashDummy(self):
    self.assert_(True)
    #self.assert_(False)

class MockTestCase(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

class RealTestCase(TestBase):
  def setUp(self):
    self.avida_edu_main_ctrl = pyEduMainCtrl()
  def tearDown(self):
    del self.avida_edu_main_ctrl


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
  return test_suite
