"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pyMdl.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pyMdl; reload(pyMdl)
pyMdl = pyMdl.pyMdl

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
    self.mdl = pyMdl()
    pass
  def tearDown(self):
    del self.mdl
    pass


def suite():
  test_suite = unittest.TestSuite()
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
  return test_suite
