"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pyEduSessionMenuBarHdlr.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pyEduSessionMenuBarHdlr; reload(pyEduSessionMenuBarHdlr)
pyEduSessionMenuBarHdlr = pyEduSessionMenuBarHdlr.pyEduSessionMenuBarHdlr

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
    self.avida_session_ctrl = pyEduSessionMenuBarHdlr(mock)
    pass
  def tearDown(self):
    del self.avida_session_ctrl
    pass


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
    #test_suite.addTest(test_case("crashDummyFailure"))
  return test_suite
