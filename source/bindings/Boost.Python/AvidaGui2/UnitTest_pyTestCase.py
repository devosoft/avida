"""
Unit tests of pyTestCase class.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
__all__ = ["suite"]

from pyTestCase import pyTestCase

from AvidaCore import cTextTestInterpreter

import unittest

class TestBase(unittest.TestCase):

  def test_dummy(self):
    self.assert_(True)

  def test_pyTestCase_subclassing(self):
    class aTestCase(pyTestCase):
      def test(self):
        self.test_is_true(1 == 0)
        self.test_is_true(0 == 1)
        self.test_is_true(False)
        self.test_is_true(True)

    class aTestSuite(pyTestCase):
      def __init__(self):
        pyTestCase.__init__(self)
        self.adoptTestCase(aTestCase())

    a_test_suite = aTestSuite()
    a_test_suite.runTest()
    result = a_test_suite.lastResult()
    self.assertEqual(1, result.successCount())
    self.assertEqual(4, result.totalTestCount())

    #interpreter = cTextTestInterpreter()
    #print interpreter.interpretation(result)

class RealTest(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

def suite():
  ### Was:
  #test_suite = unittest.TestSuite()
  #test_case_classes = [RealTest]
  #tests = [
  #  "test_dummy",
  #  "test_attribute_mocking",
  #  "test_introspection",
  #  "test_instantiating_a_class_by_name_string",
  #]
  #for test_case_class in test_case_classes:
  #  for test in tests:
  #    test_suite.addTest(test_case_class(test))
  #return test_suite
  ### Is now:
  test_suite = unittest.TestSuite()
  test_case_classes = [RealTest]
  test_loader = unittest.TestLoader()
  for test_case_class in test_case_classes:
    test_suite.addTests(map(test_case_class, test_loader.getTestCaseNames(test_case_class)))
  return test_suite
