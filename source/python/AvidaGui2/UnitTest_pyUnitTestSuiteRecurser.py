"""
Unit tests of pyUnitTestSuiteRecurser class.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
__all__ = ["suite"]

from pyUnitTestSuiteRecurser import pyUnitTestSuiteRecurser
from pyUnitTestSuite import pyUnitTestSuite
from pyTestCase import pyTestCase

from AvidaCore import cTextTestInterpreter

import unittest

class pyUnitTestSuite_pyUTRTest(pyUnitTestSuite):
  def adoptUnitTests(self):
    self.adoptUnitTestSuite("pyUTRTest2")
    class dummy_test(pyTestCase):
      def test(self):
        self.test_is_true(False)
    self.adoptUnitTestCase(dummy_test())

class pyUnitTestSuite_pyUTRTest2(pyUnitTestSuite):
  def adoptUnitTests(self):
    class dummy_test2(pyTestCase):
      def test(self):
        self.test_is_true(False)
    self.adoptUnitTestCase(dummy_test2())

class pyUTRTest:
  def unittest(self, should_recurse = False):
    namespace = globals().copy()
    namespace.update(locals())
    utr = pyUnitTestSuiteRecurser(self, namespace, True)
    utr.construct()
    utr.runTest()
    return utr.lastResult()

class pyUTRTest2:
  def unittest(self, should_recurse = False):
    namespace = globals().copy()
    namespace.update(locals())
    utr = pyUnitTestSuiteRecurser(self, namespace, True)
    utr.construct()
    utr.runTest()
    return utr.lastResult()


class TestBase(unittest.TestCase):

  def test_dummy(self):
    self.assert_(True)

  def test_pyUnitTestSuiteRecurser(self):
    interpreter = cTextTestInterpreter()

    utr_test = pyUTRTest()
    print interpreter.interpretation(utr_test.unittest(False))
    print interpreter.interpretation(utr_test.unittest(True))

    utr_test2 = pyUTRTest2()
    print interpreter.interpretation(utr_test2.unittest(False))
    print interpreter.interpretation(utr_test2.unittest(True))


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

