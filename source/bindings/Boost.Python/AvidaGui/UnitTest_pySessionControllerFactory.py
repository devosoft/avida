"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pySessionControllerFactory.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


#from AvidaGui.pySessionControllerFactory import pySessionControllerFactory
import pySessionControllerFactory; reload(pySessionControllerFactory)
pySessionControllerFactory = pySessionControllerFactory.pySessionControllerFactory

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
    mock = pmock.Mock()
    self.avida_edu_main_ctrl = pySessionControllerFactory(mock)
    pass
  def tearDown(self):
    del self.avida_edu_main_ctrl
    pass


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
    #test_suite.addTest(test_case("crashDummyFailure"))
  return test_suite
