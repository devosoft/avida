"""
Functional-tests of AvidaGui recursive unit testing classes.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
__all__ = ["suite"]

from AvidaCore import cCycleCheck, cID, cStringable, cTestCase, cTestProblem, cTestResult, cTextTestInterpreter

from py_recursive_tests_support import *
from pyTestCase import pyTestCase
from pyUnitTestSuite import pyUnitTestSuite
from pyUnitTestSuiteRecurser import pyUnitTestSuiteRecurser

from pmock import *
import re
import sys
import unittest

class TestBase(unittest.TestCase):

  def test_dummy(self):
    self.assert_(True)

  def test_attribute_mocking(self):
    class pyMdl: pass
    mdl = pyMdl()
    mdl.foo = 23
    mock = Mock()

    mock.expects(once()).method('__getattr__').with(eq('bar')).will(return_value(mdl))
    mdl_prime = mock.proxy().bar
    foo = mdl_prime.foo
    mock.verify()
    self.assertEqual(foo, mdl.foo)

    mock.expects(once()).bar(eq(foo)).will(return_value(mdl))
    mdl_prime = mock.bar(foo)
    mock.verify()
    self.assertEqual(mdl_prime, mdl)

  def test_instantiating_a_class_by_name_string(self):
    class pyBlah: pass
    namespace = globals().copy(); namespace.update(locals())
    try:
      blah_classname = "pyBlah"
      blah = namespace[blah_classname]()
    except KeyError, err:
      self.fail("%s : Unable to find class by name." % err)

    try:
      blah_classname = "ThereIsNoClassHavingThisName"
      blah = namespace[blah_classname]()
      self.fail("Expected to raise KeyError exception while instantiating nonexistent class '%s'." % blah_classname)
    except KeyError:
      pass

  def test_obtaining_name_of_class_of_an_object(self):
    class pyBlah: pass
    blah = pyBlah()
    self.assertEqual(blah.__class__.__name__, 'pyBlah')
    
  def test_instantiating_a_class_related_by_name(self):
    class pyUnitTestSuite_pyBlah: pass
    class pyBlah: pass
    namespace = globals().copy(); namespace.update(locals())
    blah = pyBlah()

    try:
      blah_test_suite = namespace["pyUnitTestSuite_%s" % blah.__class__.__name__]
    except KeyError, err:
      self.fail("%s : Unable to find class by name." % err)
    
  def test_extract_class_name_from_object(self):
    class One: pass
    self.assertEqual(extractClassName(One()), "One")
    
  def test_extract_class_name_from_class(self):
    class One: pass
    self.assertEqual(extractClassName(One), "One")

  def test_locate_class_with_prefix(self):
    class TestPrefix_One: pass
    class One: pass
    namespace = globals().copy()
    namespace.update(locals())
    self.assertEqual(locateClassWithPrefix("One", ("TestPrefix_",), namespace), TestPrefix_One)
    try:
      self.assertEqual(locateClassWithPrefix("Two", ("TestPrefix_",), namespace), TestPrefix2_One)
      self.fail("expected to raise NameError while searching by name for nonexistent class.")
    except NameError:
      pass

  def test_locate_class_with_prefix_search(self):
    class TestPrefix2_One: pass
    class One: pass
    namespace = globals().copy()
    namespace.update(locals())
    self.assertEqual(locateClassWithPrefix("One", ("TestPrefix_", "TestPrefix2_",), namespace), TestPrefix2_One)
    try:
      self.assertEqual(locateClassWithPrefix("Two", ("TestPrefix_", "TestPrefix2_",), namespace), TestPrefix2_One)
      self.fail("expected to raise NameError while searching by name for nonexistent class.")
    except NameError:
      pass

  def test_locate_class_base_with_prefix(self):
    class TestPrefix_One: pass
    class One: pass
    class Two(One): pass
    class Three: pass
    namespace = globals().copy()
    namespace.update(locals())
    self.assertEqual(locateClassWithPrefix("Two", ("TestPrefix_",), namespace), TestPrefix_One)
    try:
      self.assertEqual(locateClassWithPrefix("Three", ("TestPrefix_",), namespace), TestPrefix_One)
      self.fail("expected to raise NameError while searching by name for nonexistent class.")
    except NameError:
      pass

  def test_delete_really_happens(self):
    class One:
      def __init__(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    one = One(endotest)
    del one
    endotest.verify()

  def test_gc_after_locals(self):
    class TC:
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

  def test_gc_of_cTestCase(self):
    class TC(cTestCase):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    tc.runTest()
    del tc
    endotest.verify()

    endotest.expects(once()).mark_a_deletion().id("tc1")
    endotest2 = Mock()
    endotest2.expects(once()).mark_a_deletion().after("tc1", endotest)
    tc = TC()
    tc2 = TC()
    tc.setDelEndotest(endotest)
    tc2.setDelEndotest(endotest2)
    tc.adoptTestCase(tc2)
    tc.runTest()
    del tc
    del tc2
    endotest.verify()
    endotest2.verify()

  def test_gc_of_pyTestCase(self):
    class TC(pyTestCase):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    tc.runTest()
    del tc
    endotest.verify()

    endotest.expects(once()).mark_a_deletion().id("tc1")
    endotest2 = Mock()
    endotest2.expects(once()).mark_a_deletion().after("tc1", endotest)
    tc = TC()
    tc2 = TC()
    tc.setDelEndotest(endotest)
    tc2.setDelEndotest(endotest2)
    tc.adoptTestCase(tc2)
    tc.runTest()
    del tc
    del tc2
    endotest.verify()
    endotest2.verify()

  def test_gc_of_cCycleCheck(self):
    print """
    FIXME: FunctionalTest_recursive_tests.test_gc_of_cCycleCheck
    Looks like cycle-check doesn't work -- pySessionCtrl unit tests appear to be run twice in recursion.
    """
    class TC(cCycleCheck):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    tc = TC(True)
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

    endotest.expects(once()).mark_a_deletion()
    tc = TC(False)
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

  def test_gc_of_cID(self):
    class TC(cID):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    endotest = Mock()
    endotest.expects(once()).mark_a_deletion()
    tc = TC()
    tc.setDelEndotest(endotest)
    del tc
    endotest.verify()

  def test_gc_of_pyUnitTestSuite(self):
    class pyID(cID):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    class pyUnitTestSuite_Sub(pyUnitTestSuite):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()
      def adoptUnitTests(self):
        pass

    class pyTestCase_Sub(pyTestCase):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    namespace = globals()
    id_endotest = Mock()
    unit_test_suite_sub_endotest = Mock()
    test_case_sub_endotest = Mock()

    ### pyUnitTestSuite initialization only
    id = pyID()
    unit_test_suite_sub = pyUnitTestSuite_Sub(id, namespace)
    #
    id.setDelEndotest(id_endotest)
    unit_test_suite_sub.setDelEndotest(unit_test_suite_sub_endotest)
    #
    id_endotest.expects(once()).mark_a_deletion()
    unit_test_suite_sub_endotest.expects(once()).mark_a_deletion()
    #
    del unit_test_suite_sub
    del id
    #
    id_endotest.verify()
    unit_test_suite_sub_endotest.verify()

    ### pyUnitTestSuite initialization and construction
    id = pyID()
    unit_test_suite_sub = pyUnitTestSuite_Sub(id, namespace)
    #
    id.setDelEndotest(id_endotest)
    unit_test_suite_sub.setDelEndotest(unit_test_suite_sub_endotest)
    #
    id_endotest.expects(once()).mark_a_deletion()
    unit_test_suite_sub_endotest.expects(once()).mark_a_deletion()
    #
    unit_test_suite_sub.construct()
    #
    del unit_test_suite_sub
    del id
    #
    id_endotest.verify()
    unit_test_suite_sub_endotest.verify()

    ### pyUnitTestSuite initialization, construction, and runTest
    id = pyID()
    unit_test_suite_sub = pyUnitTestSuite_Sub(id, namespace)
    #
    id.setDelEndotest(id_endotest)
    unit_test_suite_sub.setDelEndotest(unit_test_suite_sub_endotest)
    #
    id_endotest.expects(once()).mark_a_deletion()
    unit_test_suite_sub_endotest.expects(once()).mark_a_deletion()
    #
    unit_test_suite_sub.construct()
    unit_test_suite_sub.runTest()
    #
    del unit_test_suite_sub
    del id

    ### pyUnitTestSuite initialization, construction, extra adopted test case, and runTest
    id = pyID()
    unit_test_suite_sub = pyUnitTestSuite_Sub(id, namespace)
    test_case_sub = pyTestCase_Sub()
    #
    id.setDelEndotest(id_endotest)
    unit_test_suite_sub.setDelEndotest(unit_test_suite_sub_endotest)
    test_case_sub.setDelEndotest(test_case_sub_endotest)
    #
    id_endotest.expects(once()).mark_a_deletion()
    unit_test_suite_sub_endotest.expects(once()).mark_a_deletion().id("unit_test_suite_sub1")
    test_case_sub_endotest.expects(once()).mark_a_deletion().after("unit_test_suite_sub1", unit_test_suite_sub_endotest)
    #
    unit_test_suite_sub.construct()
    unit_test_suite_sub.adoptTestCase(test_case_sub)
    unit_test_suite_sub.runTest()
    #
    del unit_test_suite_sub
    del test_case_sub
    del id
    #
    unit_test_suite_sub_endotest.verify()
    test_case_sub_endotest.verify()
    id_endotest.verify()


  def test_unit_test_recurser_deletes_adopted_test(self):
    class pyUnitTestSuite_One(pyUnitTestSuite):
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()
      def adoptUnitTests(self):
        pass
    class One:
      def setDelEndotest(self, endotest):
        self.m_endotest = endotest
      def __del__(self):
        self.m_endotest.mark_a_deletion()

    namespace = {"One":One, "pyUnitTestSuite_One":pyUnitTestSuite_One}
    #
    one_endotest = Mock()
    uts_endotest = Mock()
    #
    one_endotest.expects(once()).mark_a_deletion()
    uts_endotest.expects(once()).mark_a_deletion()
    #
    one = One()
    utr = pyUnitTestSuiteRecurser(one, namespace, True)
    uts = pyUnitTestSuite_One(utr.m_cycle_check, utr.m_namespace)
    #
    one.setDelEndotest(one_endotest)
    uts.setDelEndotest(uts_endotest)
    #
    # don't call construct(), but emulate it so that we can insert our own test suite instead of the one it wants to
    # construct.
    #utr.construct()
    utr.m_cycle_check.ckStr(utr.m_class_name)
    utr.adoptTestCase(uts)
    #
    uts.construct()
    utr.runTest()
    #
    del uts
    del utr
    del one
    #
    one_endotest.verify()
    uts_endotest.verify()


class MockTest(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

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
