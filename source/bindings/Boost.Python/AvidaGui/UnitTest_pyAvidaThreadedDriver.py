"""
--------------------------------------------------------------------------------
Unit-tests of AvidaGui.pyAvidaThreadedDriver.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
# Exported classes and functions:
__all__ = ["suite"]


import pyAvidaThreadedDriver; reload(pyAvidaThreadedDriver)
pyAvidaThreadedDriver = pyAvidaThreadedDriver.pyAvidaThreadedDriver
import pyAvidaCoreData; reload(pyAvidaCoreData)
pyAvidaCoreData = pyAvidaCoreData.pyAvidaCoreData

from AvidaCore import cString

import unittest
import pmock
import qt


class TestBase(unittest.TestCase):
  def crashDummy(self):
    self.assert_(True)
    self.avida_threaded_driver.doUpdate()
    #self.assert_(False)

class MockTestCase(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

class RealTestCase(TestBase):
  def setUp(self):
    self.avida_core_data = pyAvidaCoreData(cString("genesis"))
    self.avida_core_data.construct()
    self.avida_threaded_driver = pyAvidaThreadedDriver(self.avida_core_data.environment)
    self.avida_threaded_driver.construct()
    pass
  def tearDown(self):
    self.avida_threaded_driver.destruct()
    del self.avida_threaded_driver
    del self.avida_core_data
    pass


def suite():
  test_suite = unittest.TestSuite()
  test_cases = [RealTestCase]
  for test_case in test_cases:
    test_suite.addTest(test_case("crashDummy"))
  return test_suite
