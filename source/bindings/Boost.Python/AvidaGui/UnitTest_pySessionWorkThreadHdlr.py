"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pySessionWorkThreadHdlr.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pySessionWorkThreadHdlr; reload(pySessionWorkThreadHdlr)
pySessionWorkThreadHdlr = pySessionWorkThreadHdlr.pySessionWorkThreadHdlr
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
    self.session_mdl = pyMdl()
    self.session_mdl.avida_core_data = pmock.Mock()
    self.session_work_thread_hdlr = pySessionWorkThreadHdlr(self.session_mdl)
    pass
  def tearDown(self):
    del self.session_work_thread_hdlr
    del self.session_mdl.avida_core_data
    del self.session_mdl
    pass


def suite():
  test_suite = unittest.TestSuite()
  #test_cases = [MockTestCase, RealTestCase]
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
    #test_suite.addTest(test_case("crashDummyFailure"))
  return test_suite
