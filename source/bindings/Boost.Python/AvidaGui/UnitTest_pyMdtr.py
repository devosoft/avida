"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pyMdtr.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pyMdtr; reload(pyMdtr)
pyMdtr = pyMdtr.pyMdtr

import unittest
import pmock
import qt


class TestBase(unittest.TestCase):
  def crashDummy(self):
    self.assert_(True)
  def crashDummyFailure(self):
    self.assert_(False)

class MockTestCase(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

class RealTestCase(TestBase):
  def setUp(self):
    self.avida_main_mdtr = pyMdtr()
    pass
  def tearDown(self):
    del self.avida_main_mdtr
    pass


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
    #test_suite.addTest(test_case("crashDummyFailure"))
  return test_suite
