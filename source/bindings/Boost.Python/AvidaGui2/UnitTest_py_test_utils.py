"""
Unit tests of py_test_utils functions and classes.
"""
__author__ = "Kaben Nanlohy"
__email__ = "kaben at users dot sourceforge dot net"
__all__ = ["suite"]


import py_test_utils; reload(py_test_utils)
from py_test_utils import *

import unittest

class TestBase(unittest.TestCase):
  print """
  FIXME: UnitTest_py_test_utils
  Add unit tests of verifyDictDelete and verifyListDelete.
  """
  def test_externally_referenced_object(self):
    # negative case -- keep_around is not deleted in verifyDelete because it has an external reference.
    class KeepAround: pass
    keep_around = KeepAround()
    self.assert_(not verifyDelete(lambda : keep_around, lambda x: x))

  def test_external_reference_held_internally(self):
    # verify that verifyDelete detects external references held internally
    class KeepAround: pass
    class IHaveReferences:
      def __init__(self, external):
        self.m_external = external
    keep_around = KeepAround()
    self.assert_(not verifyDelete(lambda : IHaveReferences(keep_around), lambda x: x.m_external))

  def test_reference_cycles(self):
    # verify that verifyDelete detects reference cycles
    class IProduceCyclicReferences:
      def __init__(self):
        class Nil: pass
        self.nil_1 = Nil()
        self.nil_2 = Nil()
        self.nil_1.nil_2 = self.nil_2
        self.nil_2.nil_1 = self.nil_1
    self.assert_(not verifyDelete(lambda : IProduceCyclicReferences(), lambda x: x.nil_1))

  def test_acceptance(self):
    # functional test of verifyDelete class
    class KeepAround: pass
    # positive case -- the KeepAround object instantiated inside of verifyDelete has no external reference, so is deleted.
    self.assert_(verifyDelete(lambda : KeepAround(), lambda x: x))

  def test_recycler(self):
    class Foo: pass
    foo_list = []
    def foo_factory():
      foo = Foo()
      foo_list.append(foo)
      return foo
    def foo_recycler(foo):
      del foo_list[foo_list.index(foo)]
      del foo
    self.assert_(verifyDelete(foo_factory, lambda x: x, foo_recycler))

class RealTest(TestBase):
  def setUp(self):
    pass
  def tearDown(self):
    pass

def suite():
  test_suite = unittest.TestSuite()
  test_case_classes = [RealTest]
  test_loader = unittest.TestLoader()
  for test_case_class in test_case_classes:
    test_suite.addTests(map(test_case_class, test_loader.getTestCaseNames(test_case_class)))
  return test_suite
