
from py_recursive_tests_support import *
from pyTestCase import pyTestCase

from AvidaCore import cCycleCheck

class pyUnitTestSuiteRecurser(pyTestCase):
# test each of string, class, object as args
# test raising TypeError
# make sure that KeyError is raised if needed testing classes are missing
# refactor ckStr() and adoptTestCase() calls into a constructor method so it's easier to test this class
  def __init__(self, obj, namespace, should_recurse = False):
    pyTestCase.__init__(self)
    self.m_namespace = namespace
    self.m_class_name = extractClassName(obj)
    self.m_test_suite_class = locateClassWithPrefix(self.m_class_name, ("pyUnitTestSuite_", "cUnitTestSuite_"), self.m_namespace)
    self.m_should_recurse = should_recurse
    self.m_cycle_check = cCycleCheck(self.m_should_recurse)
  def construct(self):
    self.m_cycle_check.ckStr(self.m_class_name)
    self.adoptTestCase(self.m_test_suite_class(self.m_cycle_check, self.m_namespace).construct())
    return self
